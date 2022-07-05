/**************************************************************************************/
/**                                                                                \n**/
/**             d  a  i  l  y  _  g  r  a  s  s  l  a  n  d  .  c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function of daily update of grassland stand                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grass.h"
#include "grassland.h"

static Bool isMowingDay(int aDay,const int *mowingdays,int size)
{
  int i;
  for (i=0; i < size; i++)
  {
    if (aDay == mowingdays[i])
      return TRUE;
  }
  return FALSE;
} /* of 'isMowingDay' */

Real daily_grassland(Stand *stand,                /**< stand pointer */
                     Real co2,                    /**< atmospheric CO2 (ppmv) */
                     const Dailyclimate *climate, /**< Daily climate values */
                     int day,                     /**< day (1..365) */
                     int month,                   /**< [in] month (0..11) */
                     Real daylength,              /**< length of day (h) */
                     Real gtemp_air,              /**< value of air temperature response function */
                     Real gtemp_soil,             /**< value of soil temperature response function */
                     Real eeq,                    /**< equilibrium evapotranspiration (mm) */
                     Real par,                    /**< photosynthetic active radiation flux  (J/m2/day) */
                     Real melt,                   /**< melting water (mm/day) */
                     int npft,                    /**< number of natural PFTs */
                     int ncft,                    /**< number of crop PFTs   */
                     int UNUSED(year),            /**< simulation year (AD) */
                     Bool UNUSED(intercrop),      /**< enabled intercropping */
                     Real UNUSED(agrfrac),        /**< [in] total agriculture fraction (0..1) */
                     const Config *config         /**< LPJ config */
                    )                             /** \return runoff (mm/day) */
{
  int p,l;
  Pft *pft;
  Real *gp_pft;         /**< pot. canopy conductance for PFTs & CFTs (mm/s) */
  Real gp_stand;               /**< potential stomata conductance  (mm/s) */
  Real gp_stand_leafon;        /**< pot. canopy conduct.at full leaf cover  (mm/s) */
  Real fpc_total_stand;
  Output *output;
  Harvest harvest={{0,0},{0,0},{0,0},{0,0}};
  Real aet_stand[LASTLAYER];
  Real green_transp[LASTLAYER];
  Real evap,evap_blue,rd,gpp,frac_g_evap,runoff,wet_all,intercept,sprink_interc;
  Real rw_apply; /*applied irrigation water from rainwater harvesting storage, counted as green water */
  Real cover_stand;
  Real *wet; /* wet from pftlist */
  Real return_flow_b; /* irrigation return flows from surface runoff, lateral runoff and percolation (mm)*/
  Real rainmelt,irrig_apply;
  Real intercep_stand; /* total stand interception (rain + irrigation) (mm)*/
  Real intercep_stand_blue; /* irrigation interception (mm)*/
  Real npp; /* net primary productivity (gC/m2) */
  Real gc_pft,gcgp;
  Real wdf; /* water deficit fraction */
  Real transp;
  Bool isphen;
  Grassland *data;
  Pftgrass *grass;
  Pftgrasspar *grasspar;
  Real hfrac=0.5;
  Real cleaf=0.0;
  Real cleaf_max=0.0;
  irrig_apply=0.0;
  int n_pft,index,nnat;
  Real *fpc_inc;
#ifdef PERMUTE
  int *pvec;
#endif

  n_pft=getnpft(&stand->pftlist); /* get number of established PFTs */
  nnat=getnnat(npft,config);

  data=stand->data;
  stand->growing_days++;
  output=&stand->cell->output;
  evap=evap_blue=cover_stand=intercep_stand=intercep_stand_blue=wet_all=rw_apply=intercept=sprink_interc=rainmelt=0.0;
  runoff=return_flow_b=0.0;
  if(getnpft(&stand->pftlist)>0)
  {
    wet=newvec(Real,getnpft(&stand->pftlist)); /* wet from pftlist */
    check(wet);
#ifdef PERMUTE
    pvec=newvec(int,getnpft(&stand->pftlist));
    check(pvec);
    permute(pvec,getnpft(&stand->pftlist),stand->cell->seed);
#endif
    for(p=0;p<getnpft(&stand->pftlist);p++)
      wet[p]=0;
  }
  else
  {
    wet=NULL;
#ifdef PERMUTE
    pvec=NULL;
#endif
  }
  gp_pft=newvec(Real,npft+ncft);
  check(gp_pft);
  gp_stand=gp_sum(&stand->pftlist,co2,climate->temp,par,daylength,
                  &gp_stand_leafon,gp_pft,&fpc_total_stand,config);

  if(!config->river_routing)
    irrig_amount(stand,&data->irrigation,npft,ncft,month,config);

  for(l=0;l<LASTLAYER;l++)
    aet_stand[l]=green_transp[l]=0;

  /* green water inflow */
  rainmelt=climate->prec+melt;
  if(rainmelt<0)
    rainmelt=0.0;

  index=data->irrigation.irrigation*getnirrig(ncft,config);

  if(data->irrigation.irrigation && data->irrigation.irrig_amount>epsilon)
  {
    irrig_apply=max(data->irrigation.irrig_amount-rainmelt,0);  /*irrigate only missing deficit after rain, remainder goes to stor */
    data->irrigation.irrig_stor+=data->irrigation.irrig_amount-irrig_apply;
    data->irrigation.irrig_amount=0.0;
    if(irrig_apply<1 && data->irrigation.irrig_system!=DRIP) /* min. irrigation requirement of 1mm */
    {
      data->irrigation.irrig_stor+=irrig_apply;
      irrig_apply=0.0;
    }
    else
    {
      /* write irrig_apply to output */
      getoutput(output,IRRIG,config)+=irrig_apply*stand->frac;
      stand->cell->balance.airrig+=irrig_apply*stand->frac;
#if defined IMAGE && defined COUPLED
      if(stand->cell->ml.image_data!=NULL)
      {
        stand->cell->ml.image_data->mirrwatdem[month]+=irrig_apply*stand->frac;
        stand->cell->ml.image_data->mevapotr[month] += irrig_apply*stand->frac;
      }
#endif
      if(config->pft_output_scaled)
      {
        getoutputindex(output,CFT_AIRRIG,rothers(ncft)+index,config)+=irrig_apply*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[0];
        getoutputindex(output,CFT_AIRRIG,rmgrass(ncft)+index,config)+=irrig_apply*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[1];
      }
      else
      {
        getoutputindex(output,CFT_AIRRIG,rothers(ncft)+index,config)+=irrig_apply;
        getoutputindex(output,CFT_AIRRIG,rmgrass(ncft)+index,config)+=irrig_apply;
      }
    }
  }

  /* INTERCEPTION */
#ifdef PERMUTE
  for(p=0;p<getnpft(&stand->pftlist);p++)
#else
  foreachpft(pft,p,&stand->pftlist)
#endif
  {
#ifdef PERMUTE
    pft=getpft(&stand->pftlist,pvec[p]);
#endif
    sprink_interc=(data->irrigation.irrig_system==SPRINK) ? 1 : 0;

    intercept=interception(&wet[p],pft,eeq,climate->prec+irrig_apply*sprink_interc);
    wet_all+=wet[p]*pft->fpc;
    intercep_stand_blue+=(climate->prec+irrig_apply*sprink_interc>epsilon) ? intercept*(irrig_apply*sprink_interc)/(climate->prec+irrig_apply*sprink_interc) : 0; /* blue intercept fraction */
    intercep_stand+=intercept;
  }
  irrig_apply-=intercep_stand_blue;
  rainmelt-=(intercep_stand-intercep_stand_blue);

  /* rain-water harvesting*/
  if(!data->irrigation.irrigation && config->rw_manage && rainmelt<5)
    rw_apply=rw_irrigation(stand,gp_stand,wet,eeq,config); /* Note: RWH supplementary irrigation is here considered green water */

  /* soil inflow: infiltration and percolation */
  if(irrig_apply>epsilon)
  {
    runoff+=infil_perc_irr(stand,irrig_apply,&return_flow_b,npft,ncft,config);
    /* count irrigation events*/
    getoutputindex(output,CFT_IRRIG_EVENTS,rothers(ncft)+index,config)++; /* id is consecutively counted over natural pfts, biomass, and the cfts; ids for cfts are from 12-23, that is why npft (=12) is distracted from id */
    getoutputindex(output,CFT_IRRIG_EVENTS,rmgrass(ncft)+index,config)++; /* id is consecutively counted over natural pfts, biomass, and the cfts; ids for cfts are from 12-23, that is why npft (=12) is distracted from id */
  }

  runoff+=infil_perc_rain(stand,rainmelt+rw_apply,&return_flow_b,npft,ncft,config);

  isphen = FALSE;
#ifdef PERMUTE
  for(p=0;p<getnpft(&stand->pftlist);p++)
#else
  foreachpft(pft,p,&stand->pftlist)
#endif
  {
#ifdef PERMUTE
    pft=getpft(&stand->pftlist,pvec[p]);
#endif
    pft->phen = 1.0; /* phenology is calculated from biomass */
    cover_stand+=pft->fpc*pft->phen;

    /* calculate albedo and FAPAR of PFT */
    albedo_grass(pft, stand->soil.snowheight, stand->soil.snowfraction);

/*
 *  Calculate net assimilation, i.e. gross primary production minus leaf
 *  respiration, including conversion from FPC to grid cell basis.
 *
 */
    gpp=water_stressed(pft,aet_stand,gp_stand,gp_stand_leafon,
                       gp_pft[getpftpar(pft,id)],&gc_pft,&rd,
                       &wet[p],eeq,co2,climate->temp,par,daylength,&wdf,
                       npft,ncft,config);
    if(gp_pft[getpftpar(pft,id)]>0.0)
    {
      gcgp=gc_pft/gp_pft[getpftpar(pft,id)];
      if(stand->cell->ml.landfrac[data->irrigation.irrigation].grass[0]>0.0)
      {
        getoutputindex(output,PFT_GCGP_COUNT,nnat+rothers(ncft)+index,config)++;
        getoutputindex(output,PFT_GCGP,nnat+rothers(ncft)+index,config)+=gcgp;
      }
      if(stand->cell->ml.landfrac[data->irrigation.irrigation].grass[1]>0.0)
      {
        getoutputindex(output,PFT_GCGP_COUNT,nnat+rmgrass(ncft)+index,config)++;
        getoutputindex(output,PFT_GCGP,nnat+rmgrass(ncft)+index,config)+=gcgp;
      }
    }
    npp=npp_grass(pft,gtemp_air,gtemp_soil,gpp-rd-pft->npp_bnf,config->with_nitrogen);
    getoutput(output,NPP,config)+=npp*stand->frac;
#if defined IMAGE && defined COUPLED
    stand->cell->npp_grass+=npp*stand->frac;
#endif
    stand->cell->balance.anpp+=npp*stand->frac;
    stand->cell->balance.agpp+=gpp*stand->frac;
    output->dcflux-=npp*stand->frac;
    getoutput(output,GPP,config)+=gpp*stand->frac;
    getoutput(output,FAPAR,config)+= pft->fapar * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,PHEN_TMIN,config)+= pft->fpc * pft->phen_gsi.tmin * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,PHEN_TMAX,config)+= pft->fpc * pft->phen_gsi.tmax * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,PHEN_LIGHT,config)+= pft->fpc * pft->phen_gsi.light * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,PHEN_WATER,config)+= pft->fpc * pft->phen_gsi.wscal * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,WSCAL,config)+= pft->fpc * pft->wscal * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));


    getoutputindex(output,CFT_FPAR,rothers(ncft)+index,config)+=(fpar(pft)*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[0]*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac)));
    getoutputindex(output,CFT_FPAR,rmgrass(ncft)+index,config)+=(fpar(pft)*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[1]*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac)));

    if(config->pft_output_scaled)
    {
      getoutputindex(output,PFT_NPP,nnat+rothers(ncft)+index,config)+=npp*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[0];
      getoutputindex(output,PFT_NPP,nnat+rmgrass(ncft)+index,config)+=npp*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[1];
    }
    else
    {
      getoutputindex(output,PFT_NPP,nnat+rothers(ncft)+index,config)+=npp;
      getoutputindex(output,PFT_NPP,nnat+rmgrass(ncft)+index,config)+=npp;
    }
    getoutputindex(output,PFT_LAI,nnat+rothers(ncft)+index,config)+=actual_lai_grass(pft);
    getoutputindex(output,PFT_LAI,nnat+rmgrass(ncft)+index,config)+=actual_lai_grass(pft);
    grass = pft->data;
    if(isdailyoutput_grassland(config,stand))
    {
      if(config->crop_index==ALLSTAND)
      {
        getoutput(output,D_NPP,config) += npp*stand->frac;
        getoutput(output,D_GPP,config) += gpp*stand->frac;
      }
      else
      {
        getoutput(output,D_NPP,config)+= npp;
        getoutput(output,D_GPP,config)+= gpp;

        getoutput(output,D_CROOT,config)+= grass->ind.root.carbon;
        getoutput(output,D_CLEAF,config)+= grass->ind.leaf.carbon;
        getoutput(output,D_NROOT,config)+= grass->ind.root.nitrogen;
        getoutput(output,D_NLEAF,config)+= grass->ind.leaf.nitrogen;

        getoutput(output,D_RD,config) += rd;
        getoutput(output,D_ASSIM,config) += gpp-rd-pft->npp_bnf;
      }
    }
    pft->npp_bnf=0.0;
  }
  free(gp_pft);
  /* calculate water balance */
  waterbalance(stand,aet_stand,green_transp,&evap,&evap_blue,wet_all,eeq,cover_stand,
               &frac_g_evap,config->rw_manage);
  /* allocation, turnover and harvest AFTER photosynthesis */
  if(config->with_nitrogen)
  {
    if(n_pft>0) /* nonzero? */
    {
      fpc_inc=newvec(Real,n_pft);
      check(fpc_inc);

      foreachpft(pft,p,&stand->pftlist)
      {
        grass=pft->data;
        grasspar=getpftpar(pft,data);
        if (pft->bm_inc.carbon > 5.0|| (grass->ind.leaf.carbon*pft->nind) > param.allocation_threshold|| day==NDAYYEAR)
        {
          turnover_grass(&stand->soil.litter,pft,1.0/NDAYYEAR,config);
          if(allocation_grass(&stand->soil.litter,pft,fpc_inc+p,config))
          {
            /* kill PFT from list of established PFTs */
            fpc_inc[p]=fpc_inc[getnpft(&stand->pftlist)-1]; /*moved here by W. von Bloh */
            litter_update_grass(&stand->soil.litter,pft,pft->nind,config);
            delpft(&stand->pftlist,p);
            p--; /* adjust loop variable */
          }
          else
           // pft->bm_inc.carbon=pft->bm_inc.nitrogen=0;
           pft->bm_inc.carbon=0;
         }
         else
         {
           grass->turn.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR;
           stand->soil.litter.item[pft->litter].ag.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind;
           update_fbd_grass(&stand->soil.litter,pft->par->fuelbulkdensity,grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind);
           getoutput(output,LITFALLC,config)+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind*stand->frac;
           grass->turn_litt.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind;
           grass->turn.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR;
           stand->soil.litter.item[pft->litter].ag.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind*pft->par->fn_turnover;
           getoutput(output,LITFALLN,config)+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind*pft->par->fn_turnover*stand->frac;
           grass->turn_litt.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind;

           grass->turn.root.carbon+=grass->ind.root.carbon*grasspar->turnover.root/NDAYYEAR;
           stand->soil.litter.item[pft->litter].bg.carbon+=grass->ind.root.carbon*grasspar->turnover.root/NDAYYEAR*pft->nind;
           getoutput(output,LITFALLC,config)+=grass->ind.root.carbon*grasspar->turnover.root/NDAYYEAR*pft->nind*stand->frac;
           grass->turn_litt.root.carbon+=grass->ind.root.carbon*grasspar->turnover.root/NDAYYEAR*pft->nind;
           grass->turn.root.nitrogen+=grass->ind.root.nitrogen*grasspar->turnover.root/NDAYYEAR;
           stand->soil.litter.item[pft->litter].bg.nitrogen+=grass->ind.root.nitrogen*grasspar->turnover.root/NDAYYEAR*pft->nind*pft->par->fn_turnover;
           getoutput(output,LITFALLN,config)+=grass->ind.root.nitrogen*grasspar->turnover.root/NDAYYEAR*pft->nind*pft->par->fn_turnover*stand->frac;
           grass->turn_litt.root.nitrogen+=grass->ind.root.nitrogen*grasspar->turnover.root/NDAYYEAR*pft->nind;

           grass->growing_days++;
           fpc_inc[p]=0;
         }
      }
      light(stand,fpc_inc,config);
      free(fpc_inc);
    }
  }
  else
  {
    stand->growing_days = 1;
    /* turnover must happen before allocation */
    foreachpft(pft,p,&stand->pftlist)
      turnover_grass(&stand->soil.litter,pft,(Real)stand->growing_days/NDAYYEAR,config);
    allocation_today(stand,config);
  }

  /* daily harvest check*/
  isphen=FALSE;
  hfrac=0.0;

  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    cleaf+=grass->ind.leaf.carbon;
    cleaf_max+=grass->max_leaf;
  }

  if (stand->type->landusetype != SETASIDE_RF && stand->type->landusetype != SETASIDE_IR) 
  {
    switch(stand->cell->ml.grass_scenario)
    {
      case GS_DEFAULT: // default
        if(cleaf>cleaf_max && ((config->with_nitrogen && stand->growing_days>=20) ||
           (!config->with_nitrogen && (day==31 || day==59 || day==90 || day==120 || day==151 || day==181 || day==212 || day==243 || day==273 || day==304 || day==334 || day==365))))
        {
          isphen=TRUE;
          hfrac=1-param.hfrac2/(param.hfrac2+cleaf);
          if(config->with_nitrogen)
          {
            fpc_inc=newvec(Real,n_pft);
            check(fpc_inc);
            foreachpft(pft,p,&stand->pftlist)
            {
              grass=pft->data;
              turnover_grass(&stand->soil.litter,pft,(Real)grass->growing_days/NDAYYEAR,config);
            }
            allocation_today(stand,config);
            light(stand,fpc_inc,config);
            free(fpc_inc);
          }
        }
        break;
      case GS_MOWING: // mowing
        if (isMowingDay(day,config->mowingdays,config->mowingdays_size))
        {
          if (cleaf > STUBBLE_HEIGHT_MOWING) // 5 cm or 25 g.C.m-2 threshold
            isphen=TRUE;
        }
        break;
      case GS_GRAZING_EXT: /* ext. grazing  */
        data->rotation.mode = RM_UNDEFINED;
        if (cleaf > STUBBLE_HEIGHT_GRAZING_EXT) /* minimum threshold */
        {
          isphen=TRUE;
          data->rotation.mode = RM_GRAZING;
          data->nr_of_lsus_ext = param.lsuha;
        }
        break;
      case GS_GRAZING_INT: /* int. grazing */
        data->nr_of_lsus_int = 0.0;
        if ((cleaf > STUBBLE_HEIGHT_GRAZING_INT) || (data->rotation.mode > RM_UNDEFINED)) // 7-8 cm or 40 g.C.m-2 threshold
        {
          isphen=TRUE;
          data->nr_of_lsus_int = param.lsuha;
        }
        break;
    } /* of switch */
  }
  if(isphen)
  {
    harvest=harvest_stand(output,stand,hfrac,config);

    /* return irrig_stor and irrig_amount in case of harvest */
    if(data->irrigation.irrigation)
    {
      stand->cell->discharge.dmass_lake+=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*stand->cell->coord.area*stand->frac;
      stand->cell->balance.awater_flux-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*stand->frac;
      /* pay back conveyance losses that have already been consumed by transport into irrig_stor */
      stand->cell->discharge.dmass_lake+=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*data->irrigation.conv_evap*stand->cell->coord.area*stand->frac;
      stand->cell->balance.awater_flux-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*data->irrigation.conv_evap*stand->frac;
      getoutput(output,STOR_RETURN,config)+=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*stand->frac;
      getoutput(output,CONV_LOSS_EVAP,config)-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*data->irrigation.conv_evap*stand->frac;
      getoutput(output,CONV_LOSS_DRAIN,config)-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*(1-data->irrigation.conv_evap)*stand->frac;
      stand->cell->balance.aconv_loss_evap-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*data->irrigation.conv_evap*stand->frac;
      stand->cell->balance.aconv_loss_drain-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*(1-data->irrigation.conv_evap)*stand->frac;
#if defined IMAGE && defined COUPLED
        if(stand->cell->ml.image_data!=NULL)
        {
          stand->cell->ml.image_data->mirrwatdem[month]-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*stand->frac;
          stand->cell->ml.image_data->mevapotr[month]-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*stand->frac;
        }
#endif

      if(config->pft_output_scaled)
      {
        getoutputindex(output,CFT_CONV_LOSS_EVAP,rothers(ncft)+index,config)-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*data->irrigation.conv_evap*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[0];
        getoutputindex(output,CFT_CONV_LOSS_EVAP,rmgrass(ncft)+index,config)-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*data->irrigation.conv_evap*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[1];
        getoutputindex(output,CFT_CONV_LOSS_DRAIN,rothers(ncft)+index,config)-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*(1-data->irrigation.conv_evap)*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[0];
        getoutputindex(output,CFT_CONV_LOSS_DRAIN,rmgrass(ncft)+index,config)-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*(1-data->irrigation.conv_evap)*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[1];
      }
      else
      {
        getoutputindex(output,CFT_CONV_LOSS_EVAP,rothers(ncft)+index,config)-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*data->irrigation.conv_evap;
        getoutputindex(output,CFT_CONV_LOSS_EVAP,rmgrass(ncft)+index,config)-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*data->irrigation.conv_evap;
        getoutputindex(output,CFT_CONV_LOSS_DRAIN,rothers(ncft)+index,config)-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*(1-data->irrigation.conv_evap);
        getoutputindex(output,CFT_CONV_LOSS_DRAIN,rmgrass(ncft)+index,config)-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*(1-data->irrigation.conv_evap);
      }
      data->irrigation.irrig_stor=0;
      data->irrigation.irrig_amount=0;
    } /* of if(data->irrigation.irrigation) */
    if(config->pft_output_scaled)
    {
#if defined IMAGE && defined COUPLED
      stand->cell->pft_harvest[rothers(ncft)+index]+=harvest.harvest.carbon*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[0];
      stand->cell->pft_harvest[rmgrass(ncft)+index]+=harvest.harvest.carbon*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[1];
#endif
      getoutputindex(output,PFT_HARVESTC,rothers(ncft)+index,config)+=harvest.harvest.carbon*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[0];
      getoutputindex(output,PFT_HARVESTC,rmgrass(ncft)+index,config)+=harvest.harvest.carbon*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[1];
      getoutputindex(output,PFT_HARVESTN,rothers(ncft)+index,config)+=harvest.harvest.nitrogen*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[0];
      getoutputindex(output,PFT_HARVESTN,rmgrass(ncft)+index,config)+=harvest.harvest.nitrogen*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[1];

      getoutputindex(output,PFT_RHARVESTC,rothers(ncft)+index,config)+=harvest.residual.carbon*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[0];
      getoutputindex(output,PFT_RHARVESTC,rmgrass(ncft)+index,config)+=harvest.residual.carbon*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[1];
      getoutputindex(output,PFT_RHARVESTN,rothers(ncft)+index,config)+=harvest.residual.nitrogen*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[0];
      getoutputindex(output,PFT_RHARVESTN,rmgrass(ncft)+index,config)+=harvest.residual.nitrogen*stand->cell->ml.landfrac[data->irrigation.irrigation].grass[1];
    }
    else
    {
#if defined IMAGE && defined COUPLED
      stand->cell->pft_harvest[rothers(ncft)+index]+=harvest.harvest.carbon;
      stand->cell->pft_harvest[rmgrass(ncft)+index]+=harvest.harvest.carbon;
#endif
      getoutputindex(output,PFT_HARVESTC,rothers(ncft)+index,config)+=harvest.harvest.carbon;
      getoutputindex(output,PFT_HARVESTC,rmgrass(ncft)+index,config)+=harvest.harvest.carbon;
      getoutputindex(output,PFT_HARVESTN,rothers(ncft)+index,config)+=harvest.harvest.nitrogen;
      getoutputindex(output,PFT_HARVESTN,rmgrass(ncft)+index,config)+=harvest.harvest.nitrogen;

      getoutputindex(output,PFT_RHARVESTC,rothers(ncft)+index,config)+=harvest.residual.carbon;
      getoutputindex(output,PFT_RHARVESTC,rmgrass(ncft)+index,config)+=harvest.residual.carbon;
      getoutputindex(output,PFT_RHARVESTN,rothers(ncft)+index,config)+=harvest.residual.nitrogen;
      getoutputindex(output,PFT_RHARVESTN,rmgrass(ncft)+index,config)+=harvest.residual.nitrogen;
    }
    /* harvested area */
    getoutputindex(output,CFTFRAC,rothers(ncft)+index,config)=stand->cell->ml.landfrac[data->irrigation.irrigation].grass[0];
    getoutputindex(output,CFTFRAC,rmgrass(ncft)+index,config)=stand->cell->ml.landfrac[data->irrigation.irrigation].grass[1];
  } /* of if(isphen) */

  transp=0;
  forrootsoillayer(l)
  {
    transp+=aet_stand[l]*stand->frac;
    getoutput(output,TRANSP_B,config)+=(aet_stand[l]-green_transp[l])*stand->frac;
  }
  if(isdailyoutput_grassland(config,stand))
  {
    if(config->crop_index==ALLSTAND)
    {
      getoutput(output,D_EVAP,config)+=evap*stand->frac;
      getoutput(output,D_TRANS,config)+=transp;
      getoutput(output,D_W0,config)+=stand->soil.w[1]*stand->frac;
      getoutput(output,D_W1,config)+=stand->soil.w[2]*stand->frac;
      getoutput(output,D_WEVAP,config)+=stand->soil.w[0]*stand->frac;
      getoutput(output,D_INTERC,config)+=intercep_stand*stand->frac;
    }
    else
    {
      getoutput(output,D_EVAP,config)+=evap;
      forrootsoillayer(l)
        getoutput(output,D_TRANS,config)+=aet_stand[l];
      getoutput(output,D_IRRIG,config)=irrig_apply;
      getoutput(output,D_W0,config)+=stand->soil.w[1];
      getoutput(output,D_W1,config)+=stand->soil.w[2];
      getoutput(output,D_WEVAP,config)+=stand->soil.w[0];
      getoutput(output,D_PAR,config)=par;
      getoutput(output,D_PET,config)+=eeq*PRIESTLEY_TAYLOR;
    }
  }

  if(data->irrigation.irrigation && stand->pftlist.n>0) /*second element to avoid irrigation on just harvested fields */
    calc_nir(stand,&data->irrigation,gp_stand,wet,eeq);

  getoutput(output,TRANSP,config)+=transp;
  stand->cell->balance.atransp+=transp;
  getoutput(output,INTERC,config)+=intercep_stand*stand->frac; /* Note: including blue fraction*/
  getoutput(output,INTERC_B,config)+=intercep_stand_blue*stand->frac;   /* blue interception and evap */

  getoutput(output,EVAP,config)+=evap*stand->frac;
  stand->cell->balance.aevap+=evap*stand->frac;
  stand->cell->balance.ainterc+=intercep_stand*stand->frac;
  getoutput(output,EVAP_B,config)+=evap_blue*stand->frac;   /* blue soil evap */
#if defined(IMAGE) && defined(COUPLED)
  if(stand->cell->ml.image_data!=NULL)
    stand->cell->ml.image_data->mevapotr[month] += transp + (evap + intercep_stand)*stand->frac;
#endif

  getoutput(output,RETURN_FLOW_B,config)+=return_flow_b*stand->frac; /* now only changed in waterbalance_new.c*/

  getoutputindex(output,CFT_PET,rothers(ncft)+data->irrigation.irrigation*(ncft+NGRASS),config)+=eeq*PRIESTLEY_TAYLOR;
  getoutputindex(output,CFT_TEMP,rothers(ncft)+data->irrigation.irrigation*(ncft+NGRASS),config)+= climate->temp >= 5 ? climate->temp : 0;
  getoutputindex(output,CFT_PREC,rothers(ncft)+data->irrigation.irrigation*(ncft+NGRASS),config)+=climate->prec;
  getoutputindex(output,CFT_SRAD,rothers(ncft)+data->irrigation.irrigation*(ncft+NGRASS),config)+=climate->swdown;
  getoutputindex(output,CFT_PET,rmgrass(ncft)+data->irrigation.irrigation*(ncft+NGRASS),config)+=eeq*PRIESTLEY_TAYLOR;
  getoutputindex(output,CFT_TEMP,rmgrass(ncft)+data->irrigation.irrigation*(ncft+NGRASS),config)+=climate->temp;
  getoutputindex(output,CFT_PREC,rmgrass(ncft)+data->irrigation.irrigation*(ncft+NGRASS),config)+=climate->prec;
  getoutputindex(output,CFT_SRAD,rmgrass(ncft)+data->irrigation.irrigation*(ncft+NGRASS),config)+=climate->swdown;
  foreachpft(pft, p, &stand->pftlist)
  {
    grass=pft->data;
    getoutput(output,MEANVEGCMANGRASS,config)+=vegc_sum(pft);
    if(!isannual(FPC_BFT,config))
      getoutputindex(output,FPC_BFT,getpftpar(pft, id)-(nnat-config->ngrass)+data->irrigation.irrigation*(config->nbiomass+2*config->ngrass),config)=pft->fpc;
    if(!isannual(PFT_VEGC,config))
    {
      getoutputindex(output,PFT_VEGC,nnat+rothers(ncft)+index,config)=vegc_sum(pft);
      getoutputindex(output,PFT_VEGC,nnat+rmgrass(ncft)+index,config)=vegc_sum(pft);
    }
    if(!isannual(PFT_VEGN,config))
    {
      getoutputindex(output,PFT_VEGN,nnat+rothers(ncft)+index,config)=vegn_sum(pft)+pft->bm_inc.nitrogen;
      getoutputindex(output,PFT_VEGN,nnat+rmgrass(ncft)+index,config)=vegn_sum(pft)+pft->bm_inc.nitrogen;
    }
    if(!isannual(PFT_CLEAF,config))
    {
      getoutputindex(output,PFT_CLEAF,nnat+rothers(ncft)+index,config)=grass->ind.leaf.carbon;
      getoutputindex(output,PFT_CLEAF,nnat+rmgrass(ncft)+index,config)=grass->ind.leaf.carbon;
    }
    if(!isannual(PFT_NLEAF,config))
    {
      getoutputindex(output,PFT_NLEAF,nnat+rothers(ncft)+index,config)=grass->ind.leaf.nitrogen;
      getoutputindex(output,PFT_NLEAF,nnat+rmgrass(ncft)+index,config)=grass->ind.leaf.nitrogen;
    }
    if(!isannual(PFT_CROOT,config))
    {
      getoutputindex(output,PFT_CROOT,nnat+rothers(ncft)+index,config)=grass->ind.root.carbon;
      getoutputindex(output,PFT_CROOT,nnat+rmgrass(ncft)+index,config)=grass->ind.root.carbon;
    }
    if(!isannual(PFT_NROOT,config))
    {
      getoutputindex(output,PFT_NROOT,nnat+rothers(ncft)+index,config)=grass->ind.root.nitrogen;
      getoutputindex(output,PFT_NROOT,nnat+rmgrass(ncft)+index,config)=grass->ind.root.nitrogen;
    }

  }

  /* output for green and blue water for evaporation, transpiration and interception */
  output_gbw_grassland(output,stand,frac_g_evap,evap,evap_blue,return_flow_b,aet_stand,green_transp,
                       intercep_stand,intercep_stand_blue,ncft,config);
  free(wet);
#ifdef PERMUTE
  free(pvec);
#endif
  return runoff;
} /* of 'daily_grassland' */

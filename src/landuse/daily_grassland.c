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
#include "agriculture.h"

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
                     int year,                    /**< simulation year (AD) */
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
  Real vol_water_enth; /* volumetric enthalpy of water (J/m3) */
  Bool isphen;
  Grassland *data;
  Pftgrass *grass;
  Pftgrasspar *grasspar;
  Real hfrac=0.5;
  Real cleaf=0.0;
  Real cleaf_max=0.0;
  Real fertil;
  Real manure;
  int n_pft,index,nnat;
  Real *fpc_inc;
  Real lateral_in=0;
#ifdef PERMUTE
  int *pvec;
#endif
  irrig_apply=0.0;

  n_pft=getnpft(&stand->pftlist); /* get number of established PFTs */
  nnat=getnnat(npft,config);

  data=stand->data;
  stand->growing_days++;
  output=&stand->cell->output;
  evap=evap_blue=cover_stand=intercep_stand=intercep_stand_blue=wet_all=rw_apply=intercept=sprink_interc=rainmelt=0.0;
  runoff=return_flow_b=0.0;
#ifdef CHECK_BALANCE
  Real wfluxes_old=(stand->cell->balance.excess_water+stand->cell->lateral_water+stand->cell->balance.awater_flux+stand->cell->balance.aevap_res+stand->cell->balance.aevap_lake-stand->cell->balance.aMT_water);
  Real wstore_old=(stand->cell->discharge.dmass_lake+stand->cell->discharge.dmass_river)/stand->cell->coord.area;
  Real water_before=0;
  Real water_after,wstore_new,balancew;
  water_before+=soilwater(&stand->soil);
#endif
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
  if (stand->type->landusetype==GRASSLAND  || stand->type->landusetype==OTHERS)
  {
    if(stand->cell->ml.fertilizer_nr!=NULL) /* has to be adapted if fix_fertilization option is added */
    {
      if(day==fertday_biomass(stand->cell,config))
      {
        fertil = stand->cell->ml.fertilizer_nr[data->irrigation.irrigation].grass[stand->type->landusetype==GRASSLAND];
        stand->soil.NO3[0]+=fertil*param.nfert_no3_frac;
        stand->soil.NH4[0]+=fertil*(1-param.nfert_no3_frac);
        stand->cell->balance.influx.nitrogen+=fertil*stand->frac;
        if(stand->type->landusetype==OTHERS)
          getoutput(output,NFERT_AGR,config)+=fertil*stand->frac;
        getoutput(output,NAPPLIED_MG,config)+=fertil*stand->frac;
      } /* end fday==day */
    }
    if(stand->cell->ml.manure_nr!=NULL) /* has to be adapted if fix_fertilization option is added */
    {
      if(day==fertday_biomass(stand->cell,config) && stand->soil.litter.n>0)
      {
        manure = stand->cell->ml.manure_nr[data->irrigation.irrigation].grass[stand->type->landusetype==GRASSLAND];
        stand->soil.NH4[0] += manure*param.nmanure_nh4_frac;
        stand->soil.litter.item->agsub.leaf.carbon += manure*param.manure_cn;
        stand->soil.litter.item->agsub.leaf.nitrogen += manure*(1-param.nmanure_nh4_frac);
        stand->cell->balance.influx.carbon += manure*param.manure_cn*stand->frac;
        stand->cell->balance.influx.nitrogen += manure*stand->frac;
        if(stand->type->landusetype==OTHERS)
          getoutput(output,NMANURE_AGR,config)+=manure*stand->frac;
        getoutput(output,NAPPLIED_MG,config)+=manure*stand->frac;
      } /* end fday==day */
    }
  }

  /* green water inflow */
  rainmelt=climate->prec+melt;
  if(rainmelt<0)
    rainmelt=0.0;

  index=data->irrigation.irrigation*getnirrig(ncft,config)+(stand->type->landusetype==GRASSLAND ? rmgrass(ncft) : rothers(ncft));

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
        getoutputindex(output,CFT_AIRRIG,index,config)+=irrig_apply*stand->frac;
      }
      else
      {
        getoutputindex(output,CFT_AIRRIG,index,config)+=irrig_apply;
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
  if(irrig_apply<0)
    intercep_stand_blue+=irrig_apply;
  irrig_apply=max(0,irrig_apply);
  rainmelt-=intercep_stand;

  /* rain-water harvesting*/
  if(!data->irrigation.irrigation && config->rw_manage && rainmelt<5)
    rw_apply=rw_irrigation(stand,gp_stand,wet,eeq,config); /* Note: RWH supplementary irrigation is here considered green water */

  /* soil inflow: infiltration and percolation */
  if(irrig_apply>epsilon)
  {
    /* count irrigation events*/
    getoutputindex(output,CFT_IRRIG_EVENTS,index,config)++; /* id is consecutively counted over natural pfts, biomass, and the cfts; ids for cfts are from 12-23, that is why npft (=12) is distracted from id */
  }

  if((climate->prec+melt+rw_apply+irrig_apply)>0) /* enthalpy of soil infiltration */
    vol_water_enth = climate->temp*c_water*(climate->prec+rw_apply+irrig_apply)/(climate->prec+irrig_apply+irrig_apply+melt)+c_water2ice;
  else
    vol_water_enth=0;

  if(stand->cell->lateral_water>0 && stand->soil.iswetland==TRUE && stand->frac>0.001)
  {
    if(stand->cell->lateral_water/stand->frac>300)
      lateral_in=300;
    else
      lateral_in=stand->cell->lateral_water/stand->frac;

    runoff+=infil_perc(stand,rainmelt+rw_apply+irrig_apply+lateral_in, vol_water_enth,climate->prec+rw_apply+irrig_apply,&return_flow_b,npft,ncft,config);     //enthalpy of lateral influx?? should be the same T as in the local stand
    stand->cell->lateral_water-=lateral_in*stand->frac;
  }
  else
    runoff+=infil_perc(stand,rainmelt+rw_apply+irrig_apply, vol_water_enth,climate->prec+rw_apply+irrig_apply,&return_flow_b,npft,ncft,config);

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
                       nnat+index,npft,ncft,config);
    getoutput(output,RD,config)+=rd*stand->frac;
    if(gp_pft[getpftpar(pft,id)]>0.0)
    {
      gcgp=gc_pft/gp_pft[getpftpar(pft,id)];
      if(stand->frac>0.0)
      {
        getoutputindex(output,PFT_GCGP_COUNT,nnat+index,config)++;
        getoutputindex(output,PFT_GCGP,nnat+index,config)+=gcgp;
      }
    }
    npp=npp_grass(pft,gtemp_air,gtemp_soil,gpp-rd-pft->npp_bnf,config);
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
    getoutput(output,RA_MGRASS,config)+=(gpp-npp)*stand->frac;

    getoutputindex(output,CFT_FPAR,index,config)+=(fpar(pft)*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac)));

    if(config->pft_output_scaled)
    {
      getoutputindex(output,PFT_NPP,nnat+index,config)+=npp*stand->frac;
    }
    else
    {
      getoutputindex(output,PFT_NPP,nnat+index,config)+=npp;
    }
    getoutputindex(output,PFT_LAI,nnat+index,config)+=actual_lai_grass(pft);
    grass = pft->data;
    pft->npp_bnf=0.0;
  }
  free(gp_pft);
  /* calculate water balance */
  waterbalance(stand,aet_stand,green_transp,&evap,&evap_blue,wet_all,eeq,cover_stand,
               &frac_g_evap,config->rw_manage);
  /* allocation, turnover and harvest AFTER photosynthesis */
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
        stand->soil.litter.item[pft->litter].agtop.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind;
        update_fbd_grass(&stand->soil.litter,pft->par->fuelbulkdensity,grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind);
        getoutput(output,LITFALLC,config)+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind*stand->frac;
        grass->turn_litt.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind;
        grass->turn.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR;
        stand->soil.litter.item[pft->litter].agtop.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind*pft->par->fn_turnover;
        pft->bm_inc.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind*(1-pft->par->fn_turnover);
        getoutput(output,LITFALLN,config)+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind*pft->par->fn_turnover*stand->frac;
        grass->turn_litt.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind;

        grass->turn.root.carbon+=grass->ind.root.carbon*grasspar->turnover.root/NDAYYEAR;
        stand->soil.litter.item[pft->litter].bg.carbon+=grass->ind.root.carbon*grasspar->turnover.root/NDAYYEAR*pft->nind;
        getoutput(output,LITFALLC,config)+=grass->ind.root.carbon*grasspar->turnover.root/NDAYYEAR*pft->nind*stand->frac;
        grass->turn_litt.root.carbon+=grass->ind.root.carbon*grasspar->turnover.root/NDAYYEAR*pft->nind;
        grass->turn.root.nitrogen+=grass->ind.root.nitrogen*grasspar->turnover.root/NDAYYEAR;
        stand->soil.litter.item[pft->litter].bg.nitrogen+=grass->ind.root.nitrogen*grasspar->turnover.root/NDAYYEAR*pft->nind*pft->par->fn_turnover;
        pft->bm_inc.nitrogen+=grass->ind.root.nitrogen*grasspar->turnover.root/NDAYYEAR*pft->nind*(1-pft->par->fn_turnover);
        getoutput(output,LITFALLN,config)+=grass->ind.root.nitrogen*grasspar->turnover.root/NDAYYEAR*pft->nind*pft->par->fn_turnover*stand->frac;
        grass->turn_litt.root.nitrogen+=grass->ind.root.nitrogen*grasspar->turnover.root/NDAYYEAR*pft->nind;

        grass->growing_days++;
        fpc_inc[p]=0;
      }
    }
    light(stand,fpc_inc,config);
    free(fpc_inc);
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
    switch(stand->type->landusetype==GRASSLAND ? stand->cell->ml.grass_scenario : config->grazing_others)
    {
      case GS_DEFAULT: // default
        if(cleaf>cleaf_max && stand->growing_days>=20)
        {
          isphen=TRUE;
          hfrac=1-param.hfrac2/(param.hfrac2+cleaf);
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
        }
        break;
      case GS_GRAZING_INT: /* int. grazing */
        if ((cleaf > STUBBLE_HEIGHT_GRAZING_INT) || (data->rotation.mode > RM_UNDEFINED)) // 7-8 cm or 40 g.C.m-2 threshold
        {
          isphen=TRUE;
        }
        break;
      case GS_GRAZING_LIVE: /* livestock grazing */
        isphen=TRUE;
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
        getoutputindex(output,CFT_CONV_LOSS_EVAP,index,config)-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*data->irrigation.conv_evap*stand->frac;
        getoutputindex(output,CFT_CONV_LOSS_DRAIN,index,config)-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*(1-data->irrigation.conv_evap)*stand->frac;
      }
      else
      {
        getoutputindex(output,CFT_CONV_LOSS_EVAP,index,config)-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*data->irrigation.conv_evap;
        getoutputindex(output,CFT_CONV_LOSS_DRAIN,index,config)-=(data->irrigation.irrig_stor+data->irrigation.irrig_amount)*(1/data->irrigation.ec-1)*(1-data->irrigation.conv_evap);
      }
      data->irrigation.irrig_stor=0;
      data->irrigation.irrig_amount=0;
    } /* of if(data->irrigation.irrigation) */
    if(config->pft_output_scaled)
    {
#if defined IMAGE && defined COUPLED
      stand->cell->pft_harvest[index]+=harvest.harvest.carbon*stand->frac;
#endif
      getoutputindex(output,PFT_HARVESTC,index,config)+=harvest.harvest.carbon*stand->frac;
      getoutputindex(output,PFT_HARVESTN,index,config)+=harvest.harvest.nitrogen*stand->frac;

      getoutputindex(output,PFT_RHARVESTC,index,config)+=harvest.residual.carbon*stand->frac;
      getoutputindex(output,PFT_RHARVESTN,index,config)+=harvest.residual.nitrogen*stand->frac;
    }
    else
    {
#if defined IMAGE && defined COUPLED
      stand->cell->pft_harvest[index]+=harvest.harvest.carbon;
#endif
      getoutputindex(output,PFT_HARVESTC,index,config)+=harvest.harvest.carbon;
      getoutputindex(output,PFT_HARVESTN,index,config)+=harvest.harvest.nitrogen;

      getoutputindex(output,PFT_RHARVESTC,index,config)+=harvest.residual.carbon;
      getoutputindex(output,PFT_RHARVESTN,index,config)+=harvest.residual.nitrogen;
    }
    /* harvested area */
    getoutputindex(output,CFTFRAC,index,config)=stand->frac;
    getoutputindex(output,CFT_NHARVEST,index,config)+=1.0;
  } /* of if(isphen) */

  transp=0;
  forrootsoillayer(l)
  {
    transp+=aet_stand[l]*stand->frac;
    getoutput(output,TRANSP_B,config)+=(aet_stand[l]-green_transp[l])*stand->frac;
  }

  if(data->irrigation.irrigation && stand->pftlist.n>0) /*second element to avoid irrigation on just harvested fields */
    calc_nir(stand,&data->irrigation,gp_stand,wet,eeq,config->others_to_crop);

  getoutput(output,TRANSP,config)+=transp;
  stand->cell->balance.atransp+=transp;
  getoutput(output,INTERC,config)+=intercep_stand*stand->frac; /* Note: including blue fraction*/
  getoutput(output,INTERC_B,config)+=intercep_stand_blue*stand->frac;   /* blue interception and evap */
  stand->cell->balance.ainterc+=(intercep_stand+intercep_stand_blue)*stand->frac;

  getoutput(output,EVAP,config)+=evap*stand->frac;
  stand->cell->balance.aevap+=evap*stand->frac;
  getoutput(output,EVAP_B,config)+=evap_blue*stand->frac;   /* blue soil evap */
#if defined(IMAGE) && defined(COUPLED)
  if(stand->cell->ml.image_data!=NULL)
    stand->cell->ml.image_data->mevapotr[month] += transp + (evap + intercep_stand)*stand->frac;
#endif

  getoutput(output,RETURN_FLOW_B,config)+=return_flow_b*stand->frac; /* now only changed in waterbalance_new.c*/

  getoutputindex(output,CFT_PET,(stand->type->landusetype==GRASSLAND ? rmgrass(ncft) : rothers(ncft))+data->irrigation.irrigation*(ncft+NGRASS),config)+=eeq*PRIESTLEY_TAYLOR;
  getoutputindex(output,CFT_TEMP,(stand->type->landusetype==GRASSLAND ? rmgrass(ncft) : rothers(ncft))+data->irrigation.irrigation*(ncft+NGRASS),config)+= climate->temp >= 5 ? climate->temp : 0;
  getoutputindex(output,CFT_PREC,(stand->type->landusetype==GRASSLAND ? rmgrass(ncft) : rothers(ncft))+data->irrigation.irrigation*(ncft+NGRASS),config)+=climate->prec;
  getoutputindex(output,CFT_SRAD,(stand->type->landusetype==GRASSLAND ? rmgrass(ncft) : rothers(ncft))+data->irrigation.irrigation*(ncft+NGRASS),config)+=climate->swdown;
  foreachpft(pft, p, &stand->pftlist)
  {
    grass=pft->data;
    if(stand->type->landusetype==GRASSLAND)
    {
      getoutput(output,MEANVEGCMANGRASS,config)+=vegc_sum(pft);
      if(!isannual(FPC_BFT,config))
        getoutputindex(output,FPC_BFT,getpftpar(pft, id)-(nnat-config->ngrass)+data->irrigation.irrigation*(config->nbiomass+2*config->ngrass),config)=pft->fpc;
    }
    if(!isannual(PFT_VEGC,config))
    {
      getoutputindex(output,PFT_VEGC,nnat+index,config)=vegc_sum(pft);
    }
    if(!isannual(PFT_VEGN,config))
    {
      getoutputindex(output,PFT_VEGN,nnat+index,config)=vegn_sum(pft)+pft->bm_inc.nitrogen;
    }
    if(!isannual(PFT_CLEAF,config))
    {
      getoutputindex(output,PFT_CLEAF,nnat+index,config)=grass->ind.leaf.carbon;
    }
    if(!isannual(PFT_NLEAF,config))
    {
      getoutputindex(output,PFT_NLEAF,nnat+index,config)=grass->ind.leaf.nitrogen;
    }
    if(!isannual(PFT_CROOT,config))
    {
      getoutputindex(output,PFT_CROOT,nnat+index,config)=grass->ind.root.carbon;
    }
    if(!isannual(PFT_NROOT,config))
    {
      getoutputindex(output,PFT_NROOT,nnat+index,config)=grass->ind.root.nitrogen;
    }

  }

#ifdef CHECK_BALANCE
  transp=0;
  water_after=0;
  wstore_new=(stand->cell->discharge.dmass_lake+stand->cell->discharge.dmass_river)/stand->cell->coord.area;
  water_after+=soilwater(&stand->soil);
  Real wfluxes_new=(stand->cell->balance.excess_water+stand->cell->lateral_water+stand->cell->balance.awater_flux+stand->cell->balance.aevap_res+stand->cell->balance.aevap_lake-stand->cell->balance.aMT_water);
  forrootsoillayer(l)
   transp+=aet_stand[l];
  balancew=water_after-water_before-(climate->prec+melt+rw_apply+irrig_apply)+(transp+evap+intercep_stand+runoff)+(wfluxes_new-wfluxes_old)/stand->frac+(wstore_new-wstore_old)/stand->frac;
  if(fabs(balancew)>0.001 && stand->frac>0.00001)
  {

    fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid water balance in %s:  y: %d day: %d balanceW: %g water_before: %.6f water_after: %.6f aet: %g evap: %g intercep_stand %g runoff: %g influx: %g fluxes: %g irrig_apply: %g irrig_stor: %g frac: %g wstore: %g wstore_new: %g wstore_old: %g \n",
        __FUNCTION__,year,day,balancew,water_before,water_after,transp,evap,intercep_stand,runoff,(climate->prec+melt+rw_apply+irrig_apply),(wfluxes_new-wfluxes_old)/stand->frac,irrig_apply,data->irrigation.irrig_stor,stand->frac,(wstore_new-wstore_old)/stand->frac,wstore_new,wstore_old);
  }
 #endif

  /* output for green and blue water for evaporation, transpiration and interception */
  output_gbw(output,stand,frac_g_evap,evap,evap_blue,return_flow_b,aet_stand,green_transp,
             intercep_stand,intercep_stand_blue,index,data->irrigation.irrigation,config);
  free(wet);
#ifdef PERMUTE
  free(pvec);
#endif
  return runoff;
} /* of 'daily_grassland' */

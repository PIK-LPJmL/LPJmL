/**************************************************************************************/
/**                                                                                \n**/
/**     d  a  i  l  y  _  b  i  o  m  a  s  s  _  g  r  a  s  s  .  c              \n**/
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
#include "biomass_grass.h"

Real daily_biomass_grass(Stand *stand,                /**< stand pointer */
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
  int p,l,n_pft,nnat;
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
  Real *wet; /* wet from pftlist */
  Real return_flow_b; /* irrigation return flows from surface runoff, lateral runoff and percolation (mm)*/
  Real cover_stand;
  Real rainmelt,irrig_apply;
  Real intercep_stand; /* total stand interception (rain + irrigation) (mm)*/
  Real intercep_stand_blue; /* irrigation interception (mm)*/
  Soil *soil;
  Real npp; /* net primary productivity (gC/m2) */
  Real gc_pft,gcgp;
  Real wdf; /* water deficit fraction */
  Real transp;
  Bool isphen;
  Irrigation *data;
  Pftgrass *grass;
  Pftgrasspar *grasspar;
  Real *fpc_inc;
  int index;
  irrig_apply=0.0;

  soil = &stand->soil;
  data=stand->data;
  output=&stand->cell->output;
  evap=evap_blue=cover_stand=intercep_stand=intercep_stand_blue=runoff=return_flow_b=wet_all=intercept=sprink_interc=0;
  n_pft=getnpft(&stand->pftlist);
  if(n_pft>0)
  {
    wet=newvec(Real,getnpft(&stand->pftlist)); /* wet from pftlist */
    check(wet);
    for(p=0;p<getnpft(&stand->pftlist);p++)
      wet[p]=0;
  }
  else
    wet=NULL;
  gp_pft=newvec(Real,npft+ncft);
  check(gp_pft);
  gp_stand=gp_sum(&stand->pftlist,co2,climate->temp,par,daylength,
                  &gp_stand_leafon,gp_pft,&fpc_total_stand,config);

  if(!config->river_routing)
    irrig_amount(stand,data,npft,ncft,month,config);

  for(l=0;l<LASTLAYER;l++)
    aet_stand[l]=green_transp[l]=0;

  /* green water inflow */
  rainmelt=climate->prec+melt;
  if(rainmelt<0)
    rainmelt=0.0;
  nnat=getnnat(npft,config);
  index=rbgrass(ncft)+data->irrigation*getnirrig(ncft,config);

  if(data->irrigation && data->irrig_amount>epsilon)
  {
    irrig_apply=max(data->irrig_amount-rainmelt,0);  /*irrigate only missing deficit after rain, remainder goes to stor */
    data->irrig_stor+=data->irrig_amount-irrig_apply;
    data->irrig_amount=0.0;
    if(irrig_apply<1 && data->irrig_system!=DRIP) /* min. irrigation requirement of 1mm */
    {
      data->irrig_stor+=irrig_apply;
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
        getoutputindex(output,CFT_AIRRIG,index,config)+=irrig_apply*stand->cell->ml.landfrac[1].biomass_grass;
      else
        getoutputindex(output,CFT_AIRRIG,index,config)+=irrig_apply;
    }
  }

  /* INTERCEPTION */
  foreachpft(pft,p,&stand->pftlist)
  {
    sprink_interc=(data->irrig_system==SPRINK) ? 1 : 0;

    intercept=interception(&wet[p],pft,eeq,climate->prec+irrig_apply*sprink_interc);
    wet_all+=wet[p]*pft->fpc;
    intercep_stand_blue+=(climate->prec+irrig_apply*sprink_interc>epsilon) ? intercept*(irrig_apply*sprink_interc)/(climate->prec+irrig_apply*sprink_interc) : 0; /* blue intercept fraction */
    intercep_stand+=intercept;
  }
  irrig_apply-=intercep_stand_blue;
  rainmelt-=(intercep_stand-intercep_stand_blue);
  irrig_apply=max(0,irrig_apply);

  /* soil inflow: infiltration and percolation */
  if(irrig_apply>epsilon)
  {
    /* count irrigation events*/
    getoutputindex(output,CFT_IRRIG_EVENTS,index,config)++; /* id is consecutively counted over natural pfts, biomass, and the cfts; ids for cfts are from 12-23, that is why npft (=12) is distracted from id */
  }

  runoff+=infil_perc(stand,rainmelt+irrig_apply,&return_flow_b,npft,ncft,config);

  isphen=FALSE;
  foreachpft(pft,p,&stand->pftlist)
  {
    leaf_phenology(pft,climate->temp,day,climate->isdailytemp,config);
    cover_stand+=pft->fpc*pft->phen;

    /* calculate albedo and FAPAR of PFT */
    albedo_grass(pft, soil->snowheight, soil->snowfraction);

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
      if(stand->cell->ml.landfrac[data->irrigation].biomass_grass>0.0)
      {
        getoutputindex(output,PFT_GCGP_COUNT,nnat+index,config)++;
        getoutputindex(output,PFT_GCGP,nnat+index,config)+=gcgp;
      }
    }
    npp=npp_grass(pft,gtemp_air,gtemp_soil,gpp-rd-pft->npp_bnf,config->with_nitrogen);
    pft->npp_bnf=0.0;
    if(config->crop_index==ALLSTAND)
    {
      getoutput(output,D_NPP,config)+=npp*stand->frac;
      getoutput(output,D_GPP,config)+=gpp*stand->frac;
    }
    getoutput(output,NPP,config)+=npp*stand->frac;
    stand->cell->balance.anpp+=npp*stand->frac;
    stand->cell->balance.agpp+=gpp*stand->frac;
    output->dcflux-=npp*stand->frac;
    getoutput(output,GPP,config)+=gpp*stand->frac;
    getoutput(output,FAPAR,config) += pft->fapar * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,PHEN_TMIN,config) += pft->fpc * pft->phen_gsi.tmin * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,PHEN_TMAX,config) += pft->fpc * pft->phen_gsi.tmax * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,PHEN_LIGHT,config) += pft->fpc * pft->phen_gsi.light * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,PHEN_WATER,config) += pft->fpc * pft->phen_gsi.wscal * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,WSCAL,config) += pft->fpc * pft->wscal * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));

    getoutputindex(output,CFT_FPAR,index,config)+=(fpar(pft)*stand->cell->ml.landfrac[data->irrigation].biomass_grass*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac)));

    if(config->pft_output_scaled)
      getoutputindex(output,PFT_NPP,nnat+index,config)+=npp*stand->cell->ml.landfrac[data->irrigation].biomass_grass;
    else
      getoutputindex(output,PFT_NPP,nnat+index,config)+=npp;
    getoutputindex(output,PFT_LAI,nnat+index,config)+=actual_lai(pft);
  } /* of foreachpft */

  /* calculate water balance */
  free(gp_pft);
  waterbalance(stand,aet_stand,green_transp,&evap,&evap_blue,wet_all,eeq,cover_stand,
               &frac_g_evap,config->rw_manage);

  /* allocation, turnover and harvest AFTER photosynthesis */
  if(n_pft>0)
  {
    fpc_inc=newvec(Real,n_pft);
    check(fpc_inc);

    foreachpft(pft,p,&stand->pftlist)
    {
      grass=pft->data;
      grasspar=getpftpar(pft,data);
      if (pft->bm_inc.carbon > 5.0|| day==NDAYYEAR)
      {
        turnover_grass(&stand->soil.litter,pft,(Real)grass->growing_days/NDAYYEAR,config);
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
         grass->turn.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR;
         grass->turn_litt.leaf.carbon+=grass->ind.leaf.carbon*grasspar->turnover.leaf/NDAYYEAR*pft->nind;
         grass->turn_litt.leaf.nitrogen+=grass->ind.leaf.nitrogen*grasspar->turnover.leaf/NDAYYEAR*pft->nind;

         grass->turn.root.carbon+=grass->ind.root.carbon*grasspar->turnover.root/NDAYYEAR;
         grass->turn.root.nitrogen+=grass->ind.root.nitrogen*grasspar->turnover.root/NDAYYEAR;
         grass->turn_litt.root.carbon+=grass->ind.root.carbon*grasspar->turnover.root/NDAYYEAR*pft->nind;
         grass->turn_litt.root.nitrogen+=grass->ind.root.nitrogen*grasspar->turnover.root/NDAYYEAR*pft->nind;

         grass->growing_days++;
         fpc_inc[p]=0;
       }
    }
    light(stand,fpc_inc,config);
    free(fpc_inc);
  }
  /* daily turnover and harvest check*/
  isphen=FALSE;
  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    grass->max_leaf=max(grass->max_leaf,grass->ind.leaf.carbon);
    if (grass->ind.leaf.carbon >= 350 || (grass->ind.leaf.carbon>1 && grass->ind.leaf.carbon<(0.75*grass->max_leaf))) //changed from 100 to 350
      isphen=TRUE;
  } /* of foreachpft() */

  if(isphen)
  {
    harvest=harvest_stand(output,stand,0.75,config);
    if(data->irrigation)
    {
      stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*stand->cell->coord.area*stand->frac;
      stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*stand->frac;
      getoutput(output,STOR_RETURN,config)+=(data->irrig_stor+data->irrig_amount)*stand->frac;

      /* pay back conveyance losses that have already been consumed by transport into irrig_stor */
      stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->coord.area*stand->frac;
      stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
      getoutput(output,CONV_LOSS_EVAP,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
      stand->cell->balance.aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
      getoutput(output,CONV_LOSS_DRAIN,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
      stand->cell->balance.aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
#if defined IMAGE && defined COUPLED
      if(stand->cell->ml.image_data!=NULL)
      {
        stand->cell->ml.image_data->mirrwatdem[month]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
        stand->cell->ml.image_data->mevapotr[month]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
      }
#endif

      if(config->pft_output_scaled)
      {
        getoutputindex(output,CFT_CONV_LOSS_EVAP,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->ml.landfrac[1].biomass_grass;
        getoutputindex(output,CFT_CONV_LOSS_DRAIN,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->cell->ml.landfrac[1].biomass_grass;
      }
      else
      {
        getoutputindex(output,CFT_CONV_LOSS_EVAP,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
        getoutputindex(output,CFT_CONV_LOSS_DRAIN,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
      }

      data->irrig_stor=0;
      data->irrig_amount=0;
    }
    if(config->pft_output_scaled)
    {
      getoutputindex(output,PFT_HARVESTC,index,config)+=harvest.harvest.carbon*stand->cell->ml.landfrac[data->irrigation].biomass_grass;
      getoutputindex(output,PFT_HARVESTN,index,config)+=harvest.harvest.nitrogen*stand->cell->ml.landfrac[data->irrigation].biomass_grass;
    }
    else
    {
      getoutputindex(output,PFT_HARVESTC,index,config)+=harvest.harvest.carbon;
      getoutputindex(output,PFT_HARVESTN,index,config)+=harvest.harvest.nitrogen;
    }
    /* harvested area */
    getoutputindex(output,CFTFRAC,index,config)+=stand->cell->ml.landfrac[data->irrigation].biomass_grass;
  } /* of if(isphen) */

  if(data->irrigation && stand->pftlist.n>0) /*second element to avoid irrigation on just harvested fields */
    calc_nir(stand,data,gp_stand,wet,eeq);
  transp=0;
  forrootsoillayer(l)
  {
    transp+=aet_stand[l]*stand->frac;
    getoutput(output,TRANSP_B,config)+=(aet_stand[l]-green_transp[l])*stand->frac;
  }
  if(config->crop_index==ALLSTAND)
  {
    getoutput(output,D_EVAP,config)+=evap*stand->frac;
    getoutput(output,D_TRANS,config)+=transp;
    getoutput(output,D_W0,config)+=stand->soil.w[1]*stand->frac;
    getoutput(output,D_W1,config)+=stand->soil.w[2]*stand->frac;
    getoutput(output,D_WEVAP,config)+=stand->soil.w[0]*stand->frac;
    getoutput(output,D_INTERC,config)+=intercep_stand*stand->frac;
  }

  getoutput(output,TRANSP,config)+=transp;
  stand->cell->balance.atransp+=transp;
  getoutput(output,EVAP,config)+=evap*stand->frac;
  stand->cell->balance.aevap+=evap*stand->frac;
  stand->cell->balance.ainterc+=intercep_stand*stand->frac;
  getoutput(output,EVAP_B,config)+=evap_blue*stand->frac;   /* blue soil evap */

  getoutput(output,INTERC,config)+=intercep_stand*stand->frac;
  getoutput(output,INTERC_B,config)+=intercep_stand_blue*stand->frac;   /* blue interception and evap */
#if defined(IMAGE) && defined(COUPLED)
  if(stand->cell->ml.image_data!=NULL)
    stand->cell->ml.image_data->mevapotr[month] += transp + (evap + intercep_stand)*stand->frac;
#endif

  getoutput(output,RETURN_FLOW_B,config)+=return_flow_b*stand->frac;

  output_gbw_biomass_grass(output,stand,frac_g_evap,evap,evap_blue,return_flow_b,aet_stand,green_transp,
                           intercep_stand,intercep_stand_blue,ncft,config);
  free(wet);
  return runoff;
} /* of 'daily_biomass_grass' */

/**************************************************************************************/
/**                                                                                \n**/
/**     d  a  i  l  y  _  b  i  o  m  a  s  s  _  t  r  e  e  .  c                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function of daily update of biomass tree stand                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "biomass_tree.h"

Real daily_biomass_tree(Stand *stand,                /**< stand pointer */
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
  int p,l,nnat,nirrig;
  Pft *pft;
  Real *gp_pft;         /**< pot. canopy conductance for PFTs & CFTs (mm/s) */
  Real gp_stand;               /**< potential stomata conductance  (mm/s) */
  Real gp_stand_leafon;        /**< pot. canopy conduct.at full leaf cover  (mm/s) */
  Real fpc_total_stand;
  Output *output;
  Real aet_stand[LASTLAYER];
  Real green_transp[LASTLAYER];
  Real evap; /* evaporation (mm) */
  Real evap_blue; /* evaporation of irrigation water (mm) */
  Real rd,gpp,frac_g_evap,runoff,wet_all,intercept,sprink_interc;
  Real *wet; /* wet from pftlist */
  Real return_flow_b; /* irrigation return flows from surface runoff, lateral runoff and percolation (mm)*/
  Real cover_stand;
  Real rainmelt,irrig_apply;
  Real intercep_stand; /* total stand interception (rain + irrigation) (mm)*/
  Real intercep_stand_blue; /* irrigation interception (mm)*/
  Real npp; /* net primary productivity (gC/m2) */
  Real wdf; /* water deficit fraction */
  Real transp;
  Real gc_pft;
  Real fertil;
  Biomass_tree *data;
  Soil *soil;
  irrig_apply=0.0;

  soil = &stand->soil;
  data=stand->data;
  output=&stand->cell->output;
  stand->growing_days++;
  evap=evap_blue=cover_stand=intercep_stand=intercep_stand_blue=runoff=return_flow_b=wet_all=intercept=sprink_interc=rainmelt=irrig_apply=0.0;
  if(getnpft(&stand->pftlist)>0)
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
    irrig_amount(stand,&data->irrigation,npft,ncft,month,config);
  nnat=getnnat(npft,config);
  nirrig=getnirrig(ncft,config);
  for(l=0;l<LASTLAYER;l++)
    aet_stand[l]=green_transp[l]=0;

  /* Apply fertilizers */
  if (config->with_nitrogen)
      fertilize_tree(stand,
                    (stand->cell->ml.fertilizer_nr==NULL) ? 0.0 : stand->cell->ml.fertilizer_nr[data->irrigation.irrigation].biomass_tree,
                    (stand->cell->ml.manure_nr==NULL) ? 0.0 : stand->cell->ml.manure_nr[data->irrigation.irrigation].biomass_tree,
                    day,config);

  /* green water inflow */
  rainmelt=climate->prec+melt;
  if(rainmelt<0)
    rainmelt=0.0;

  if(data->irrigation.irrigation && data->irrigation.irrig_amount>epsilon)
  {
    irrig_apply=max(data->irrigation.irrig_amount-rainmelt,0);  /*irrigate only missing deficit after rain, remainder goes to stor */
    data->irrigation.irrig_stor+=data->irrigation.irrig_amount-irrig_apply;
    data->irrigation.irrig_amount=0.0;
    if(irrig_apply<1 && data->irrigation.irrig_system!=DRIP)  /* min. irrigation requirement of 1mm */
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
        getoutputindex(output,CFT_AIRRIG,rbtree(ncft)+data->irrigation.irrigation*nirrig,config)+=irrig_apply*stand->cell->ml.landfrac[1].biomass_tree;
      else
        getoutputindex(output,CFT_AIRRIG,rbtree(ncft)+data->irrigation.irrigation*nirrig,config)+=irrig_apply;
    }
  }

  /* INTERCEPTION */
  foreachpft(pft,p,&stand->pftlist)
  {
    /* calculate old or new phenology */
    if (config->gsi_phenology)
      phenology_gsi(pft, climate->temp, climate->swdown, day,climate->isdailytemp,config);
    else
      leaf_phenology(pft,climate->temp,day,climate->isdailytemp,config);
    sprink_interc=(data->irrigation.irrig_system==SPRINK) ? 1 : 0;

    intercept=interception(&wet[p],pft,eeq,climate->prec+irrig_apply*sprink_interc); /* in case of sprinkler, irrig_amount goes through interception, in case of mixed, 0.5 of irrig_amount */
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
    getoutputindex(output,CFT_IRRIG_EVENTS,rbtree(ncft)+data->irrigation.irrigation*nirrig,config)++;
  }

  runoff+=infil_perc(stand,rainmelt+irrig_apply,&return_flow_b,npft,ncft,config);

  foreachpft(pft,p,&stand->pftlist)
  {
    cover_stand+=pft->fpc*pft->phen;

    /* calculate albedo and FAPAR of PFT */
    albedo_pft(pft, soil->snowheight, soil->snowfraction);

/*
 *  Calculate net assimilation, i.e. gross primary production minus leaf
 *  respiration, including conversion from FPC to grid cell basis.
 *
 */
    gpp=water_stressed(pft,aet_stand,gp_stand,gp_stand_leafon,
                       gp_pft[getpftpar(pft,id)],&gc_pft,&rd,
                       &wet[p],eeq,co2,climate->temp,par,daylength,&wdf,
                       npft,ncft,config);
   if(stand->cell->ml.landfrac[data->irrigation.irrigation].biomass_tree>0.0 &&
      gp_pft[getpftpar(pft,id)]>0.0)
   {
     getoutputindex(output,PFT_GCGP_COUNT,nnat+rbtree(ncft)+data->irrigation.irrigation*nirrig,config)++;
     getoutputindex(output,PFT_GCGP,nnat+rbtree(ncft)+data->irrigation.irrigation*nirrig,config)+=gc_pft/gp_pft[getpftpar(pft,id)];
   }
   npp=npp(pft,gtemp_air,gtemp_soil,gpp-rd-pft->npp_bnf,config->with_nitrogen);
   pft->npp_bnf=0.0;
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
   getoutputindex(output,CFT_FPAR,rbtree(ncft)+data->irrigation.irrigation*nirrig,config)+=(fpar(pft)*stand->cell->ml.landfrac[data->irrigation.irrigation].biomass_tree*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac)));

   if(config->pft_output_scaled)
     getoutputindex(output,PFT_NPP,nnat+rbtree(ncft)+data->irrigation.irrigation*nirrig,config)+=npp*stand->cell->ml.landfrac[data->irrigation.irrigation].biomass_tree;
   else
     getoutputindex(output,PFT_NPP,nnat+rbtree(ncft)+data->irrigation.irrigation*nirrig,config)+=npp;
   getoutputindex(output,PFT_LAI,nnat+rbtree(ncft)+data->irrigation.irrigation*nirrig,config)+=actual_lai(pft);

  } /* of foreachpft */

  /* soil outflow: evap and transpiration */
  free(gp_pft);
  waterbalance(stand,aet_stand,green_transp,&evap,&evap_blue,wet_all,eeq,cover_stand,
               &frac_g_evap,config->rw_manage);

  if(data->irrigation.irrigation && stand->pftlist.n>0) /*second element to avoid irrigation on just harvested fields */
    calc_nir(stand,&data->irrigation,gp_stand,wet,eeq,config->others_to_crop);
  transp=0;
  forrootsoillayer(l)
  {
    transp+=aet_stand[l]*stand->frac;
    getoutput(output,TRANSP_B,config)+=(aet_stand[l]-green_transp[l])*stand->frac;
  }
  getoutput(output,TRANSP,config)+=transp;
  stand->cell->balance.atransp+=transp;
  getoutput(output,INTERC,config)+=intercep_stand*stand->frac; /* Note: including blue fraction*/
  getoutput(output,INTERC_B,config)+=intercep_stand_blue*stand->frac;   /* blue interception and evap */
  stand->cell->balance.ainterc+=intercep_stand*stand->frac;

  getoutput(output,EVAP,config)+=evap*stand->frac;
  stand->cell->balance.aevap+=evap*stand->frac;
  getoutput(output,EVAP_B,config)+=evap_blue*stand->frac;   /* blue soil evap */
#if defined(IMAGE) && defined(COUPLED)
  if(stand->cell->ml.image_data!=NULL)
    stand->cell->ml.image_data->mevapotr[month] += transp + (evap + intercep_stand)*stand->frac;
#endif

  getoutput(output,RETURN_FLOW_B,config)+=return_flow_b*stand->frac; /* now only changed in waterbalance_new.c*/

  /* output for green and blue water for evaporation, transpiration and interception */
  output_gbw_biomass_tree(output,stand,frac_g_evap,evap,evap_blue,return_flow_b,aet_stand,green_transp,
                          intercep_stand,intercep_stand_blue,ncft,config);
  free(wet);
  return runoff;
} /* of 'daily_biomass_tree' */

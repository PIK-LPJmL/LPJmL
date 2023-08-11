/**************************************************************************************/
/**                                                                                \n**/
/**                d  a  i  l  y  _  n  a  t  u  r  a  l  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function of daily update of natural stand                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "natural.h"

Real daily_natural(Stand *stand,                /**< [inout] stand pointer */
                   Real co2,                    /**< [in] atmospheric CO2 (ppmv) */
                   const Dailyclimate *climate, /**< [in] Daily climate values */
                   int day,                     /**< [in] day (1..365) */
                   int month,                   /**< [in] month (0..11) */
                   Real daylength,              /**< [in] length of day (h) */
                   Real gtemp_air,              /**< [in] value of air temperature response function */
                   Real gtemp_soil,             /**< [in] value of soil temperature response function */
                   Real eeq,                    /**< [in] equilibrium evapotranspiration (mm) */
                   Real par,                    /**< [in] photosynthetic active radiation flux  (J/m2/day) */
                   Real melt,                   /**< [in] melting water (mm/day) */
                   int npft,                    /**< [in] number of natural PFTs */
                   int ncft,                    /**< [in] number of crop PFTs   */
                   int year,                    /**< [in] simulation year (AD) */
                   Bool UNUSED(intercrop),      /**< [in] enabled intercropping */
                   Real agrfrac,
                   const Config *config         /**< [in] LPJ config */
                  )                             /** \return runoff (mm/day) */
{
  int p,l;
#ifdef PERMUTE
  int *pvec;
#endif
  Pft *pft;
  Real *gp_pft;         /**< pot. canopy conductance for PFTs & CFTs (mm/s) */
  Real gp_stand;               /**< potential stomata conductance  (mm/s) */
  Real gp_stand_leafon;        /**< pot. canopy conduct.at full leaf cover  (mm/s) */
  Real fpc_total_stand;
  Output *output;
  Real aet_stand[LASTLAYER];
  Real green_transp[LASTLAYER];
  Real evap,evap_blue; /* evaporation (mm) */
  Real *wet; /* wet from pftlist */
  Real rd,gpp,frac_g_evap,runoff,wet_all;
  Real return_flow_b; /* irrigation return flows from surface runoff, lateral runoff and percolation (mm)*/
  Real cover_stand,intercep_stand;
  Real npp; /* net primary productivity (gC/m2) */
  Real wdf; /* water deficit fraction */
  Real gc_pft;
  Real transp;

#ifdef DAILY_ESTABLISHMENT
  Stocks flux_estab = {0,0};
#endif
  Soil *soil;
  soil = &stand->soil;
  output=&stand->cell->output;

  evap=evap_blue=cover_stand=intercep_stand=wet_all=0;

  runoff=return_flow_b=0.0;
  stand->growing_days++;
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

  for(l=0;l<LASTLAYER;l++)
    aet_stand[l]=green_transp[l]=0;
#ifdef PERMUTE
  for(p=0;p<getnpft(&stand->pftlist);p++)
#else
  foreachpft(pft,p,&stand->pftlist)
#endif
  {
#ifdef PERMUTE
    pft=getpft(&stand->pftlist,pvec[p]);
#endif
    /* calculate old or new phenology*/
    if (config->gsi_phenology)
      phenology_gsi(pft, climate->temp, climate->swdown, day,climate->isdailytemp,config);
    else
      leaf_phenology(pft,climate->temp,day,climate->isdailytemp,config);
    cover_stand+=pft->fpc*pft->phen;

    /* calculate albedo and FAPAR of PFT  already called in update_daily via albedo_stand*/
    albedo_pft(pft, soil->snowheight, soil->snowfraction);
    intercep_stand+=interception(&wet[p],pft,eeq,climate->prec);
    wet_all+=wet[p]*pft->fpc;
  }

  /* soil inflow: infiltration and percolation */
  runoff+=infil_perc_rain(stand,climate->prec+melt-intercep_stand,&return_flow_b,npft,ncft,config);
#ifdef PERMUTE
  for(p=0;p<getnpft(&stand->pftlist);p++)
#else
  foreachpft(pft,p,&stand->pftlist)
#endif
  {
/*
 *  Calculate net assimilation, i.e. gross primary production minus leaf
 *  respiration, including conversion from FPC to grid cell basis.
 *
 */
#ifdef PERMUTE
    pft=getpft(&stand->pftlist,pvec[p]);
#endif
    gpp=water_stressed(pft,aet_stand,gp_stand,gp_stand_leafon,
                       gp_pft[getpftpar(pft,id)],&gc_pft,&rd,
                       &wet[p],eeq,co2,climate->temp,par,daylength,&wdf,pft->par->id,npft,ncft,config);
    getoutput(output,AUTOTROPHIC_RESPIRATION,config)+=rd*stand->frac;
    if(gp_pft[getpftpar(pft,id)]>0.0)
    {
      getoutputindex(output,PFT_GCGP_COUNT,pft->par->id,config)++;
      getoutputindex(output,PFT_GCGP,pft->par->id,config)+=gc_pft/gp_pft[getpftpar(pft,id)];
    }

    npp=npp(pft,gtemp_air,gtemp_soil,gpp-rd-pft->npp_bnf,config->with_nitrogen);
    pft->npp_bnf=0.0;
    output->dcflux-=npp*stand->frac;
#if defined IMAGE && defined COUPLED
    if(stand->type->landusetype==NATURAL)
    {
      stand->cell->npp_nat+=npp*stand->frac;
    }
#endif
    stand->cell->balance.anpp+=npp*stand->frac;
    stand->cell->balance.agpp+=gpp*stand->frac;
    getoutput(output,NPP,config)+=npp*stand->frac;
    getoutput(output,GPP,config)+=gpp*stand->frac;
    getoutput(output,FAPAR,config) += pft->fapar * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    if (stand->type->landusetype == SETASIDE_RF || stand->type->landusetype == SETASIDE_IR)
      getoutput(output,NPP_AGR,config) += npp*stand->frac / agrfrac;

    getoutput(output,PHEN_TMIN,config)  += pft->fpc * pft->phen_gsi.tmin * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,PHEN_TMAX,config)  += pft->fpc * pft->phen_gsi.tmax * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,PHEN_LIGHT,config) += pft->fpc * pft->phen_gsi.light * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,PHEN_WATER,config) += pft->fpc * pft->phen_gsi.wscal * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(output,WSCAL,config)      += pft->fpc * pft->wscal * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));


    if(pft->stand->type->landusetype==NATURAL)
    {
      if(config->pft_output_scaled)
        getoutputindex(output,PFT_NPP,pft->par->id,config)+=npp*stand->frac;
      else
        getoutputindex(output,PFT_NPP,pft->par->id,config)+=npp;
      getoutputindex(output,PFT_LAI,pft->par->id,config)+=actual_lai(pft);
    }
  } /* of foreachpft */
  free(gp_pft);
  /* soil outflow: evap and transpiration */
  waterbalance(stand,aet_stand,green_transp,&evap,&evap_blue,wet_all,eeq,cover_stand,
               &frac_g_evap,FALSE);

  transp=0;
  forrootsoillayer(l)
  {
    transp+=aet_stand[l]*stand->frac;
    getoutput(output,TRANSP_B,config)+=(aet_stand[l]-green_transp[l])*stand->frac;
  }
  getoutput(output,TRANSP,config)+=transp;
  stand->cell->balance.atransp+=transp;
  getoutput(output,INTERC,config)+=intercep_stand*stand->frac;
  getoutput(output,EVAP,config)+=evap*stand->frac;
  stand->cell->balance.aevap+=evap*stand->frac;
  stand->cell->balance.ainterc+=intercep_stand*stand->frac;
  getoutput(output,EVAP_B,config)+=evap_blue*stand->frac;
  getoutput(output,RETURN_FLOW_B,config)+=return_flow_b*stand->frac;
#if defined(IMAGE) && defined(COUPLED)
  if(stand->cell->ml.image_data!=NULL)
    stand->cell->ml.image_data->mevapotr[month] += transp + (evap + intercep_stand)*stand->frac;
#endif
  if(stand->type->landusetype==NATURAL)
    foreachpft(pft, p, &stand->pftlist)
    {
      getoutputindex(output,NV_LAI,getpftpar(pft,id),config)+=actual_lai(pft);
    }

#ifdef DAILY_ESTABLISHMENT
  if (year==911 && day==365) /* TODO: replace the hardcoded value 911 with a more indicative flag like first_year_of_spinup */
    flux_estab=establishmentpft(stand,config->pftpar,npft,config->ntypes,stand->cell->balance.aprec,year);
  else if (year>911)
    flux_estab=establishmentpft(stand,config->pftpar,npft,config->ntypes,stand->cell->balance.aprec,year);
  getoutput(output,FLUX_ESTABC,config)+=flux_estab.carbon*stand->frac;
  getoutput(output,FLUX_ESTABN,config)+=flux_estab.nitrogen*stand->frac;
  stand->cell->balance.flux_estab.carbon+=flux_estab.carbon*stand->frac;
  stand->cell->balance.flux_estab.nitrogen+=flux_estab.nitrogen*stand->frac;
  output->dcflux-=flux_estab.carbon*stand->frac;
#endif

  free(wet);
#ifdef PERMUTE
  free(pvec);
#endif
  return runoff;
} /* of 'daily_natural' */

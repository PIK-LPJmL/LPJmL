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
                   Real daylength,              /**< [in] length of day (h) */
                   const Real gp_pft[],         /**< [out] pot. canopy conductance for PFTs & CFTs (mm/s) */
                   Real gtemp_air,              /**< [in] value of air temperature response function */
                   Real gtemp_soil,             /**< [in] value of soil temperature response function */
                   Real gp_stand,               /**< [in] potential stomata conductance  (mm/s) */
                   Real gp_stand_leafon,        /**< [in] pot. canopy conduct.at full leaf cover  (mm/s) */
                   Real eeq,                    /**< [in] equilibrium evapotranspiration (mm) */
                   Real par,                    /**< [in] photosynthetic active radiation flux  (J/m2/day) */
                   Real melt,                   /**< [in] melting water (mm/day) */
                   int npft,                    /**< [in] number of natural PFTs */
                   int ncft,                    /**< [in] number of crop PFTs   */
                   int year,                    /**< [in] simulation year (AD) */
                   Bool UNUSED(intercrop),      /**< [in] enabled intercropping */
                   const Config *config         /**< [in] LPJ config */
                  )                             /** \return runoff (mm/day) */
{
  int p,l;
#ifdef PERMUTE
  int *pvec;
#endif
  Pft *pft;
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
    if (config->new_phenology)
      phenology_gsi(pft, climate->temp, climate->swdown, day,climate->isdailytemp);
    else
      leaf_phenology(pft,climate->temp,day,climate->isdailytemp);
    cover_stand+=pft->fpc*pft->phen;

    /* calculate albedo and FAPAR of PFT  already called in update_daily via albedo_stand*/
    albedo_pft(pft, soil->snowheight, soil->snowfraction);
    intercep_stand+=interception(&wet[p],pft,eeq,climate->prec);
    wet_all+=wet[p]*pft->fpc;
  }

  /* soil inflow: infiltration and percolation */
  runoff+=infil_perc_rain(stand,climate->prec+melt-intercep_stand,&return_flow_b,config);
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
                       &wet[p],eeq,co2,climate->temp,par,daylength,&wdf,npft,ncft,config);
    if(gp_pft[getpftpar(pft,id)]>0.0)
    {
      output->gcgp_count[pft->par->id]++;
      output->pft_gcgp[pft->par->id]+=gc_pft/gp_pft[getpftpar(pft,id)];
    }

    npp=npp(pft,gtemp_air,gtemp_soil,gpp-rd,config->with_nitrogen);
    if(config->withdailyoutput && isdailyoutput_stand(output,stand))
    {
      if(output->daily.cft==ALLSTAND)
      {
        output->daily.npp+=npp*stand->frac;
        output->daily.gpp+=gpp*stand->frac;
      }
      else
      {
        output->daily.npp+=npp;
        output->daily.gpp+=gpp;
      }
    }
    output->dcflux-=npp*stand->frac;
#if defined IMAGE && defined COUPLED
    if(stand->type->landusetype==NATURAL)
    {
      output->npp_nat+=npp*stand->frac;
    }
#endif
    stand->cell->balance.nep+=npp*stand->frac;
    output->npp+=npp*stand->frac;
    output->gpp+=gpp*stand->frac;
    output->fapar += pft->fapar * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));

    output->mphen_tmin += pft->fpc * pft->phen_gsi.tmin * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    output->mphen_tmax += pft->fpc * pft->phen_gsi.tmax * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    output->mphen_light += pft->fpc * pft->phen_gsi.light * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    output->mphen_water += pft->fpc * pft->phen_gsi.wscal * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    output->mwscal += pft->fpc * pft->wscal * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));


    if(pft->stand->type->landusetype==NATURAL)
    {
      if(config->pft_output_scaled)
        output->pft_npp[pft->par->id]+=npp*stand->frac;
      else
        output->pft_npp[pft->par->id]+=npp;
      output->mpft_lai[pft->par->id]+=actual_lai(pft);
    }
  } /* of foreachpft */

  /* soil outflow: evap and transpiration */
  waterbalance(stand,aet_stand,green_transp,&evap,&evap_blue,wet_all,eeq,cover_stand,
               &frac_g_evap,FALSE);

  if(config->withdailyoutput && isdailyoutput_stand(output,stand))
  {
    if(output->daily.cft==ALLSTAND)
    {
      output->daily.evap+=evap*stand->frac;
      forrootsoillayer(l)
        output->daily.trans+=aet_stand[l]*stand->frac;
      output->daily.interc+=intercep_stand*stand->frac;
      output->daily.w0+=stand->soil.w[1]*stand->frac;
      output->daily.w1+=stand->soil.w[2]*stand->frac;
      output->daily.wevap+=stand->soil.w[0]*stand->frac;
      output->daily.par=par;
      output->daily.daylength=daylength;
      output->daily.pet=eeq*PRIESTLEY_TAYLOR;
    }
    else
    {
      output->daily.evap=evap;
      forrootsoillayer(l)
        output->daily.trans+=aet_stand[l];
      output->daily.irrig=0;
      output->daily.w0=stand->soil.w[1];
      output->daily.w1=stand->soil.w[2];
      output->daily.wevap=stand->soil.w[0];
      output->daily.par=par;
      output->daily.daylength=daylength;
      output->daily.pet=eeq*PRIESTLEY_TAYLOR;
      output->daily.interc=intercep_stand*stand->frac;
      forrootsoillayer(l)
      {
        output->daily.nh4+=stand->soil.NH4[l];
        output->daily.no3+=stand->soil.NO3[l];
        output->daily.nsoil_fast+=stand->soil.pool[l].fast.nitrogen;
        output->daily.nsoil_slow+=stand->soil.pool[l].slow.nitrogen;
      }
    }
  }
  transp=0;
  forrootsoillayer(l)
  {
    transp+=aet_stand[l]*stand->frac;
    output->mtransp_b+=(aet_stand[l]-green_transp[l])*stand->frac;
  }
  output->transp+=transp;
  stand->cell->balance.atransp+=transp;
  output->interc+=intercep_stand*stand->frac;
  output->evap+=evap*stand->frac;
  stand->cell->balance.aevap+=evap*stand->frac;
  stand->cell->balance.ainterc+=intercep_stand*stand->frac;
  output->mevap_b+=evap_blue*stand->frac;
  output->mreturn_flow_b+=return_flow_b*stand->frac;
  if(stand->type->landusetype==NATURAL)
    foreachpft(pft, p, &stand->pftlist)
    {
      output->nv_lai[getpftpar(pft,id)]+=actual_lai(pft);
    }

#ifdef DAILY_ESTABLISHMENT
  if (year==911 && day==365) /* TODO: replace the hardcoded value 911 with a more indicative flag like first_year_of_spinup */
    flux_estab=establishmentpft(stand,config->pftpar,npft,config->ntypes,stand->cell->balance.aprec,year);
  else if (year>911)
    flux_estab=establishmentpft(stand,config->pftpar,npft,config->ntypes,stand->cell->balance.aprec,year);
  output->flux_estab.carbon+=flux_estab.carbon*stand->frac;
  output->flux_estab.nitrogen+=flux_estab.nitrogen*stand->frac;
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

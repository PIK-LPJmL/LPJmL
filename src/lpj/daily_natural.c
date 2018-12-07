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

Real daily_natural(Stand *stand, /**< stand pointer */
                   Real co2,     /**< atmospheric CO2 (ppmv) */
                   const Dailyclimate *climate, /**< Daily climate values */
                   int day,    /**< day (1..365) */
                   Real daylength, /**< length of day (h) */
                   const Real gp_pft[], /**< pot. canopy conductance for PFTs & CFTs*/
                   Real gtemp_air,  /**< value of air temperature response function */
                   Real gtemp_soil, /**< value of soil temperature response function */
                   Real gp_stand,   /* potential stomata conductance */
                   Real gp_stand_leafon, /**< pot. canopy conduct.at full leaf cover */
                   Real eeq,   /**< equilibrium evapotranspiration (mm) */
                   Real par,   /**< photosynthetic active radiation flux */
                   Real melt,  /**< melting water (mm) */
                   int npft,   /**< number of natural PFTs */
                   int UNUSED(ncft), /**< number of crop PFTs   */
                   int year,         /**< simulation year (AD) */
                   Bool withdailyoutput, /**< daily output enabled */
                   Bool UNUSED(intercrop), /**< enabled intercropping */
                   const Config *config /**< LPJ config */
                  ) /** \return runoff (mm) */
{
  int p,l;
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

#ifdef DAILY_ESTABLISHMENT
  Real acflux_estab = 0;
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
    for(p=0;p<getnpft(&stand->pftlist);p++)
      wet[p]=0;
  }
  else
    wet=NULL;
  for(l=0;l<LASTLAYER;l++)
    aet_stand[l]=green_transp[l]=0;

  foreachpft(pft,p,&stand->pftlist)
  {
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
  runoff+=infil_perc_rain(stand,climate->prec+melt-intercep_stand,&return_flow_b,config->rw_manage);

  foreachpft(pft,p,&stand->pftlist)
  {
/*
 *  Calculate net assimilation, i.e. gross primary production minus leaf
 *  respiration, including conversion from FPC to grid cell basis.
 *
 */
    gpp=water_stressed(pft,aet_stand,gp_stand,gp_stand_leafon,
                       gp_pft[getpftpar(pft,id)],&gc_pft,&rd,
                       &wet[p],eeq,co2,climate->temp,par,daylength,&wdf,config->permafrost);
    if(gp_pft[getpftpar(pft,id)]>0.0)
    {
      output->gcgp_count[pft->par->id]++;
      output->pft_gcgp[pft->par->id]+=gc_pft/gp_pft[getpftpar(pft,id)];
    }

    npp=npp(pft,gtemp_air,gtemp_soil,gpp-rd);
    if (withdailyoutput){
      if (output->daily.cft == ALLNATURAL)
        output->daily.npp+=npp;
    }
    output->dcflux-=npp*stand->frac;
    output->mnpp+=npp*stand->frac;
    output->mgpp+=gpp*stand->frac;
    output->mfapar += pft->fapar * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));

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
    }
  } /* of foreachpft */

  /* soil outflow: evap and transpiration */
  waterbalance(stand,aet_stand,green_transp,&evap,&evap_blue,wet_all,eeq,cover_stand,
               &frac_g_evap,FALSE);

  if(withdailyoutput)
  {
#ifndef COUPLING_WITH_FMS
    // when coupling with FMS/POEM, we always need evap output,
    // regardless whether it is requested in the LPJ output configuration.
    foreachpft(pft,p,&stand->pftlist)
    if(pft->par->id==output->daily.cft)
#endif
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
      }
    output->daily.interc=intercep_stand*stand->frac;
  }

  forrootsoillayer(l)
  {
    output->mtransp+=aet_stand[l]*stand->frac;
    output->mtransp_b+=(aet_stand[l]-green_transp[l])*stand->frac;
  }

  output->minterc+=intercep_stand*stand->frac;
  output->mevap+=evap*stand->frac;
  output->mevap_b+=evap_blue*stand->frac;
  output->mreturn_flow_b+=return_flow_b*stand->frac;

  #ifdef DAILY_ESTABLISHMENT
  if (year==911 && day==365) /* TODO: replace the hardcoded value 911 with a more indicative flag like first_year_of_spinup */
    acflux_estab=establishmentpft(stand,config->pftpar,npft,config->ntypes,stand->cell->balance.aprec,year);
  else if (year>911)
    acflux_estab=establishmentpft(stand,config->pftpar,npft,config->ntypes,stand->cell->balance.aprec,year);
  output->flux_estab+=acflux_estab*stand->frac;
  output->dcflux-=acflux_estab*stand->frac;
#endif

  free(wet);
  return runoff;
} /* of 'daily_natural' */

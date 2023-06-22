/**************************************************************************************/
/**                                                                                \n**/
/**                d  a  i  l  y  _  s  e  t  a  s  i  d  e  .  c                  \n**/
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

#ifdef DAILY_ESTABLISHMENT
#include "lpj.h"
#include "grass.h"
#include "natural.h"
#include "agriculture.h"

Real daily_setaside(Stand *stand, /**< stand pointer */
                   Real co2,     /**< atmospheric CO2 (ppmv) */
                   const Dailyclimate *climate, /**< Daily climate values */
                   int day,    /**< day (1..365) */
                   int month,       /**< [in] month (0..11) */
                   Real daylength, /**< length of day (h) */
                   Real gtemp_air,  /**< value of air temperature response function */
                   Real gtemp_soil, /**< value of soil temperature response function */
                   Real eeq,   /**< equilibrium evapotranspiration (mm) */
                   Real par,   /**< photosynthetic active radiation flux */
                   Real melt,  /**< melting water (mm) */
                   int npft,   /**< number of natural PFTs */
                   int ncft,   /**< number of crop PFTs   */
                   int year,           /**< simulation year (AD) */
                   Bool intercrop, /**< enable intercropping (TRUE/FALSE) */
                   Real agrfrac,
                   const Config *config /**< LPJ config */
                  ) /** \return runoff (mm) */
{
  int p,l;
  Pft *pft;
  Real *gp_pft;         /**< pot. canopy conductance for PFTs & CFTs (mm/s) */
  Real gp_stand;               /**< potential stomata conductance  (mm/s) */
  Real gp_stand_leafon;        /**< pot. canopy conduct.at full leaf cover  (mm/s) */
  Real fpc_total_stand;
  Output *output;
  Real aet_stand[LASTLAYER];
  Real green_transp[LASTLAYER];
  Real evap,evap_blue; /* evaporation (mm) */
  Real rd,gpp,frac_g_evap,runoff,wet_all;
  Real *wet; /* wet from pftlist */
  Real return_flow_b; /* irrigation return flows from surface runoff, lateral runoff and percolation (mm)*/
  Real cover_stand,intercep_stand,rainmelt;
  Real npp; /* net primary productivity (gC/m2) */
  Real wdf; /* water deficit fraction */
  Real transp;
  Real gc_pft;
  Stocks flux_estab = {0,0};
  Stocks stocks;
  int n_est;
  Bool *present;
  Soil *soil;

  soil = &stand->soil;
  output=&stand->cell->output;

  evap=evap_blue=cover_stand=intercep_stand=wet_all=rainmelt=0.0;
  present=newvec(Bool,npft);
  check(present);
  for(p=0;p<npft;p++)
    present[p]=FALSE;
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
  gp_pft=newvec(Real,npft+ncft);
  check(gp_pft);
  gp_stand=gp_sum(&stand->pftlist,co2,climate->temp,par,daylength,
                  &gp_stand_leafon,gp_pft,&fpc_total_stand,config);

  for(l=0;l<LASTLAYER;l++)
    aet_stand[l]=green_transp[l]=0;

  foreachpft(pft,p,&stand->pftlist)
  {
    present[pft->par->id]=TRUE;
    /* calculate old or new phenology */
    if (config->gsi_phenology)
      phenology_gsi(pft, climate->temp,climate->swdown,day,climate->isdailytemp);
    else
      leaf_phenology(pft,climate->temp,day,climate->isdailytemp);
    cover_stand+=pft->fpc*pft->phen;

    /* calculate albedo and FAPAR of PFT*/
    albedo_pft(pft, soil->snowheight, soil->snowfraction);

    intercep_stand+=interception(&wet[p],pft,eeq,climate->prec);
    wet_all+=wet[p]*pft->fpc;
  }

  /* soil inflow: infiltration and percolation */
  rainmelt=climate->prec+melt;
  if(rainmelt<0)
    rainmelt=0.0;
  runoff+=infil_perc_rain(stand,rainmelt-intercep_stand,&return_flow_b,config);

  foreachpft(pft,p,&stand->pftlist)
  {
    /*
     *  Calculate net assimilation, i.e. gross primary production minus leaf
     *  respiration, including conversion from FPC to grid cell basis.
     *
     */
    gpp=water_stressed(pft,aet_stand,gp_stand,gp_stand_leafon,
                       gp_pft[getpftpar(pft,id)],&gc_pft,&rd,
                       &wet[p],eeq,co2,climate->temp,par,daylength,&wdf,npft,ncft,config);
    if(gp_pft[getpftpar(pft,id)]>0.0)
    {
      output->gcgp_count[pft->par->id]++;
      output->pft_gcgp[pft->par->id]+=gc_pft/gp_pft[getpftpar(pft,id)];
    }

    npp=npp(pft,gtemp_air,gtemp_soil,gpp-rd-pft->npp_bnf,config->with_nitrogen);
    pft->npp_bnf=0.0;
    if(output->daily.cft==ALLSTAND)
    {
      output->daily.npp+=npp*stand->frac;
      output->daily.gpp+=gpp*stand->frac;
    }
    output->npp+=npp*stand->frac;
    output->npp_agr += npp*stand->frac / agrfrac;
    stand->cell->balance.anpp+=npp*stand->frac;
    stand->cell->balance.agpp+=gpp*stand->frac;
    output->dcflux-=npp*stand->frac;
    output->gpp+=gpp*stand->frac;
    output->fapar += pft->fapar * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    output->mphen_tmin += pft->fpc * pft->phen_gsi.tmin * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    output->mphen_tmax += pft->fpc * pft->phen_gsi.tmax * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    output->mphen_light += pft->fpc * pft->phen_gsi.light * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    output->mphen_water += pft->fpc * pft->phen_gsi.wscal * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    output->mwscal += pft->fpc * pft->wscal * stand->frac * (1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));


  } /* of foreachpft */

  free(gp_pft);
  /* soil outflow: evap and transpiration */
  waterbalance(stand,aet_stand,green_transp,&evap,&evap_blue,wet_all,eeq,cover_stand,
               &frac_g_evap,config->rw_manage);
  if(output->daily.cft==ALLSTAND)
  {
    output->daily.evap+=evap*stand->frac;
    forrootsoillayer(l)
      output->daily.trans+=aet_stand[l]*stand->frac;
    output->daily.irrig=0;
    output->daily.w0+=stand->soil.w[1]*stand->frac;
    output->daily.w1+=stand->soil.w[2]*stand->frac;
    output->daily.wevap+=stand->soil.w[0]*stand->frac;
    output->daily.par=par;
    output->daily.pet=eeq*PRIESTLEY_TAYLOR;
  }

  transp=0;
  forrootsoillayer(l)
  {
    transp+=aet_stand[l]*stand->frac;
    output->mtransp_b+=(aet_stand[l]-green_transp[l])*stand->frac;
  }
  output->transp+=transp;
  output->atransp+=transp;
  output->interc+=intercep_stand*stand->frac;
  output->evap+=evap*stand->frac;
  output->aevap+=evap*stand->frac;
  output->ainterc+=intercep_stand*stand->frac;
  output->mevap_b+=evap_blue*stand->frac; /*TODOJJ why are monthly outputs in setaside written but cft-outputs not? */
  output->mreturn_flow_b+=return_flow_b*stand->frac;
#if defined(IMAGE) && defined(COUPLED)
  if(cell->ml.image_data!=NULL)
    stand->cell->ml.image_data->mevapotr[month] += transp + (evap + intercep_stand)*stand->frac;
#endif

  /* new block for daily establishment */
  n_est=0;
  if(intercrop)
  {
    flux_estab.carbon=flux_estab.nitrogen=0;
    for(p=0;p<npft;p++)
    {
      if(config->pftpar[p].type==GRASS && config->pftpar[p].cultivation_type==NONE /* still correct?? */ && (!present[p]) &&
         establish(stand->cell->gdd[p],config->pftpar+p,&stand->cell->climbuf))
        addpft(stand,config->pftpar+p,year,day);
        n_est++;
    }
    foreachpft(pft,p,&stand->pftlist)
      if(!pft->established)
      {
        stocks=establishment_grass(pft,0,0,n_est);
        flux_estab.carbon+=stocks.carbon;
        flux_estab.nitrogen+=stocks.nitrogen;
        pft->established=TRUE;
      }

    /* separate calculation of grass FPC after all grass PFTs have been updated */
    foreachpft(pft,p,&stand->pftlist)
      fpc_grass(pft);

    output->flux_estab.carbon+=flux_estab.carbon*stand->frac;
    output->flux_estab.nitrogen+=flux_estab.nitrogen*stand->frac;
    stand->cell->balance.flux_estab.carbon+=flux_estab.carbon*stand->frac;
    stand->cell->balance.flux_estab.nitrogen+=flux_estab.nitrogen*stand->frac;
    output->dcflux-=flux_estab.carbon*stand->frac;
  }
  free(present);
  /* end new block for daily establishment */

  free(wet);
  return runoff;
} /* of 'daily_setaside' */
#endif /* DAILY_ESTABLISHMENT */

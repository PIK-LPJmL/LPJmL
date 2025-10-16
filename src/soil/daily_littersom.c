/**************************************************************************************/
/**                                                                                \n**/
/**                d a i l y _ l  i  t  t  e  r  s  o  m  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**       calls sub-daily littersom and tests stability criterions                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define np -0.5              /* parameter given in Riera et al. 1999*/
#define wind_speed 3.28      // average global wind speed in m/s over lands https://web.stanford.edu/group/efmh/winds/global_winds.html

typedef struct
{
  Real bCH4[LASTLAYER],bO2[LASTLAYER];
} Data;

static Bool istoolarge(const Soil *soil,const Real O2[LASTLAYER],const Real CH4[LASTLAYER])
{
  Real test_O2;
  Real test_CH4;
  int l;
  forrootsoillayer(l)
  {
    test_O2=fabs(soil->O2[l]-O2[l]);
    test_CH4=fabs(soil->CH4[l]-CH4[l]);
    if(test_O2>O2[l]*0.2 || test_CH4>CH4[l]*0.2)
      return TRUE;
  }
  return FALSE;
} /* of 'istoolarge' */

static Real f_wfps(const Soil *soil,      /* Soil data */
                   int l                  /* soil layer */
                  )                       /* return soil temperature (deg C) */
{
  Real x;
  x=(soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->wpwps[l]+soil->w_fw[l]+soil->ice_fw[l])/soil->wsats[l];
  return pow((x-soil->par->b_nit)/soil->par->n_nit,soil->par->z_nit)*
      pow((x-soil->par->c_nit)/soil->par->m_nit,soil->par->d_nit);
} /* of 'f_wfps' */

Stocks daily_littersom(Stand *stand,                      /**< [inout] pointer to stand data */
                       const Real gtemp_soil[NSOILLAYER], /**< [in] respiration coefficents */
                       Real cellfrac_agr,                 /**< [in] stand fraction of agricultural cells (0..1) */
                       Real *CH4_source,                  /**< [out] CH4 emissions (gC/m2/day) */
                       Real airtemp,                      /**< [in] air temperature (deg C) */
                       Real pch4,                         /**< [in] atmospheric methane (ppm) */
                       Real *lrunoff,                     /**< [out] runoff (mm/day) */
                       Real *MT_lwater,                   /**< [out] water from oxidized methane (mm/day) */
                       Real *CH4_sink,                    /**< [out] negative CH4 emissions (gC/m2/day) */
                       Real *rice_em,
                       int npft,                          /**< [in] number of natural PFTs */
                       int ncft,                          /**< [in] number of crop PFTs */
                       const Config *config               /**< [in] LPJmL configuration */
                      )                                   /** \return decomposed carbon/nitrogen (g/m2) */
{
  Soil *soil;
  soil=&stand->soil;
  Soil savesoil;
  Data data;
  int timesteps=6;
  int faststep=6;
  int i,l,dt,p;
  Stocks hetres;
  Stocks hetres1;
  Bool fast_needed  = FALSE;
  hetres.nitrogen=hetres.carbon=0;
#ifdef USE_TIMING
  double tstart;
  timing_start(tstart);
#endif
  soil->count++;
  *rice_em=*MT_lwater=*lrunoff=0;
  if(config->with_methane)
  {
    Real temp;
    Real methaneflux_litter,runoff,MT_water,ch4_sink;
    Real O2_save[LASTLAYER];
    Real CH4_save[LASTLAYER];
    Real Q10_oxid[LASTLAYER],fac_wfps[LASTLAYER],fac_temp[LASTLAYER];
    Real moist,w_agtop;
    Real response_agtop_leaves,*response_agsub_wood,*response_agtop_wood;
    Real CH4_air, ScCH4, k_600, kCH4;
    Real O2_air, ScO2, kO2;
    response_agtop_wood=newvec(Real,ncft+npft);
    check(response_agtop_wood);
    response_agsub_wood=newvec(Real,ncft+npft);
    check(response_agsub_wood);
    moist=(soil->w[0]*soil->whcs[0]+(soil->wpwps[0]*(1-soil->ice_pwp[0]))+soil->w_fw[0])
      /(soil->wsats[0]-soil->ice_depth[0]-soil->ice_fw[0]-(soil->wpwps[0]*soil->ice_pwp[0]));

    w_agtop=soil->litter.agtop_wcap>epsilon ? soil->litter.agtop_moist/soil->litter.agtop_wcap : moist;

    response_agtop_leaves=temp_response(soil->litter.agtop_temp,soil->amean_temp[0])*(INTERCEPT+MOIST_3*(w_agtop*w_agtop*w_agtop)+MOIST_2*(w_agtop*w_agtop)+MOIST*w_agtop);
    for(p=0;p<(npft+ncft);p++)
    {
      response_agsub_wood[p]=pow(config->pftpar[p].k_litter10.q10_wood,(soil->temp[0]-10)/10.0)*(INTERCEPT+MOIST_3*(moist*moist*moist)+MOIST_2*(moist*moist)+MOIST*moist);
      response_agtop_wood[p]=pow(config->pftpar[p].k_litter10.q10_wood,(soil->litter.agtop_temp-10)/10.0)*(INTERCEPT+MOIST_3*(w_agtop*w_agtop*w_agtop)+MOIST_2*(w_agtop*w_agtop)+MOIST*w_agtop);
    }


    forrootsoillayer(l)
    {
      if (soil->temp[l]>40)
        temp = 40;
      else
        temp=soil->temp[l];
      Q10_oxid[l]=pow(Q10,(temp-soil->amean_temp[l])/10);
      fac_wfps[l] = f_wfps(soil,l);
      fac_temp[l] = f_temp(soil->temp[l]);
      data.bO2[l]=0.0647*exp(-0.0257*soil->temp[l]);
      data.bCH4[l]=0.0523*exp(-0.0236*soil->temp[l]);
    }

    CH4_air=p_s/R_gas/degCtoK(airtemp)*pch4*1e-6*WCH4; /*g/m3 methane concentration*/
    O2_air=p_s/R_gas/degCtoK(airtemp)*O2s*WO2;         /*g/m3 oxygen concentration*/

    /* Calculate Schmidt number and gas transfer velocity */
    /*--------------------------------------------------------*/
    if (airtemp >= 50)
      ScCH4 = 20;
    else
      ScCH4 = 0.00055667*airtemp*airtemp*airtemp*airtemp - 0.06747*airtemp*airtemp*airtemp + 3.2403*airtemp*airtemp - 84.575*airtemp + 1420.5;               //1898 - 110.1 * airtemp + 2.834 * airtemp*airtemp - 0.02791 * airtemp*airtemp*airtemp;
    k_600 = 2.07 + 0.215*pow(wind_speed,1.7);
    kCH4 = k_600*pow((ScCH4 / 600), np);   //piston velocity
    if (airtemp >= 40)
      ScO2 = 8;
    else
      ScO2 = 0.00086842*airtemp*airtemp*airtemp*airtemp - 0.10115*airtemp*airtemp*airtemp + 4.8055*airtemp*airtemp - 124.34*airtemp + 1745.1;                //1800.6-120.1*airtemp+3.7818*airtemp*airtemp-0.047608 * airtemp*airtemp*airtemp;
    kO2 = k_600*pow((ScO2/600),np);
    kCH4 = kCH4/100*24;
    kO2 = kO2/100*24;


    *CH4_sink=*CH4_source=*lrunoff=*MT_lwater=0;

    savesoil.decomp_litter_pft=newvec(Stocks,npft+ncft);
    check(savesoil.decomp_litter_pft);
    savesoil.litter.n=0;

    for(dt=0;dt<timesteps;dt++)
    {
      soil_status(&savesoil, soil, npft+ncft);
      forrootsoillayer(l)
      {
        O2_save[l]=soil->O2[l];
        CH4_save[l]=soil->CH4[l];
      }
      hetres1=littersom(stand,gtemp_soil,cellfrac_agr,&methaneflux_litter,airtemp,pch4,&runoff,&MT_water,&ch4_sink,npft,ncft,config,
          Q10_oxid,fac_wfps,fac_temp,data.bCH4,data.bO2,response_agtop_leaves,response_agsub_wood,response_agtop_wood,timesteps);
      fast_needed=istoolarge(soil,O2_save,CH4_save);
      if(!fast_needed)
        *rice_em+=plant_gas_transport(stand,CH4_air,O2_air,config,kCH4/timesteps,kO2/timesteps)*stand->frac;   //fluxes in routine written to output

      if(fast_needed )
      {
        soil_status(soil, &savesoil, npft+ncft);
        for (i=0;i<faststep;i++)
        {
          hetres1=littersom(stand,gtemp_soil,cellfrac_agr,&methaneflux_litter,airtemp,pch4,&runoff,&MT_water,&ch4_sink,npft,ncft,config,
              Q10_oxid,fac_wfps,fac_temp,data.bCH4,data.bO2,response_agtop_leaves,response_agsub_wood,response_agtop_wood,timesteps*faststep);
          *rice_em+=plant_gas_transport(stand,CH4_air,O2_air,config,kCH4/(timesteps*faststep),kO2/(timesteps*faststep))*stand->frac;   //fluxes in routine written to output
          hetres.carbon+=hetres1.carbon;
          hetres.nitrogen+=hetres1.nitrogen;
          *CH4_sink+=ch4_sink;
          *CH4_source+=methaneflux_litter;
          *lrunoff+=runoff;
          *MT_lwater+=MT_water;
        }
      }
      else
      {
        hetres.carbon+=hetres1.carbon;
        hetres.nitrogen+=hetres1.nitrogen;
        *CH4_sink+=ch4_sink;
        *CH4_source+=methaneflux_litter;
        *lrunoff+=runoff;
        *MT_lwater+=MT_water;
      }
      fast_needed = FALSE;
    }
    freelitter(&savesoil.litter);
    free(savesoil.decomp_litter_pft);
    free(response_agtop_wood);
    free(response_agsub_wood);
  }
  else
  {
    hetres=littersom_nomethane(stand,gtemp_soil,cellfrac_agr,npft,ncft,config);
  }
#ifdef USE_TIMING
  timing_stop(DAILY_LITTERSOM_FCN,tstart);
#endif
  return hetres;
} /* of 'daily_littersom' */

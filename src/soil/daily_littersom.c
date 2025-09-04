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

static Real f_wfps(const Soil *soil, /* Soil data */
                   int l             /* soil layer */
                  )                  /* return soil temperature (deg C) */
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
                       int npft,                          /**< [in] number of natural PFTs */
                       int ncft,                          /**< [in] number of crop PFTs */
                       const Config *config               /**< [in] LPJmL configuration */
                      )                                   /** \return decomposed carbon/nitrogen (g/m2) */
{
  Soil *soil;
  soil=&stand->soil;
  Soil savesoil;
  int timesteps=6;
  int i,l,dt,p;
  Stocks hetres;
  Stocks hetres1;
  hetres.nitrogen=hetres.carbon=0;
  Real temp;
  Real methaneflux_litter,runoff,MT_water,ch4_sink;
  Littersom_param data;
  Real moist,w_agtop;

#ifdef USE_TIMING
  double tstart;
  tstart=mrun();
#endif
  data.response_agtop_wood=newvec(Real,ncft+npft);
  check(data.response_agtop_wood);
  data.response_agsub_wood=newvec(Real,ncft+npft);
  check(data.response_agsub_wood);
  soil->count++;

  moist=(soil->w[0]*soil->whcs[0]+(soil->wpwps[0]*(1-soil->ice_pwp[0]))+soil->w_fw[0])
  /(soil->wsats[0]-soil->ice_depth[0]-soil->ice_fw[0]-(soil->wpwps[0]*soil->ice_pwp[0]));

  w_agtop=soil->litter.agtop_wcap>epsilon ? soil->litter.agtop_moist/soil->litter.agtop_wcap : moist;
  data.response_agtop_leaves=temp_response(soil->litter.agtop_temp,soil->amean_temp[0])*(INTERCEPT+MOIST_3*(w_agtop*w_agtop*w_agtop)+MOIST_2*(w_agtop*w_agtop)+MOIST*w_agtop);
  for(p=0;p<(npft+ncft);p++)
  {
    data.response_agsub_wood[p]=pow(config->pftpar[p].k_litter10.q10_wood,(soil->temp[0]-10)*0.1)*(INTERCEPT+MOIST_3*(moist*moist*moist)+MOIST_2*(moist*moist)+MOIST*moist);
    data.response_agtop_wood[p]=pow(config->pftpar[p].k_litter10.q10_wood,(soil->litter.agtop_temp-10)*0.1)*(INTERCEPT+MOIST_3*(w_agtop*w_agtop*w_agtop)+MOIST_2*(w_agtop*w_agtop)+MOIST*w_agtop);
  }

  forrootsoillayer(l)
  {
    temp=min(soil->temp[l],40);
    data.Q10_oxid[l]=pow(Q10,(temp-soil->amean_temp[l])*0.1);
    data.fac_wfps[l] = f_wfps(soil,l);
    data.fac_temp[l] = f_temp(soil->temp[l]);
    data.bO2[l]=0.0647*exp(-0.0257*soil->temp[l]);
    data.bCH4[l]=0.0523*exp(-0.0236*soil->temp[l]);
  }

  *CH4_sink=*CH4_source=*lrunoff=*MT_lwater=0;

  savesoil.decomp_litter_pft=newvec(Stocks,npft+ncft);
  check(savesoil.decomp_litter_pft);
  savesoil.litter.n=0;

/*###################### TESTE*/
//  timesteps=100;
//  for(dt=0;dt<timesteps;dt++)
//  {
//    hetres1=littersom(stand,gtemp_soil,cellfrac_agr,&methaneflux_litter,airtemp,pch4,&runoff,&MT_water,&ch4_sink,npft,ncft,config,
//         Q10_oxid,fac_wfps,fac_temp,data.bCH4,data.bO2,response_agtop_leaves,response_agsub_wood,response_agtop_wood,timesteps);
//    hetres.carbon+=hetres1.carbon;
//    hetres.nitrogen+=hetres1.nitrogen;
//    *CH4_sink+=ch4_sink;
//    *CH4_source+=methaneflux_litter;
//    *lrunoff+=runoff;
//    *MT_lwater+=MT_water;
//  }
/*######################*/
  for(dt=0;dt<timesteps;dt++)
  {
    soil_status(&savesoil, soil, npft+ncft);
    hetres1=littersom(stand,gtemp_soil,cellfrac_agr,&methaneflux_litter,airtemp,pch4,&runoff,&MT_water,&ch4_sink,npft,ncft,config,&data,timesteps);
    if(istoolarge(soil,savesoil.O2,savesoil.CH4))
    {
      soil_status(soil, &savesoil, npft+ncft);
      for (i=0;i<5;i++)
      {
        hetres1=littersom(stand,gtemp_soil,cellfrac_agr,&methaneflux_litter,airtemp,pch4,&runoff,&MT_water,&ch4_sink,npft,ncft,config,&data,timesteps*5);
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
  } /* of  for(dt=0;dt<timesteps;dt++) */
  freelitter(&savesoil.litter);
  free(savesoil.decomp_litter_pft);
  free(data.response_agtop_wood);
  free(data.response_agsub_wood);
#ifdef USE_TIMING
  timing.littersom+=mrun()-tstart;
#endif
  return hetres;
} /* of 'daily_littersom' */

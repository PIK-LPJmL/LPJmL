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

#define Q10 1.8

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
                       int npft,                          /**< [in] number of natural PFTs */
                       int ncft,                          /**< [in] number of crop PFTs */
                       const Config *config               /**< [in] LPJmL configuration */
                      )                                   /** \return decomposed carbon/nitrogen (g/m2) */
{
  Soil *soil;
  soil=&stand->soil;
  Soil savesoil;
  Data data;
  int timesteps=3;
  int i,l,dt;
  Stocks hetres;
  Stocks hetres1;
  Bool fast_needed  = FALSE;
  hetres.nitrogen=hetres.carbon=0;
  Real temp;
  Real methaneflux_litter,runoff,MT_water,ch4_sink;
  Real O2_save[LASTLAYER];
  Real CH4_save[LASTLAYER];
  Real Q10_oxid[LASTLAYER],fac_wfps[LASTLAYER],fac_temp[LASTLAYER];



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


  *CH4_sink=*CH4_source=*lrunoff=*MT_lwater=0;

  savesoil.decomp_litter_pft=newvec(Stocks,npft+ncft);
  check(savesoil.decomp_litter_pft);
  savesoil.litter.n=0;

/*###################### TESTE*/
//  timesteps=100;
//  for(dt=0;dt<timesteps;dt++)
//  {
//    hetres1=littersom(stand,gtemp_soil,cellfrac_agr,&methaneflux_litter,airtemp,pch4,&runoff,&MT_water,&ch4_sink,npft,ncft,config,
//         Q10_oxid,fac_wfps,fac_temp,data.bCH4,data.bO2,timesteps);
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
    forrootsoillayer(l)
    {
      O2_save[l]=soil->O2[l];
      CH4_save[l]=soil->CH4[l];
    }
    hetres1=littersom(stand,gtemp_soil,cellfrac_agr,&methaneflux_litter,airtemp,pch4,&runoff,&MT_water,&ch4_sink,npft,ncft,config,
        Q10_oxid,fac_wfps,fac_temp,data.bCH4,data.bO2,timesteps);
    fast_needed=istoolarge(soil,O2_save,CH4_save);

    if(fast_needed )
    {
      soil_status(soil, &savesoil, npft+ncft);
      for (i=0;i<10;i++)
      {
        hetres1=littersom(stand,gtemp_soil,cellfrac_agr,&methaneflux_litter,airtemp,pch4,&runoff,&MT_water,&ch4_sink,npft,ncft,config,
            Q10_oxid,fac_wfps,fac_temp,data.bCH4,data.bO2,timesteps*10);
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
  return hetres;
} /* of 'daily_littersom' */

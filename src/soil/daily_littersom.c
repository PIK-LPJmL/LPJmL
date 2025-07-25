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
  int timesteps=3;
  int l,dt;
  Stocks hetres;
  Stocks hetres1;
  int fast_needed = 0;
  Bool stop = FALSE;
  hetres.nitrogen=hetres.carbon=0;
  Real methaneflux_litter,runoff,MT_water,ch4_sink;
  Real O2_save[LASTLAYER];
  Real CH4_save[LASTLAYER];

  *CH4_sink=*CH4_source=*lrunoff=*MT_lwater=0;

  savesoil.decomp_litter_pft=newvec(Stocks,npft+ncft);
  check(savesoil.decomp_litter_pft);

  savesoil.litter.n=0;
  soil_status(&savesoil, soil, npft+ncft);

  for(dt=0;dt<timesteps;dt++)
  {
    forrootsoillayer(l)
        {
      O2_save[l]=soil->O2[l];
      CH4_save[l]=soil->CH4[l];
        }
    hetres1=littersom(stand,gtemp_soil,cellfrac_agr,&methaneflux_litter,airtemp,pch4,&runoff,&MT_water,&ch4_sink,npft,ncft,config,timesteps);
    if((stop=istoolarge(soil,O2_save,CH4_save)))
      break;
    hetres.carbon+=hetres1.carbon;
    hetres.nitrogen+=hetres1.nitrogen;
    *CH4_sink+=ch4_sink;
    *CH4_source+=methaneflux_litter;
    *lrunoff+=runoff;
    *MT_lwater+=MT_water;
  }
  if(stop)
  {
    soil_status(soil, &savesoil, npft+ncft);
    *CH4_sink=*CH4_source=*lrunoff=*MT_lwater=hetres.nitrogen=hetres.carbon=0;
    timesteps=30;
    for(dt=0;dt<timesteps;dt++)
    {
      hetres1=littersom(stand,gtemp_soil,cellfrac_agr,&methaneflux_litter,airtemp,pch4,&runoff,&MT_water,&ch4_sink,npft,ncft,config,timesteps);
      hetres.carbon+=hetres1.carbon;
      hetres.nitrogen+=hetres1.nitrogen;
      *CH4_sink+=ch4_sink;
      *CH4_source+=methaneflux_litter;
      *lrunoff+=runoff;
      *MT_lwater+=MT_water;
    }
  }
  freelitter(&savesoil.litter);
  free(savesoil.decomp_litter_pft);
  return hetres;
} /* of 'daily_littersom' */

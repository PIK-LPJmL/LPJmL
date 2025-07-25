/**************************************************************************************/
/**                                                                                \n**/
/**                d a i l y _ l  i  t  t  e  r  s  o  m  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**       calls sub-daily littersom and tests stability criterions                 \n**/
/**                                                                                \n**/
/**                                                                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/


#include "lpj.h"

Stocks daily_littersom(Stand *stand,                      /**< [inout] pointer to stand data */
                 const Real gtemp_soil[NSOILLAYER], /**< [in] respiration coefficents */
                 Real cellfrac_agr,                 /**< [in] stand fraction of agricultural cells (0..1) */
                 Real *CH4_source,          /**< [out] CH4 emissions (gC/m2/day) */
                 Real airtemp,                      /**< [in] air temperature (deg C) */
                 Real pch4,                         /**< [in] atmospheric methane (ppm) */
                 Real *lrunoff,                      /**< [out] runoff (mm/day) */
                 Real *MT_lwater,                    /**< [out] water from oxidized methane (mm/day) */
                 Real *CH4_sink,                    /**< [out] negative CH4 emissions (gC/m2/day) */
                 int npft,                          /**< [in] number of natural PFTs */
                 int ncft,                          /**< [in] number of crop PFTs */
                 const Config *config               /**< [in] LPJmL configuration */
                )                                   /** \return decomposed carbon/nitrogen (g/m2) */
{
  Soil *soil;
  soil=&stand->soil;
  Soil savesoil;
  Soil testsoil;
  int timesteps=3;
  int i,p,l,dt;
  Stocks hetres;
  Stocks hetres1;
  Real test_O2,test_CH4;
  int stop = 0;
  test_O2=test_CH4=0;
  hetres.nitrogen=hetres.carbon=0;
  Real methaneflux_litter,runoff,MT_water,ch4_sink;

  *CH4_sink=*CH4_source=*lrunoff=*MT_lwater=0;

  savesoil.decomp_litter_pft=newvec(Stocks,npft+ncft);
  testsoil.decomp_litter_pft=newvec(Stocks,npft+ncft);

  testsoil.litter.n=0;
  savesoil.litter.n=0;
  soil_status(&savesoil, soil, npft+ncft);

  for(dt=0;dt<timesteps && !stop;dt++)
  {
    test_O2=test_CH4=0;
    soil_status(&testsoil, soil, npft+ncft);
    hetres1=littersom(stand,gtemp_soil,cellfrac_agr,&methaneflux_litter,airtemp,pch4,&runoff,&MT_water,&ch4_sink,npft,ncft,config,timesteps);
    hetres.carbon+=hetres1.carbon;
    hetres.nitrogen+=hetres1.nitrogen;
    *CH4_sink+=ch4_sink;
    *CH4_source+=methaneflux_litter;
    *lrunoff+=runoff;
    *MT_lwater+=MT_water;
    forrootsoillayer(l)
    {
      test_O2=fabs(soil->O2[l]-testsoil.O2[l]);
      test_CH4=fabs(soil->CH4[l]-testsoil.CH4[l]);
      if(test_O2>testsoil.O2[l]*0.2 || test_CH4>testsoil.CH4[l]*0.2)
      {
        stop=1;
        break;
      }
    }
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
  freelitter(&testsoil.litter);
  free(savesoil.decomp_litter_pft);
  free(testsoil.decomp_litter_pft);
  return hetres;
}

/**************************************************************************************/
/**                                                                                \n**/
/**                          e b u l l i t i o n . c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Calculates gas transport through the soil layers                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define CH4_min  0.006 /* 0.012 threshold value at which ebullition occur at totally vegetated soils g*m-3 8/1000*/
#define k_e 1          /* rate constant h-1 */

#ifdef DEBUG

static void printch4(const Real CH4[LASTLAYER])
{
  int i;
  for (i = 0; i<LASTLAYER; i++)
    printf(" %g", CH4[i]);
  printf("\n");
}

#endif

Real ebullition(Soil *soil,   /**< pointer to soil data */
                Real fpc_all  /**< plant cover  (0..1) */
              )
{
  Real C_thres, Q_ebull, ratio, Q_ebull_day, soil_moist[NSOILLAYER], V[NSOILLAYER], epsilon_CH4; //, epsilon_CH4_u;
  int l, i;
  Q_ebull = Q_ebull_day = 0.0;
#ifdef DEBUG
  printf("EBULL before:");
  printch4(soil->CH4);
#endif
  for (l = 0; l<NSOILLAYER; l++)
  {
    soil_moist[l] = getsoilmoist(soil,l);
    V[l] = getV(soil,l);  /*soil air content (m3 air/m3 soil)*/
  }
  C_thres = CH4_min*(2 - fpc_all);

  for (l = LASTLAYER - 1; l >= 0; l--)
  {
    epsilon_CH4 = max(0.001, V[l] + soil_moist[l] * soil->wsat[l]*BCH4);
    ratio = min((layerbound[l] - soil->wtable) / soildepth[l], 1);
    for (i = 1; i <= 24; i++)
    {
      if ((soil->CH4[l]/soildepth[l]*1000/epsilon_CH4*ratio)>C_thres ) // && soil->CH4[l] / soildepth[l] * 1000 / epsilon_CH4 > soil->CH4[l - 1] / soildepth[l - 1] * 1000 / epsilon_CH4_u)
      {
/*
      if ((soil->CH4[l] / soildepth[l] * 1000 / epsilon_CH4*ratio)>C_thres && soil->wtable<layerbound[l] && soil->CH4[l] / soildepth[l] * 1000 / epsilon_CH4 > soil->CH4[l - 1] / soildepth[l - 1] * 1000 / epsilon_CH4_u)
      {
*/
        Q_ebull=k_e*(soil->CH4[l]/soildepth[l]/epsilon_CH4*1000*ratio-C_thres)*soildepth[l]*epsilon_CH4*1e-3;
        Q_ebull= max(0,min(soil->CH4[l],Q_ebull));
        soil->CH4[l] -= Q_ebull;
        if (l == 0)
          Q_ebull_day += Q_ebull;
        else
          soil->CH4[l-1] += Q_ebull;
        //printf("ebullition: %g layer: %d\n",Q_ebull,l);
      }
      else
        break;
    }
  }
#ifdef DEBUG
  printf("EBULL after:");
  printch4(soil->CH4);
#endif
  return Q_ebull_day;
} /* of 'ebullition */

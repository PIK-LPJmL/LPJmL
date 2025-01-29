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

#define CH4_min  0.012 /* 0.012 threshold value at which ebullition occur at totally vegetated soils g*m-3 8/1000*/
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
  Real C_thres, Q_ebull[BOTTOMLAYER], ratio, soil_moist[BOTTOMLAYER], V, epsilon_CH4[BOTTOMLAYER]; //, epsilon_CH4_u;
  Real Q_ebull2;
  int l, i;
#ifdef DEBUG
  printf("EBULL before:");
  printch4(soil->CH4);
#endif
  for (l = 0; l<BOTTOMLAYER; l++)
  {
    soil_moist[l] = getsoilmoist(soil,l);
    V = getV(soil,l);  /*soil air content (m3 air/m3 soil)*/
    epsilon_CH4[l] = max(0.001, V + soil_moist[l]*soil->wsat[l]*BCH4);
  }
  C_thres = CH4_min*(2 - fpc_all);

  for (i = 1; i <= 24; i++)
  {
    for (l = 0; l<BOTTOMLAYER; l++)
      Q_ebull[l]=0;
    for (l = LASTLAYER - 1; l >= 0; l--)
    {
  //    ratio = min((layerbound[l] - soil->wtable) / soildepth[l], 1);
      ratio=1;
      if(l<LASTLAYER-1)
      {
        if(soil_moist[l]>0.9)
          Q_ebull[l]=Q_ebull[l+1];
        else
          soil->CH4[l]+=Q_ebull[l+1];
      } 
      if ((soil->CH4[l]/soildepth[l]/epsilon_CH4[l]*1000*ratio)>C_thres ) // && soil->CH4[l] / soildepth[l] * 1000 / epsilon_CH4 > soil->CH4[l - 1] / soildepth[l - 1] * 1000 / epsilon_CH4_u)
      {
/*
      if ((soil->CH4[l] / soildepth[l] * 1000 / epsilon_CH4*ratio)>C_thres && soil->wtable<layerbound[l] && soil->CH4[l] / soildepth[l] * 1000 / epsilon_CH4 > soil->CH4[l - 1] / soildepth[l - 1] * 1000 / epsilon_CH4_u)
      {
*/
        Q_ebull2=k_e*(soil->CH4[l]/soildepth[l]/epsilon_CH4[l]*1000*ratio-C_thres)*soildepth[l]*epsilon_CH4[l]*1e-3;
        Q_ebull2= max(0,min(soil->CH4[l],Q_ebull2));
        soil->CH4[l] -= Q_ebull2;
        Q_ebull[l]+=Q_ebull2;
      }
      else
        break;
    }
  }

#ifdef DEBUG
  printf("EBULL after:");
  printch4(soil->CH4);
#endif
  return Q_ebull[0];
} /* of 'ebullition */

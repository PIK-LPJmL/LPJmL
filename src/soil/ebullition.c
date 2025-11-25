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

#define CH4_min  4     /**< threshold value at which ebullition occur at totally vegetated soils in g*m-3*/
#define k_e 1          /**< rate constant h-1 */

#ifdef DEBUG

static void printch4(const Real CH4[LASTLAYER])
{
  int i;
  for (i = 0; i<LASTLAYER; i++)
    printf(" %g", CH4[i]);
  printf("\n");
}

#endif

Real ebullition(Stand *stand,   /**< pointer to stand */
                Real T          /**< airtemp */
               )
{
  Real C_thres, Q_ebull[BOTTOMLAYER], soil_moist[BOTTOMLAYER]; //, epsilon_CH4_u;
  Real Q_ebull2;
  Real Q_ebull_day=0;
  Soil *soil;
  Pft *pft;
  int l, i,p;
  Real fpc_total_stand=0;

  soil=&stand->soil;
#ifdef DEBUG
  printf("EBULL before:");
  printch4(soil->CH4);
#endif

  foreachpft(pft, p, &stand->pftlist)
    if(stand->type->landusetype!=AGRICULTURE)                //I'm not sure about tea and cotton
      fpc_total_stand += pft->fpc*pft->par->alpha_e*pft->phen;
    else
      fpc_total_stand +=fpar(pft)*pft->par->alpha_e;

  for (l = 0; l<BOTTOMLAYER; l++)
  {
    soil_moist[l] = getsoilmoist(soil,l);
  }
  C_thres = CH4_min*(2 - fpc_total_stand);

  for (i = 1; i <= 24; i++)
  {
    for (l = 0; l<BOTTOMLAYER; l++)
      Q_ebull[l]=0;
    for (l = LASTLAYER - 1; l >= 0; l--)
    {
      if(l<LASTLAYER-1)
      {
        if(soil_moist[l]>0.95)
          Q_ebull[l]=Q_ebull[l+1];
        else
          soil->CH4[l]+=Q_ebull[l+1];
      } 
      if ((soil->CH4[l]/soildepth[l]*1000)>C_thres)
      {
        Q_ebull2=k_e*(soil->CH4[l]/soildepth[l]*1000-C_thres)*soildepth[l]*1e-3;
        Q_ebull2= max(0,min(soil->CH4[l],Q_ebull2));
        soil->CH4[l] -= Q_ebull2;
        Q_ebull[l]+=Q_ebull2;
      }
    }
    Q_ebull_day+=Q_ebull[0];
  }
#ifdef DEBUG
  printf("EBULL after:");
  printch4(soil->CH4);
#endif
  return Q_ebull_day;
} /* of 'ebullition */

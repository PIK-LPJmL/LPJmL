/**************************************************************************************/
/**                                                                                \n**/
/**               l i t t e r _ u p d a t e _ f i r e . c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real litter_update_fire(Litter *litter,const Fuel *fuel)
{
  int i,j;
  Real fuel_consum;
  /* litter update [gC/m2]*/
  fuel_consum=0;
  for(i=0;i<litter->n;i++)
  {
    fuel_consum+=litter->ag[i].trait.leaf*fuel->deadfuel_consum[0];
    litter->ag[i].trait.leaf*=(1-fuel->deadfuel_consum[0]);
    for(j=0;j<NFUELCLASS;j++)
    {
      fuel_consum+=litter->ag[i].trait.wood[j]*fuel->deadfuel_consum[j];
      litter->ag[i].trait.wood[j]*=(1-fuel->deadfuel_consum[j]);
    }
  }
  return fuel_consum;
} /* of litter_update_fire' */

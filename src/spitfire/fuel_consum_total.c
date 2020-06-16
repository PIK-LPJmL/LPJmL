/**************************************************************************************/
/**                                                                                \n**/
/**       f  u  e  l  _  c  o  n  s  u  m  _  t  o  t  a  l  .  c                  \n**/
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

Real fuel_consum_total(const Litter *litter,const Fuel *fuel)
{
  int i,l;
  Real fuel_consum;
  fuel_consum=0;
  for(l=0;l<litter->n;l++)
  {
    fuel_consum += fuel->deadfuel_consum[0]*litter->ag[l].trait.leaf;
    for (i=0;i<NFUELCLASS;i++)
      fuel_consum += fuel->deadfuel_consum[i]*litter->ag[l].trait.wood[i];
  }
  return fuel_consum;
} /* of 'fuel_consum_total' */ 

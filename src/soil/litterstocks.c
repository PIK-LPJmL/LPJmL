/**************************************************************************************/
/**                                                                                \n**/
/**                l  i  t  t  e  r  s  t  o  c  k  s  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function computes sum of all litter pools                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Stocks litterstocks(const Litter *litter /**< pointer to litter pools */
                   )                     /** \return total carbon and nitrogen (gC/m2, gN/m2) */
{
  int i,l;
  Stocks sum={0,0};
  for(l=0;l<litter->n;l++)
  {
    sum.carbon+=litter->ag[l].trait.leaf.carbon;
    sum.nitrogen+=litter->ag[l].trait.leaf.nitrogen;
    for(i=0;i<NFUELCLASS;i++)
    {
      sum.carbon+=litter->ag[l].trait.wood[i].carbon;
      sum.nitrogen+=litter->ag[l].trait.wood[i].nitrogen;
    }
    sum.carbon+=litter->bg[l].carbon;
    sum.nitrogen+=litter->bg[l].nitrogen;
  }
  return sum;
} /* of 'litterstocks' */

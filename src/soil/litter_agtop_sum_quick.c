/**************************************************************************************/
/**                                                                                \n**/
/**     l  i  t  t  e  r  _  a  g  t  o  p  _  s  u  m  _  q  u  i  c  k  .  c     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**     Function computes sum of all above-ground litter pools                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real litter_agtop_sum_quick(const Litter *litter /**< pointer to litter data */
                        )                        /** \return aboveground fast litter (gC/m2) */
{
  int i,l;
  Real sum;
  sum=0;
  for(l=0;l<litter->n;l++)
  {
    sum+=litter->item[l].agtop.leaf.carbon;
    for(i=0;i<NFUELCLASS-1;i++)
      sum+=litter->item[l].agtop.wood[i].carbon;
  }
  return sum;
} /* of 'litter_agtop_sum_quick' */

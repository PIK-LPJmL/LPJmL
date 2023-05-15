/**************************************************************************************/
/**                                                                                \n**/
/**                l  i  t  t  e  r  c  a  r  b  o  n  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
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

Real littercarbon(const Litter *litter /**< pointer to litter data */
                 )                     /** \return litter carbon (gC/m2) */
{
  int i,l;
  Real sum;
  sum=0;
  for(l=0;l<litter->n;l++)
  {
    sum+=litter->item[l].agtop.leaf.carbon+litter->item[l].agsub.leaf.carbon;
    for(i=0;i<NFUELCLASS;i++)
      sum+=litter->item[l].agtop.wood[i].carbon+litter->item[l].agsub.wood[i].carbon;
    sum+=litter->item[l].bg.carbon;
  }
  return sum;
} /* of 'littercarbon' */

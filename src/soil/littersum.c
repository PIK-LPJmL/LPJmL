/**************************************************************************************/
/**                                                                                \n**/
/**                l  i  t  t  e  r  s  u  m  .  c                                 \n**/
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

Real littersum(const Litter *litter /**< pointer to litter data */
              )                     /** \return litter carbon (gC/m2) */
{
  int i,l;
  Real sum;
  sum=0;
  for(l=0;l<litter->n;l++)
  {
    sum+=litter->ag[l].trait.leaf;
    for(i=0;i<NFUELCLASS;i++)
      sum+=litter->ag[l].trait.wood[i];
    sum+=litter->bg[l];
  }
  return sum;
} /* of 'littersum' */

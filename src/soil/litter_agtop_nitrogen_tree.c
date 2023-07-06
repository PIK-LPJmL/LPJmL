/**************************************************************************************/
/**                                                                                \n**/
/**             l i t t e r _ a g t o p _ n i t r o g e n _ t r e e . c            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** Function computes sum of all above-ground nitrogen litter pools for trees      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real litter_agtop_nitrogen_tree(const Litter *litter,int fuel)
{
  int l;
  Real sum;
  sum=0;
  if(fuel==0)
  {
    for(l=0;l<litter->n;l++)
      if(litter->item[l].pft->type==TREE)
        sum+=litter->item[l].agtop.leaf.nitrogen+litter->item[l].agtop.wood[0].nitrogen;
  }
  else
  {
    for(l=0;l<litter->n;l++)
      if(litter->item[l].pft->type==TREE)
        sum+=litter->item[l].agtop.wood[fuel].nitrogen;
  }
  return sum;
} /* of litter_agtop_nitrogen_tree */

/**************************************************************************************/
/**                                                                                \n**/
/**                m  o  i  s  t  f  a  c  t  o  r  .  c                           \n**/
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

Real moistfactor(const Litter *litter  /**< Litter pool */
                )                      /** \return moist factor */
{
  Real moistfactor,litter_sum,sum;
  int i,p;
  litter_sum=litter_ag_sum(litter);
  if(litter_sum==0)
    return 0;
  moistfactor=0;
  for(p=0;p<litter->n;p++)
  {
    sum=litter->item[p].ag.leaf.carbon;
    for(i=0;i<NFUELCLASS;i++)
      sum+=litter->item[p].ag.wood[i].carbon;
    moistfactor+=litter->item[p].pft->flam*sum;
  }
  return moistfactor/litter_sum;
} /* of 'moistfactor' */

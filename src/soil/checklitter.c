/**************************************************************************************/
/**                                                                                \n**/
/**                c  h  e  c  k  l  i  t  t  e  r  .  c                           \n**/
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

Stocks checklitter(Litter *litter)
{
  int i,l;
  Stocks sum={0,0};
  for(l=0;l<litter->n;l++)
  {
    if(litter->item[l].agtop.leaf.carbon<0)
    {
      sum.carbon+=litter->item[l].agtop.leaf.carbon;
      litter->item[l].agtop.leaf.carbon=0;
    }
    for(i=0;i<NFUELCLASS;i++)
      if(litter->item[l].agtop.wood[i].carbon<0)
      {
        sum.carbon+=litter->item[l].agtop.wood[i].carbon;
        litter->item[l].agtop.wood[i].carbon=0;
      }
    if(litter->item[l].bg.carbon<0)
    {
      sum.carbon+=litter->item[l].bg.carbon;
      litter->item[l].bg.carbon=0;
    }
    if(litter->item[l].agtop.leaf.nitrogen<0)
    {
      sum.nitrogen+=litter->item[l].agtop.leaf.nitrogen;
      litter->item[l].agtop.leaf.nitrogen=0;
    }
    for(i=0;i<NFUELCLASS;i++)
      if(litter->item[l].agtop.wood[i].nitrogen<0)
      {
        sum.nitrogen+=litter->item[l].agtop.wood[i].nitrogen;
        litter->item[l].agtop.wood[i].nitrogen=0;
      }
    if(litter->item[l].bg.nitrogen<0)
    {
      sum.nitrogen+=litter->item[l].bg.nitrogen;
      litter->item[l].bg.nitrogen=0;
    }
  }
  return sum;
} /* of 'checklitter' */

/**************************************************************************************/
/**                                                                                \n**/
/**                t  i  l  l  a  g  e  .  c                                       \n**/
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

void tillage(Soil *soil, /**< pointer to soil data */
             Real frac   /**< fraction (0..1) */
            )
{
  int i,l;

  /* move above ground litter pools to same litter structure in sub-soil (agsub) */
  for (l = 0; l < soil->litter.n; l++)
  {
    soil->litter.item[l].agsub.leaf.carbon += soil->litter.item[l].ag.leaf.carbon*frac;
    soil->litter.item[l].ag.leaf.carbon *= (1-frac);
    if (soil->litter.item[l].ag.leaf.carbon < epsilon)
      soil->litter.item[l].ag.leaf.carbon = 0;
    soil->litter.item[l].agsub.leaf.nitrogen += soil->litter.item[l].ag.leaf.nitrogen*frac;
    soil->litter.item[l].ag.leaf.nitrogen *= (1 - frac);
    if (soil->litter.item[l].ag.leaf.nitrogen < epsilon)
      soil->litter.item[l].ag.leaf.nitrogen = 0;

    for (i = 0; i < NFUELCLASS; i++)
    {
      soil->litter.item[l].agsub.wood[i].carbon += soil->litter.item[l].ag.wood[i].carbon*frac;
      soil->litter.item[l].ag.wood[i].carbon *= (1 - frac);
      if (soil->litter.item[l].ag.wood[i].carbon < epsilon)
        soil->litter.item[l].ag.wood[i].carbon = 0;
      soil->litter.item[l].agsub.wood[i].nitrogen += soil->litter.item[l].ag.wood[i].nitrogen*frac;
      soil->litter.item[l].ag.wood[i].nitrogen *= (1 - frac);
      if (soil->litter.item[l].ag.wood[i].nitrogen < epsilon)
        soil->litter.item[l].ag.wood[i].nitrogen = 0;
    }
  }

  /* do some changes to bulk density here */
  for(l=0;l<NTILLLAYER;l++)
    soil->df_tillage[l]-=(soil->df_tillage[l]-0.667*1)*param.mixing_efficiency; /* eq. 305 from APEX documentation, expressed as fraction of BD after full settling */

  /* do whatever tillage is supposed to do otherwise here */
 
} /* of 'tillage' */

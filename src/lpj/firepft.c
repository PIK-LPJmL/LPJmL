/**************************************************************************************/
/**                                                                                \n**/
/**                    f  i  r  e  p  f  t  .  c                                   \n**/
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

Stocks firepft(Litter *litter,   /**< Litter pool */
               Pftlist *pftlist, /**< list of established PFTs */
               Real fire_frac,    /**< fire fraction (0..1) */
               const Config *config
              )                  /** \return fire flux (gC/m2) */
{
  int i,p;
  Pft *pft;
  Stocks flux,flux_litter,flux_sum;
  flux_litter.carbon=flux_litter.nitrogen=flux_sum.carbon=flux_sum.nitrogen=0;
  if(isempty(pftlist)) /*if(pftlist->n==0)*/
    return flux_sum;
  foreachpft(pft,p,pftlist)
  {
    flux=fire(pft,&fire_frac);
    flux_sum.carbon+=flux.carbon;
    flux_sum.nitrogen+=flux.nitrogen;
  }
  for(p=0;p<litter->n;p++)
  {
    flux_litter.carbon+=litter->item[p].ag.leaf.carbon;
    flux_litter.nitrogen+=litter->item[p].ag.leaf.nitrogen;
    for(i=0;i<NFUELCLASS;i++)
    {
      flux_litter.carbon+=litter->item[p].ag.wood[i].carbon;
      flux_litter.nitrogen+=litter->item[p].ag.wood[i].nitrogen;
    }
    litter->item[p].ag.leaf.carbon*=(1-fire_frac);
    litter->item[p].ag.leaf.nitrogen*=(1-fire_frac);
    for(i=0;i<NFUELCLASS;i++)
    {
      getoutput(&pft->stand->cell->output,LITBURNC_WOOD,config)+=litter->item[p].ag.wood[i].carbon*fire_frac;
      litter->item[p].ag.wood[i].carbon*=(1-fire_frac);
      litter->item[p].ag.wood[i].nitrogen*=(1-fire_frac);
    }
  } /* of 'for(p=0;...)' */
  getoutput(&pft->stand->cell->output,LITBURNC,config)+=flux_litter.carbon*fire_frac;
  flux_sum.carbon+=flux_litter.carbon*fire_frac;
  flux_sum.nitrogen+=flux_litter.nitrogen*fire_frac;
  return flux_sum; 
} /* of 'firepft' */

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

Real firepft(Litter *litter,   /**< Litter pool */
             Pftlist *pftlist, /**< list of established PFTs */
             Real fire_frac    /**< fire fraction (0..1) */
            )                  /** \return fire flux (gC/m2) */
{
  int i,p;
  Pft *pft;
  Real flux,flux_litter;
  if(isempty(pftlist)) /*if(pftlist->n==0)*/
    return 0.0;
  flux=flux_litter=0;
  foreachpft(pft,p,pftlist)
  {
    flux+=fire(pft,&fire_frac);
  }
  for(p=0;p<litter->n;p++)
  {
    flux_litter+=litter->ag[p].trait.leaf;
    for(i=0;i<NFUELCLASS;i++)
      flux_litter+=litter->ag[p].trait.wood[i];
    litter->ag[p].trait.leaf*=(1-fire_frac);
    for(i=0;i<NFUELCLASS;i++)
      litter->ag[p].trait.wood[i]*=(1-fire_frac);
  } /* of 'for(p=0;...)' */
  return flux+flux_litter*fire_frac; 
} /* of 'firepft' */

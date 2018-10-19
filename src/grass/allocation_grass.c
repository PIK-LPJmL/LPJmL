/**************************************************************************************/
/**                                                                                \n**/
/**       a  l  l  o  c  a  t  i  o  n  _  g  r  a  s  s  .  c                     \n**/
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
#include "grass.h"
#include "landuse.h"

Bool allocation_grass(Litter *litter, /**< litter pool */
                      Pft *pft        /**< pointer to PFT */
                     )                /** \return TRUE on death */
{
  Real bm_inc_ind,lmtorm,lmtormscal;
  Grassphys inc_ind;
  Pftgrass *grass;
  bm_inc_ind=pft->bm_inc/pft->nind; 

  if (pft->stand->growing_days>epsilon)
  {
    lmtormscal = pft->wscal_mean/pft->stand->growing_days;
    lmtorm=getpftpar(pft,lmro_ratio)*lmtormscal;
  }
  else
    lmtorm=getpftpar(pft,lmro_ratio);

  pft->wscal_mean=0;
  grass=pft->data;
  /* daily allocation for new grasslands */
  if(pft->stand->type->landusetype==GRASSLAND || pft->stand->type->landusetype==BIOMASS_GRASS) 
  {
    /* otherwise there wouldn't be any growth */
    lmtorm=max(lmtorm,0.25);

    if(bm_inc_ind>0.0)
    { /* try to achieve today's lmtorm */

      inc_ind.leaf=(bm_inc_ind+grass->ind.root-grass->ind.leaf/lmtorm)/(1.0+1.0/lmtorm);
      /* no reallocation from leaves to roots */
      inc_ind.leaf=max(inc_ind.leaf,0.0);
      /* no reallocation from roots to leaves */
      inc_ind.leaf=min(inc_ind.leaf,bm_inc_ind);

      inc_ind.root=bm_inc_ind-inc_ind.leaf;
    }
    else 
    {
      /* negative bm_inc reduces leaves and roots proportionally */
      inc_ind.leaf=bm_inc_ind*grass->ind.leaf/(grass->ind.root+grass->ind.leaf);
      inc_ind.root=bm_inc_ind*grass->ind.root/(grass->ind.root+grass->ind.leaf);
    }
  }
  else /* this is for natural vegetation with yearly full reallocation */
  {
    if (lmtorm<1.0e-10)
    {
      inc_ind.leaf=0.0;
      inc_ind.root=bm_inc_ind;
    }
    else
    {
      inc_ind.leaf=(bm_inc_ind+grass->ind.root-grass->ind.leaf/lmtorm)/(1.0+1.0/lmtorm);
      if(inc_ind.leaf<0.0)  /*NEGATIVE ALLOCATION TO LEAF MASS */
      {
        inc_ind.root=bm_inc_ind;
        inc_ind.leaf=(grass->ind.root+inc_ind.root)*lmtorm-grass->ind.leaf;
        litter->ag[pft->litter].trait.leaf+=-inc_ind.leaf*pft->nind;
        update_fbd_grass(litter,pft->par->fuelbulkdensity,
          -inc_ind.leaf*pft->nind);
      }
      else
        inc_ind.root=bm_inc_ind-inc_ind.leaf;
    }
  }

  grass->ind.leaf+=inc_ind.leaf;
  grass->ind.root+=inc_ind.root;

  return (grass->ind.leaf<0 || grass->ind.root<0.0);
} /* of 'allocation_grass' */ 


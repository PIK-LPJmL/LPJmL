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
                      Pft *pft,       /**< pointer to PFT */
                      Real *fpc_inc   /**< fpc increment */
                     )                /** \return TRUE on death */
{
  Stocks bm_inc_ind={0,0};
  Real lmtorm,lmtormscal,vscal;
  Grassphys inc_ind;
  Pftgrass *grass;
  const Pftgrasspar *grasspar;
  Real leaf_carbon_lastday, root_carbon_lastday;
  Real leaf_nitrogen_lastday, root_nitrogen_lastday;
  Real a;
  grasspar=pft->par->data;
  grass=pft->data;

  bm_inc_ind.carbon=pft->bm_inc.carbon/pft->nind;
  bm_inc_ind.nitrogen=pft->bm_inc.nitrogen/pft->nind;
  vscal=min(1,pft->vscal/pft->stand->growing_days);
  lmtorm=getpftpar(pft,lmro_ratio)*min(vscal,pft->wscal_mean/pft->stand->growing_days);

  if (pft->stand->growing_days>epsilon)
  {
    lmtormscal = pft->wscal_mean/pft->stand->growing_days;
    lmtorm=getpftpar(pft,lmro_ratio)*min(vscal,lmtormscal);
  }
  else
    lmtorm=getpftpar(pft,lmro_ratio);

  pft->wscal_mean=pft->vscal=0;
  /* daily allocation for new grasslands */
  if(pft->stand->type->landusetype==GRASSLAND || pft->stand->type->landusetype==BIOMASS_GRASS) 
  {
    /* otherwise there wouldn't be any growth */
    lmtorm=max(lmtorm,0.25);

    if(bm_inc_ind.carbon>0.0)
    { /* try to achieve today's lmtorm */

      inc_ind.leaf.carbon=(bm_inc_ind.carbon+grass->ind.root.carbon-grass->ind.leaf.carbon/lmtorm)/(1.0+1.0/lmtorm);
      /* no reallocation from leaves to roots */
      inc_ind.leaf.carbon=max(inc_ind.leaf.carbon,0.0);
      /* no reallocation from roots to leaves */
      inc_ind.leaf.carbon=min(inc_ind.leaf.carbon,bm_inc_ind.carbon);

      inc_ind.root.carbon=bm_inc_ind.carbon-inc_ind.leaf.carbon;
    }
    else 
    {
      /* negative bm_inc reduces leaves and roots proportionally */
      inc_ind.leaf.carbon=bm_inc_ind.carbon*grass->ind.leaf.carbon/(grass->ind.root.carbon+grass->ind.leaf.carbon);
      inc_ind.root.carbon=bm_inc_ind.carbon*grass->ind.root.carbon/(grass->ind.root.carbon+grass->ind.leaf.carbon);
    }
  }
  else /* this is for natural vegetation with yearly full reallocation */
  {
    if (lmtorm<1.0e-10)
    {
      inc_ind.leaf.carbon=0.0;
      inc_ind.root.carbon=bm_inc_ind.carbon;
    }
    else
    {
      inc_ind.leaf.carbon=(bm_inc_ind.carbon+grass->ind.root.carbon-grass->ind.leaf.carbon/lmtorm)/(1.0+1.0/lmtorm);
      if(inc_ind.leaf.carbon<0.0)  /*NEGATIVE ALLOCATION TO LEAF MASS */
      {
        inc_ind.root.carbon=bm_inc_ind.carbon;
        inc_ind.leaf.carbon=(grass->ind.root.carbon+inc_ind.root.carbon)*lmtorm-grass->ind.leaf.carbon;
        litter->ag[pft->litter].trait.leaf.carbon+=-inc_ind.leaf.carbon*pft->nind;
        update_fbd_grass(litter,pft->par->fuelbulkdensity,
          -inc_ind.leaf.carbon*pft->nind);
      }
      else
        inc_ind.root.carbon=bm_inc_ind.carbon-inc_ind.leaf.carbon;
    }
  }

  grass->ind.leaf.carbon+=inc_ind.leaf.carbon;
  grass->ind.root.carbon+=inc_ind.root.carbon;
  leaf_nitrogen_lastday = grass->ind.leaf.nitrogen;
  root_nitrogen_lastday = grass->ind.root.nitrogen;
  //printf("NC_leaf, NC_root: %g %g\n",grass->ind.leaf.nitrogen/grass->ind.leaf.carbon,grass->ind.root.nitrogen/grass->ind.root.carbon);
  if (grass->ind.leaf.carbon >0 && bm_inc_ind.nitrogen>0)
  {
    a = (grasspar->ratio*grass->ind.root.nitrogen*grass->ind.leaf.carbon - grass->ind.leaf.nitrogen*grass->ind.root.carbon + grass->ind.leaf.carbon*bm_inc_ind.nitrogen*grasspar->ratio) / 
      (bm_inc_ind.nitrogen*grass->ind.root.carbon + bm_inc_ind.nitrogen*grasspar->ratio*grass->ind.leaf.carbon);
    if (a<0)
      a = 0;
    if (a>1)
      a = 1;
    if ((grass->ind.leaf.nitrogen + a*bm_inc_ind.nitrogen) / grass->ind.leaf.carbon>pft->par->ncleaf.high)
    {
      grass->ind.leaf.nitrogen = grass->ind.leaf.carbon*pft->par->ncleaf.high;
      pft->bm_inc.nitrogen -= grass->ind.leaf.nitrogen - leaf_nitrogen_lastday;
      if (pft->bm_inc.nitrogen >= grass->ind.root.carbon*pft->par->ncleaf.high / grasspar->ratio - grass->ind.root.nitrogen)
      {
        grass->ind.root.nitrogen = grass->ind.root.carbon*pft->par->ncleaf.high / grasspar->ratio;
        pft->bm_inc.nitrogen -= grass->ind.root.nitrogen - root_nitrogen_lastday;
      }
      else
      {
        grass->ind.root.nitrogen += pft->bm_inc.nitrogen;
        pft->bm_inc.nitrogen = 0;
      }
    }
    else
    {
      grass->ind.leaf.nitrogen += a*bm_inc_ind.nitrogen;
      grass->ind.root.nitrogen += (1 - a)*bm_inc_ind.nitrogen;
      pft->bm_inc.nitrogen = 0;
      /* testing if there is too much carbon for allowed NC ratios */
      if (grass->ind.leaf.nitrogen / grass->ind.leaf.carbon<pft->par->ncleaf.low) {
        leaf_carbon_lastday = grass->ind.leaf.carbon;
        root_carbon_lastday = grass->ind.root.carbon;
        grass->ind.leaf.carbon = grass->ind.leaf.nitrogen / pft->par->ncleaf.low;
        if (root_carbon_lastday>grass->ind.root.nitrogen / pft->par->ncleaf.low*grasspar->ratio)
        {
          grass->ind.root.carbon = grass->ind.root.nitrogen / pft->par->ncleaf.low*grasspar->ratio;
          litter->bg[pft->litter].carbon += root_carbon_lastday - grass->ind.root.carbon;
        }
        litter->ag[pft->litter].trait.leaf.carbon += leaf_carbon_lastday - grass->ind.leaf.carbon;
        update_fbd_grass(litter, pft->par->fuelbulkdensity,
                         leaf_carbon_lastday - grass->ind.leaf.carbon);
      }
    }
  }
  pft->nleaf = grass->ind.leaf.nitrogen;

  *fpc_inc=fpc_grass(pft);
 // fprintf(stdout,"allocation grass leaf %g root %g fpcinc %g\n",grass->ind.leaf,grass->ind.root,fpc_inc);
  return (grass->ind.leaf.carbon<0 || grass->ind.root.carbon<0.0 || pft->fpc<=1e-20);
} /* of 'allocation_grass' */ 

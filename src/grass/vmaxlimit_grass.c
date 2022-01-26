/**************************************************************************************/
/**                                                                                \n**/
/**               v  m  a  x  l  i  m  i  t  _  g  r  a  s  s  .  c                \n**/
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

Real vmaxlimit_grass(const Pft *pft, /**< pointer to PFT */
                     Real daylength, /**< day length (h) */
                     Real temp       /**< temperature (deg C) */
                    )                /** \return vmax (gC/m2/day) */
{
  const Pftgrass *grass;
  Real vmax,NC_leaf;
  grass=pft->data; 
  NC_leaf=(grass->ind.leaf.nitrogen+pft->bm_inc.nitrogen/pft->nind*grass->falloc.leaf)/(grass->ind.leaf.carbon+pft->bm_inc.carbon/pft->nind*grass->falloc.leaf);
  //vmax=((grass->ind.leaf.nitrogen-pft->par->ncleaf.low*grass->ind.leaf.carbon)*pft->nind)/exp(-param.k_temp*(temp-25))/f_lai(lai_grass(pft))/param.p/0.02314815*daylength;
  vmax=(pft->nleaf-pft->par->ncleaf.low*(grass->ind.leaf.carbon*pft->nind+pft->bm_inc.carbon*grass->falloc.leaf))/exp(-param.k_temp*(temp-25))/f_lai(lai_grass(pft))/param.p/0.02314815*daylength;
  //printf("in VMAXLIMIT vmaxold: %g vmaxnew: %g NC_leaf %g nc.leaf.low  %g leaf.median  %g nc.leaf.high  %g \n",pft->vmax,vmax,NC_leaf,pft->par->ncleaf.low,pft->par->ncleaf.median,pft->par->ncleaf.high);
  return min(pft->vmax,max(vmax,0.0001));
  } /* of 'vmaxlimit_grass' */

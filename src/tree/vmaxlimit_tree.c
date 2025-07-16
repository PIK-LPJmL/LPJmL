/**************************************************************************************/
/**                                                                                \n**/
/**               v  m  a  x  l  i  m  i  t  _  t  r  e  e  .  c                   \n**/
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
#include "tree.h"

Real vmaxlimit_tree(const Pft *pft, /**< pointer to PFT */
                    Real daylength, /**< day length (h) */
                    Real temp       /**< temperature (deg C) */
                   )                /** \return vmax (gC/m2/day) */
{
  Real vmax;
  const Pfttree *tree;
  tree=pft->data;
  //vmax=(pft->nleaf-param.n0*0.001*(pft->bm_inc.carbon*tree->falloc.leaf+tree->ind.leaf.carbon*pft->nind-tree->turn_litt.leaf.carbon)/CCpDM)/exp(-param.k_temp*(temp-25))/f_lai(lai_tree(pft))/param.p/9.6450617e-4;
  vmax=(pft->nleaf-pft->par->ncleaf.low*(pft->bm_inc.carbon*tree->falloc.leaf+tree->ind.leaf.carbon*pft->nind-tree->turn_litt.leaf.carbon))/exp(66530/8.314*(1/degCtoK(temp)-1/degCtoK(25)))/0.006*(NSECONDSDAY*cmass*1e-6);
  return min(pft->vmax,max(vmax,0.0001));
} /* of 'vmaxlimit_tree' */

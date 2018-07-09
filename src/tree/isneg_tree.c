/**************************************************************************************/
/**                                                                                \n**/
/**               i  s  n  e  g  _  t  r  e  e  .  c                               \n**/
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

Bool isneg_tree(const Pft *pft)
{
  Pfttree *tree;
  tree=pft->data;
#ifdef DEBUG3
  printf("isneg: %g %g %g %g %g %g %g %g\n",tree->ind.leaf.carbon*pft->nind,tree->ind.root.carbon*pft->nind,
         tree->ind.sapwood.carbon*pft->nind,tree->ind.heartwood.carbon*pft->nind,tree->ind.debt.carbon*pft->nind,pft->fpc,pft->nind,pft->bm_inc.carbon);
#endif
  return ((tree->ind.leaf.carbon+tree->ind.root.carbon+tree->ind.sapwood.carbon+tree->ind.heartwood.carbon-tree->ind.debt.carbon)<0.0 
         || tree->ind.root.carbon<0.0 || tree->ind.leaf.carbon<0.0 || tree->ind.sapwood.carbon<0.0 || tree->ind.heartwood.carbon<0.0
         ||pft->fpc<=1e-20 || pft->nind<=1e-16);
} /* of 'isneg_tree' */

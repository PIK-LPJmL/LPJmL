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

Bool isneg_tree(const Pft *pft /**< pointer to tree PFT */
               )               /** \return TRUE if one pool size is negative */
{
  Pfttree *tree;
  tree=pft->data;
#ifdef DEBUG3
  printf("isneg: %g %g %g %g %g %g %g %g\n",tree->ind.leaf*pft->nind,tree->ind.root*pft->nind,
         tree->ind.sapwood*pft->nind,tree->ind.heartwood*pft->nind,tree->ind.debt*pft->nind,pft->fpc,pft->nind,pft->bm_inc);
#endif
  return ((tree->ind.leaf+tree->ind.root+tree->ind.sapwood+tree->ind.heartwood-tree->ind.debt)<0.0 
         || tree->ind.root<0.0 || tree->ind.leaf<0.0 || tree->ind.sapwood<0.0 || tree->ind.heartwood<0.0
         ||pft->fpc<=1e-20 || pft->nind<=1e-16);
} /* of 'isneg_tree' */

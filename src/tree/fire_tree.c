/**************************************************************************************/
/**                                                                                \n**/
/**                    f  i  r  e  _  t  r  e  e  .  c                             \n**/
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

Stocks fire_tree(Pft *pft,      /**< pointer to tree PFT */
                 Real *fireprob /**< fire probability */
                )               /** \return burnt stock (gC/m2,gN/m2) */
{
  Real disturb;
  Stocks flux;
  Pfttree *tree;
  tree=pft->data;
  disturb=(1-pft->par->resist)**fireprob;
  flux.carbon=disturb*pft->nind*(tree->ind.leaf.carbon+tree->ind.sapwood.carbon+
                          tree->ind.heartwood.carbon-tree->ind.debt.carbon+tree->ind.root.carbon+tree->excess_carbon);
  flux.nitrogen=disturb*pft->nind*(tree->ind.leaf.nitrogen+tree->ind.sapwood.nitrogen+
                          tree->ind.heartwood.nitrogen-tree->ind.debt.nitrogen+tree->ind.root.nitrogen);
  flux.nitrogen+=pft->bm_inc.nitrogen*disturb;
  pft->bm_inc.nitrogen*=(1-disturb);
  pft->nind*=(1-disturb);
  /* Include fruits even though fruit trees do not experience fire at the moment. */
  flux.carbon+=disturb*tree->fruit.carbon;
  flux.nitrogen+=disturb*tree->fruit.nitrogen;
  tree->fruit.carbon*=(1-disturb);
  tree->fruit.nitrogen*=(1-disturb);
  return flux;
} /* of 'fire_tree' */

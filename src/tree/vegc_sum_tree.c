/**************************************************************************************/
/**                                                                                \n**/
/**                 v  e  g  c  _  s  u  m  _  t  r  e  e  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates vegetation carbon of tree PFTs                         \n**/
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

Real vegc_sum_tree(const Pft *pft /**< pointer to tree PFT */
                  )               /** \return vegetation carbon (gC/m2) */
{
  const Pfttree *tree;
  tree=pft->data;
  return (phys_sum_tree(tree->ind)-tree->ind.debt)*pft->nind-tree->turn_litt.leaf;
} /* of 'vegc_sum_tree' */

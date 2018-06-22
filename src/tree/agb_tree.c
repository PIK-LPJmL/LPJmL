/**************************************************************************************/
/**                                                                                \n**/
/**                    a  g  b  _  t  r  e  e  .  c                                \n**/
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

Real agb_tree(const Pft *pft)
{
  const Pfttree *tree;
  tree=pft->data;
  return (tree->ind.leaf+tree->ind.heartwood+tree->ind.sapwood-tree->ind.debt)*pft->nind;
} /* of 'agb_tree' */

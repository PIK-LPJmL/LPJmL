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

Real fire_tree(Pft *pft,Real *fireprob)
{
  Real disturb,flux;
  Pfttree *tree;
  tree=pft->data;
  disturb=(1-pft->par->resist)**fireprob;
  flux=disturb*pft->nind*(tree->ind.leaf+tree->ind.sapwood+
                          tree->ind.heartwood-tree->ind.debt+tree->ind.root);
  pft->nind*=(1-disturb);
  return flux;
} /* of 'fire_tree' */

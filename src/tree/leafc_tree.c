/**************************************************************************************/
/**                                                                                \n**/
/**                        l  e  a f  c  _  t  r  e  e  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates leaf carbon                                            \n**/
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

Real leafc_tree(const Pft *pft /**< pointer to tree PFT */
               )               /** \return leaf carbon (gC/m2) */
{
  const Pfttree *tree;
  tree=pft->data;
  return tree->ind.leaf.carbon*pft->nind;
} /* of 'leafc_tree' */

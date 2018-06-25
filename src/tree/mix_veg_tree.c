/**************************************************************************************/
/**                                                                                \n**/
/**     m  i  x  _  v  e  g  _  t  r  e  e  .  c                                   \n**/
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

void mix_veg_tree(Pft *pft,Real scaler)
{
  Pfttree *tree;
  tree=pft->data;
  pft->nind*=scaler;
  tree->turn_litt.leaf*=scaler;
} /* of 'mix_veg_tree' */

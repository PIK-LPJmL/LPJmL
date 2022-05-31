/**************************************************************************************/
/**                                                                                \n**/
/**              f  r  e  e  _  t  r  e  e  .  c                                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function deallocates tree-specific variables                               \n**/
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

void free_tree(Pft *pft)
{
  Pfttree *tree;
  tree=pft->data;
  pft->stand->soil.litter.item[pft->litter].ag.leaf.carbon+=tree->turn.leaf.carbon*pft->nind-tree->turn_litt.leaf.carbon;
  pft->stand->soil.litter.item[pft->litter].ag.leaf.nitrogen+=tree->turn.leaf.nitrogen*pft->nind-tree->turn_litt.leaf.nitrogen;
  update_fbd_tree(&pft->stand->soil.litter,pft->par->fuelbulkdensity,tree->turn.leaf.carbon*pft->nind-tree->turn_litt.leaf.carbon,0);
  pft->stand->soil.litter.item[pft->litter].bg.carbon+=tree->turn.root.carbon*pft->nind-tree->turn_litt.root.carbon;
  pft->stand->soil.litter.item[pft->litter].bg.nitrogen+=tree->turn.root.nitrogen*pft->nind-tree->turn_litt.root.nitrogen;
  free(pft->data);
} /* of 'free_tree' */

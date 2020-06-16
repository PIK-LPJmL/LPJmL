/**************************************************************************************/
/**                                                                                \n**/
/**              f  r  e  e  _  t  r  e  e  .  c                                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
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
  pft->stand->soil.litter.ag[pft->litter].trait.leaf+=tree->turn.leaf*pft->nind-tree->turn_litt.leaf;
  update_fbd_tree(&pft->stand->soil.litter,pft->par->fuelbulkdensity,tree->turn.leaf*pft->nind-tree->turn_litt.leaf,0);
  pft->stand->soil.litter.bg[pft->litter]+=tree->turn.root*pft->nind-tree->turn_litt.root;
  free(pft->data);
} /* of 'free_tree' */

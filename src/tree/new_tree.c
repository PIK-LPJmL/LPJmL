/**************************************************************************************/
/**                                                                                \n**/
/**                       n  e  w  _  t  r  e  e  .  c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**     Function allocates and initializes tree-specific variables                 \n**/
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

void new_tree(Pft *pft, /**< Parameter of pft */
              int UNUSED(year),
              int UNUSED(day)
             )         
{
  Pfttree *tree;
  tree=new(Pfttree);
  check(tree);
  pft->data=tree;
  init_tree(pft);
  tree->ind.root=tree->ind.sapwood=tree->ind.heartwood=tree->ind.leaf=0.0;
  tree->turn.root=tree->turn.sapwood=tree->turn.leaf=tree->turn_litt.leaf=tree->turn_litt.root=0.0;
  tree->ind.debt=tree->gddtw=tree->aphen_raingreen=0.0;
  tree->height=tree->crownarea=0.0;
  tree->isphen=FALSE;
} /* of 'new_tree' */

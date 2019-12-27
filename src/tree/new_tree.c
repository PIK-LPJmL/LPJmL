/**************************************************************************************/
/**                                                                                \n**/
/**                       n  e  w  _  t  r  e  e  .  c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
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
              int UNUSED(day),
              int UNUSED(with_nitrogen)
             )         
{
  Pfttree *tree;
  Pfttreepar *treepar;
  Real sum;
  tree=new(Pfttree);
  check(tree);
  pft->data=tree;
  treepar=pft->par->data;
  init_tree(pft);
  tree->excess_carbon=0.0;
  tree->ind.root.carbon=tree->ind.sapwood.carbon=tree->ind.heartwood.carbon=tree->ind.leaf.carbon=0.0;
  tree->ind.root.nitrogen=tree->ind.sapwood.nitrogen=tree->ind.heartwood.nitrogen=tree->ind.leaf.nitrogen=0.0;
  tree->turn.root.carbon=tree->turn.leaf.carbon=tree->turn_litt.leaf.carbon=tree->turn_litt.root.carbon=0.0;
  tree->turn.root.nitrogen=tree->turn.leaf.nitrogen=tree->turn_litt.leaf.nitrogen=tree->turn_litt.root.nitrogen=0.0;
  tree->turn_nbminc=0.0;
  sum=treepar->sapl.leaf.carbon+treepar->sapl.root.carbon+treepar->sapl.sapwood.carbon;
  tree->falloc.leaf=treepar->sapl.leaf.carbon/sum;
  tree->falloc.root=treepar->sapl.root.carbon/sum;
  tree->falloc.sapwood=treepar->sapl.sapwood.carbon/sum;
  tree->ind.debt.carbon=tree->ind.debt.nitrogen=tree->gddtw=tree->aphen_raingreen=0.0;
  tree->height=tree->crownarea=0.0;
  tree->isphen=FALSE;
  pft->vmax=0;
} /* of 'new_tree' */

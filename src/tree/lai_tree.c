/**************************************************************************************/
/**                                                                                \n**/
/**                       l  a  i  _  t  r  e  e  .  c                             \n**/
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

Real lai_tree(const Pft *pft)
{
  Pfttree *tree;
  tree=pft->data;
    
  return (tree->crownarea>0.0) ? 
         tree->ind.leaf*getpftpar(pft,sla)/tree->crownarea : 0;
} /* of 'lai_tree' */
/*
- this function is called in fpc_tree() and returns the maximum lai of a tree (last year's LAI)
*/

Real actual_lai_tree(const Pft *pft)
{
  Pfttree *tree;
  tree=pft->data;
    
  return (tree->crownarea>0.0) ? 
    tree->ind.leaf*getpftpar(pft,sla)/tree->crownarea*pft->phen : 0;
} /* of 'lai_tree' */
/*
- this function is called in interception() and returns the actual lai of a tree (multiplied with phen)
*/

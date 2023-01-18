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
  //pft->nind*=scaler;
  pft->bm_inc.nitrogen*=scaler;
  pft->bm_inc.carbon*=scaler; /*not necessary as Carbon should always be zero when this function is called?*/
  pft->establish.carbon*=scaler;
  pft->establish.nitrogen*=scaler;

  tree->excess_carbon*=scaler;
  tree->fruit.carbon*=scaler;
  tree->fruit.nitrogen*=scaler;
  tree->ind.leaf.carbon*=scaler;
  tree->ind.leaf.nitrogen*=scaler;
  tree->ind.root.carbon*=scaler;
  tree->ind.root.nitrogen*=scaler;
  tree->ind.sapwood.carbon*=scaler;
  tree->ind.sapwood.nitrogen*=scaler;
  tree->ind.heartwood.carbon*=scaler;
  tree->ind.heartwood.nitrogen*=scaler;
  tree->ind.debt.carbon*=scaler;
  tree->ind.debt.nitrogen*=scaler;

  tree->turn.root.carbon*=scaler;
  tree->turn.root.nitrogen*=scaler;
  tree->turn.leaf.carbon*=scaler;
  tree->turn.leaf.nitrogen*=scaler;
  tree->turn_litt.leaf.carbon*=scaler;
  tree->turn_litt.leaf.nitrogen*=scaler;
  tree->turn_litt.root.carbon*=scaler;
  tree->turn_litt.root.nitrogen*=scaler;
} /* of 'mix_veg_tree' */

Bool mix_veg_stock_tree(Pft *pft1,Pft *pft2,Real frac1, Real frac2,const Config *config)
{
  Pfttree *tree1;
  Pfttree *tree2;
  Pft *pft;
  Bool isdead=FALSE;
  int p,id;
  Real nind,test;
  tree1=pft1->data;
  tree2=pft2->data;
  nind=(pft1->nind*frac1+pft2->nind*frac2)/(frac1+frac2);
  test=pft1->nind*frac1+pft2->nind*frac2;
  if (test>epsilon)
  {
    tree1->ind.leaf.carbon=(pft1->nind*frac1*tree1->ind.leaf.carbon+pft2->nind*frac2*tree2->ind.leaf.carbon)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->excess_carbon=(pft1->nind*frac1*tree1->excess_carbon+pft2->nind*frac2*tree2->excess_carbon)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.root.carbon=(pft1->nind*frac1*tree1->ind.root.carbon+pft2->nind*frac2*tree2->ind.root.carbon)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.sapwood.carbon=(pft1->nind*frac1*tree1->ind.sapwood.carbon+pft2->nind*frac2*tree2->ind.sapwood.carbon)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.heartwood.carbon=(pft1->nind*frac1*tree1->ind.heartwood.carbon+pft2->nind*frac2*tree2->ind.heartwood.carbon)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.debt.carbon=(pft1->nind*frac1*tree1->ind.debt.carbon+pft2->nind*frac2*tree2->ind.debt.carbon)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.leaf.nitrogen=(pft1->nind*frac1*tree1->ind.leaf.nitrogen+pft2->nind*frac2*tree2->ind.leaf.nitrogen)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.root.nitrogen=(pft1->nind*frac1*tree1->ind.root.nitrogen+pft2->nind*frac2*tree2->ind.root.nitrogen)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.sapwood.nitrogen=(pft1->nind*frac1*tree1->ind.sapwood.nitrogen+pft2->nind*frac2*tree2->ind.sapwood.nitrogen)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.heartwood.nitrogen=(pft1->nind*frac1*tree1->ind.heartwood.nitrogen+pft2->nind*frac2*tree2->ind.heartwood.nitrogen)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.debt.nitrogen=(pft1->nind*frac1*tree1->ind.debt.nitrogen+pft2->nind*frac2*tree2->ind.debt.nitrogen)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->turn.leaf.carbon=(pft1->nind*frac1*tree1->turn.leaf.carbon+pft2->nind*frac2*tree2->turn.leaf.carbon)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->turn.root.carbon=(pft1->nind*frac1*tree1->turn.root.carbon+pft2->nind*frac2*tree2->turn.root.carbon)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->turn.leaf.nitrogen=(pft1->nind*frac1*tree1->turn.leaf.nitrogen+pft2->nind*frac2*tree2->turn.leaf.nitrogen)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->turn.root.nitrogen=(pft1->nind*frac1*tree1->turn.root.nitrogen+pft2->nind*frac2*tree2->turn.root.nitrogen)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->turn_litt.leaf.carbon=(tree1->turn_litt.leaf.carbon*frac1+tree2->turn_litt.leaf.carbon*frac2)/(frac1+frac2);
    tree1->turn_litt.root.carbon=(tree1->turn_litt.root.carbon*frac1+tree2->turn_litt.root.carbon*frac2)/(frac1+frac2);
    tree1->turn_litt.leaf.nitrogen=(tree1->turn_litt.leaf.nitrogen*frac1+tree2->turn_litt.leaf.nitrogen*frac2)/(frac1+frac2);
    tree1->turn_litt.root.nitrogen=(tree1->turn_litt.root.nitrogen*frac1+tree2->turn_litt.root.nitrogen*frac2)/(frac1+frac2);
    pft1->nind=nind;
    pft1->bm_inc.nitrogen=(frac1*pft1->bm_inc.nitrogen+frac2*pft2->bm_inc.nitrogen)/(frac1+frac2);
    pft1->bm_inc.carbon=(frac1*pft1->bm_inc.carbon+frac2*pft2->bm_inc.carbon)/(frac1+frac2);
    pft1->establish.carbon=(pft1->establish.carbon*frac1+pft2->establish.carbon*frac2)/(frac1+frac2);
    pft1->establish.nitrogen=(pft1->establish.nitrogen*frac1+pft2->establish.nitrogen*frac2)/(frac1+frac2);
    tree1->fruit.carbon=(frac1*tree1->fruit.carbon+frac2*tree2->fruit.carbon)/(frac1+frac2);
    tree1->fruit.nitrogen=(frac1*tree1->fruit.nitrogen+frac2*tree2->fruit.nitrogen)/(frac1+frac2);
  }
  else
  {
    Soil *soil1, *soil2;
    soil1=&pft1->stand->soil;
    soil2=&pft2->stand->soil;
    litter_update(&pft1->stand->soil.litter,pft1,pft1->nind,config);
    litter_update(&pft2->stand->soil.litter,pft2,pft2->nind,config);
    isdead=TRUE;
  }
  return isdead;
  //fpc_tree(pft1);
} /* of 'mix_veg_stock_tree' */

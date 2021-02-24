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
  pft->bm_inc.nitrogen*=scaler;
  pft->bm_inc.carbon*=scaler; /*not necessary as Carbon should always be zero when this function is called?*/
  tree->fruit.carbon*=scaler;
  tree->fruit.nitrogen*=scaler;
  tree->turn_litt.leaf.carbon*=scaler;
  tree->turn_litt.leaf.nitrogen*=scaler;
} /* of 'mix_veg_tree' */

void mix_veg_stock_tree(Pft *pft1,Pft *pft2,Real frac1, Real frac2)
{
  Pfttree *tree1;
  Pfttree *tree2;
  Real nind,test;
  tree1=pft1->data;
  tree2=pft2->data;
  nind=(pft1->nind*frac1+pft2->nind*frac2)/(frac1+frac2);
  test=pft1->nind*frac1+pft2->nind*frac2;
  pft1->bm_inc.nitrogen=(frac1*pft1->bm_inc.nitrogen+frac2*pft2->bm_inc.nitrogen)/(frac1+frac2);
  pft1->bm_inc.carbon=(frac1*pft1->bm_inc.carbon+frac2*pft2->bm_inc.carbon)/(frac1+frac2);
  if (test>epsilon)
  {
    tree1->ind.leaf.carbon=(pft1->nind*frac1*tree1->ind.leaf.carbon+pft2->nind*frac2*tree2->ind.leaf.carbon)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.root.carbon=(pft1->nind*frac1*tree1->ind.root.carbon+pft2->nind*frac2*tree2->ind.root.carbon)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.sapwood.carbon=(pft1->nind*frac1*tree1->ind.sapwood.carbon+pft2->nind*frac2*tree2->ind.sapwood.carbon)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.heartwood.carbon=(pft1->nind*frac1*tree1->ind.heartwood.carbon+pft2->nind*frac2*tree2->ind.heartwood.carbon)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.debt.carbon=(pft1->nind*frac1*tree1->ind.debt.carbon+pft2->nind*frac2*tree2->ind.debt.carbon)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.leaf.nitrogen=(pft1->nind*frac1*tree1->ind.leaf.nitrogen+pft2->nind*frac2*tree2->ind.leaf.nitrogen)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.root.nitrogen=(pft1->nind*frac1*tree1->ind.root.nitrogen+pft2->nind*frac2*tree2->ind.root.nitrogen)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.sapwood.nitrogen=(pft1->nind*frac1*tree1->ind.sapwood.nitrogen+pft2->nind*frac2*tree2->ind.sapwood.nitrogen)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.heartwood.nitrogen=(pft1->nind*frac1*tree1->ind.heartwood.nitrogen+pft2->nind*frac2*tree2->ind.heartwood.nitrogen)/(pft1->nind*frac1+pft2->nind*frac2);
    tree1->ind.debt.nitrogen=(pft1->nind*frac1*tree1->ind.debt.nitrogen+pft2->nind*frac2*tree2->ind.debt.nitrogen)/(pft1->nind*frac1+pft2->nind*frac2);
    pft1->nind=nind;
  }
/*  tree1->turn.leaf=(tree1->turn.leaf*frac1+tree2->turn.leaf*frac2)/(frac1+frac2);
  tree1->turn.root=(tree1->turn.root*frac1+tree2->turn.root*frac2)/(frac1+frac2);
  tree1->turn.sapwood=(tree1->turn.sapwood*frac1+tree2->turn.sapwood*frac2)/(frac1+frac2);*/
  tree1->turn_litt.leaf.carbon=(tree1->turn_litt.leaf.carbon*frac1+tree2->turn_litt.leaf.carbon*frac2)/(frac1+frac2);
  tree1->turn_litt.root.carbon=(tree1->turn_litt.root.carbon*frac1+tree2->turn_litt.root.carbon*frac2)/(frac1+frac2);
  tree1->turn_litt.leaf.nitrogen=(tree1->turn_litt.leaf.nitrogen*frac1+tree2->turn_litt.leaf.nitrogen*frac2)/(frac1+frac2);
  tree1->turn_litt.root.nitrogen=(tree1->turn_litt.root.nitrogen*frac1+tree2->turn_litt.root.nitrogen*frac2)/(frac1+frac2);
  /*tree1->turn_litt.sapwood=(tree1->turn_litt.sapwood*frac1+tree2->turn_litt.sapwood*frac2)/(frac1+frac2);*/
  //allometry_tree(pft1);
  //fpc_tree(pft1);
} /* of 'mix_veg_stock_tree' */

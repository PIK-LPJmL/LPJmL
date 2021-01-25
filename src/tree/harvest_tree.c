/**************************************************************************************/
/**                                                                                \n**/
/**               h  a  r  v  e  s  t  _  t  r  e  e  .  c                         \n**/
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

Stocks harvest_tree(Pft *pft)
{
    Stocks harvest;
    Pfttree *tree;
    Pfttreepar *treepar;
    tree=pft->data;
    treepar=pft->par->data;
    if(!strcmp(pft->par->name,"tea"))
    {
      harvest.carbon=tree->ind.leaf.carbon*treepar->harvest_ratio*pft->nind;
      harvest.nitrogen=tree->ind.leaf.nitrogen*treepar->harvest_ratio*pft->nind;
      tree->ind.leaf.carbon*=(1-treepar->harvest_ratio);
      tree->ind.leaf.nitrogen*=(1-treepar->harvest_ratio);
    }
    else
    {
      harvest=tree->fruit;
      tree->fruit.carbon=tree->fruit.nitrogen=0;
    }
    //printf("harvest(%s)=%g\n",pft->par->name,harvest.carbon);
    //pft->bm_inc.carbon-=harvest.carbon;
    //pft->bm_inc.nitrogen-=harvest.nitrogen;
    return harvest;
} /* of 'harvest_tree' */

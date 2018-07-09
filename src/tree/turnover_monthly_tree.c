/**************************************************************************************/
/**                                                                                \n**/
/**        t u r n o v e r _ m o n t h l y _ t r e e . c                           \n**/
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

void turnover_monthly_tree(Litter *litter,Pft *pft)
{
  Pfttree *tree;
  Pfttreepar *treepar;  
  treepar=pft->par->data;
  tree=pft->data;
    tree->turn.root.carbon+=tree->ind.root.carbon*treepar->turnover.root/NMONTH;
  tree->turn_litt.root.carbon+=tree->ind.root.carbon*treepar->turnover.root/NMONTH*pft->nind;
  litter->bg[pft->litter].carbon+=tree->ind.root.carbon*treepar->turnover.root/NMONTH*pft->nind;
  tree->turn.root.nitrogen+=tree->ind.root.nitrogen*treepar->turnover.root/NMONTH;
  tree->turn_litt.root.nitrogen+=tree->ind.root.nitrogen*treepar->turnover.root/NMONTH*pft->nind;//*pft->par->fn_turnover;
  litter->bg[pft->litter].nitrogen+=tree->ind.root.nitrogen*treepar->turnover.root/NMONTH*pft->nind*pft->par->fn_turnover;
  tree->turn_nbminc+= tree->ind.root.nitrogen*treepar->turnover.root/NMONTH*pft->nind*(1-pft->par->fn_turnover);
} /* of 'turnover_monthly_tree' */

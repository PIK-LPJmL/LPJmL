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

void turnover_monthly_tree(Litter *litter, /**< pointer to litter pool */
                           Pft *pft,        /**< pointer to tree PFT */
                           const Config *config /**< LPJmL configuration */
                          )
{
  Pfttree *tree;
  Pfttreepar *treepar;
  Output *output;
  treepar=pft->par->data;
  tree=pft->data;
  output=&pft->stand->cell->output;
  tree->turn.root.carbon+=tree->ind.root.carbon*treepar->turnover.root/NMONTH;
  tree->turn_litt.root.carbon+=tree->ind.root.carbon*treepar->turnover.root/NMONTH*pft->nind;
  litter->item[pft->litter].bg.carbon+=tree->ind.root.carbon*treepar->turnover.root/NMONTH*pft->nind;
  getoutput(output,LITFALLC,config)+=tree->ind.root.carbon*treepar->turnover.root/NMONTH*pft->nind*pft->stand->frac;
  tree->turn.root.nitrogen+=tree->ind.root.nitrogen*treepar->turnover.root/NMONTH;
  tree->turn_litt.root.nitrogen+=tree->ind.root.nitrogen*treepar->turnover.root/NMONTH*pft->nind;
  litter->item[pft->litter].bg.nitrogen+=tree->ind.root.nitrogen*treepar->turnover.root/NMONTH*pft->nind*pft->par->fn_turnover;
  getoutput(output,LITFALLN,config)+=tree->ind.root.nitrogen*treepar->turnover.root/NMONTH*pft->nind*pft->par->fn_turnover*pft->stand->frac;
  tree->turn_nbminc+= tree->ind.root.nitrogen*treepar->turnover.root/NMONTH*pft->nind*(1-pft->par->fn_turnover);
  pft->nbalance_cor+= tree->ind.root.nitrogen*treepar->turnover.root/NMONTH*pft->nind*(1-pft->par->fn_turnover);
} /* of 'turnover_monthly_tree' */

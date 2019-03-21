/**************************************************************************************/
/**                                                                                \n**/
/**      c  o  p  p  i  c  e  _  t  r  e  e  .  c                                  \n**/
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

#define HARVEST_EFFICIENCY 0.9
/* Sapwood reduction needs to be lower to avoid fatal numerical instabilities in allometry */
/* Makes sense as part (~30 %) of sapwood carbon is located underground */
#define HARVEST_EFFICIENCY_SAP 0.65


Stocks coppice_tree(Pft *pft /**< pointer to tree PFT */
                   )         /** \return harvest (gC/m2,gN/m2) */
{
  Pfttree *tree;
  Stocks harvest={0,0};
  Stocks leaf_old;
  tree=pft->data;

  harvest.carbon=(((tree->ind.sapwood.carbon*HARVEST_EFFICIENCY_SAP+tree->ind.heartwood.carbon*HARVEST_EFFICIENCY)-tree->ind.debt.carbon)*pft->nind);
  harvest.nitrogen=(((tree->ind.sapwood.nitrogen*HARVEST_EFFICIENCY_SAP+tree->ind.heartwood.nitrogen*HARVEST_EFFICIENCY)-tree->ind.debt.nitrogen)*pft->nind);
  leaf_old.carbon=tree->ind.leaf.carbon;
  leaf_old.nitrogen=tree->ind.leaf.nitrogen;
  tree->ind.heartwood.carbon*=1-HARVEST_EFFICIENCY;
  tree->ind.sapwood.carbon*=1-HARVEST_EFFICIENCY_SAP;
  tree->ind.debt.carbon=0;
  tree->ind.leaf.carbon=8000*tree->ind.sapwood.carbon/(wooddens*tree->height*pft->par->sla);
  harvest.carbon+=(leaf_old.carbon-tree->ind.leaf.carbon)*pft->nind;
  tree->ind.heartwood.nitrogen*=1-HARVEST_EFFICIENCY;
  tree->ind.sapwood.nitrogen*=1-HARVEST_EFFICIENCY_SAP;
  tree->ind.debt.nitrogen=0;
  tree->ind.leaf.nitrogen=8000*tree->ind.sapwood.nitrogen/(wooddens*tree->height*pft->par->sla);
  harvest.nitrogen+=(leaf_old.nitrogen-tree->ind.leaf.nitrogen)*pft->nind;
  /* Call allometry to adjust height and crownarea */
  allometry_tree(pft);
  fpc_tree(pft);

  return harvest;
} /* of 'coppice_tree' */

/* Simulate coppicing of SRWC tree
 * put all leafmass into litter
 * put 90 % of heart- and sapwood into harvest
 * fine roots remain unchanged
 * sapwood and heartwood are reduced by 90 % */

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


Real coppice_tree(Pft *pft)
{
  Pfttree *tree;
  Real harvest=0.0;
  Real leaf_old;
  tree=pft->data;

  harvest=(((tree->ind.sapwood*HARVEST_EFFICIENCY_SAP+tree->ind.heartwood*HARVEST_EFFICIENCY)-tree->ind.debt)*pft->nind);
  leaf_old=tree->ind.leaf;
  tree->ind.heartwood*=1-HARVEST_EFFICIENCY;
  tree->ind.sapwood*=1-HARVEST_EFFICIENCY_SAP;
  tree->ind.debt=0;
  tree->ind.leaf=8000*tree->ind.sapwood/(wooddens*tree->height*pft->par->sla);
  harvest+=(leaf_old-tree->ind.leaf)*pft->nind;
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

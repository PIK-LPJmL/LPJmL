/**************************************************************************************/
/**                                                                                \n**/
/**     l  i  t  t  e  r  _  u  p  d  a  t  e  _  t  r   e  e  .  c                \n**/
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

void litter_update_tree(Litter *litter, /**< Litter pool */
                        Pft *pft,       /**< PFT variables */
                        Real frac,      /**< fraction added to litter (0..1) */
                        const Config *config /**< LPJmL configuration */
                       )
{
  int i;
  const Pfttreepar *treepar;
  Output *output;
  Pfttree *tree;
  tree=pft->data;
  treepar=pft->par->data;
  output=&pft->stand->cell->output;
  tree->ind.leaf.carbon-= tree->turn.leaf.carbon;
  tree->ind.root.carbon-= tree->turn.root.carbon;
  tree->ind.leaf.nitrogen-= tree->turn.leaf.nitrogen;
  tree->ind.root.nitrogen-= tree->turn.root.nitrogen;
  litter->item[pft->litter].agtop.leaf.carbon+=tree->turn.leaf.carbon*pft->nind-tree->turn_litt.leaf.carbon;
  litter->item[pft->litter].agtop.leaf.nitrogen+=tree->turn.leaf.nitrogen*pft->nind-tree->turn_litt.leaf.nitrogen;
  update_fbd_tree(litter,pft->par->fuelbulkdensity,tree->turn.leaf.carbon*pft->nind-tree->turn_litt.leaf.carbon,0);
  litter->item[pft->litter].bg.carbon+=tree->turn.root.carbon*pft->nind-tree->turn_litt.root.carbon;
  litter->item[pft->litter].bg.nitrogen+=tree->turn.root.nitrogen*pft->nind-tree->turn_litt.root.nitrogen;
  tree->turn.root.carbon=tree->turn.leaf.carbon=tree->turn_litt.leaf.carbon=tree->turn_litt.root.carbon=0.0;
  tree->turn.root.nitrogen=tree->turn.leaf.nitrogen=tree->turn_litt.leaf.nitrogen=tree->turn_litt.root.nitrogen=0.0;

  if(pft->nind>0)
  {
    litter->item[pft->litter].agtop.leaf.carbon+=tree->fruit.carbon/pft->nind*frac;
    litter->item[pft->litter].agtop.leaf.nitrogen+=(pft->bm_inc.nitrogen+tree->fruit.nitrogen)/pft->nind*frac;
    getoutput(output,LITFALLN,config)+=(pft->bm_inc.nitrogen+tree->fruit.nitrogen)/pft->nind*frac*pft->stand->frac;
    getoutput(output,LITFALLC,config)+=tree->fruit.carbon/pft->nind*frac*pft->stand->frac;
  }
  litter->item[pft->litter].agtop.leaf.carbon+=tree->ind.leaf.carbon*frac;
  getoutput(output,LITFALLC,config)+=tree->ind.leaf.carbon*frac*pft->stand->frac;
  litter->item[pft->litter].agtop.leaf.nitrogen+=tree->ind.leaf.nitrogen*frac;
  getoutput(output,LITFALLN,config)+=tree->ind.leaf.nitrogen*frac*pft->stand->frac;
  for(i=0;i<NFUELCLASS;i++)
  {
    litter->item[pft->litter].agtop.wood[i].carbon+=(tree->ind.sapwood.carbon+tree->ind.heartwood.carbon-
                                  tree->ind.debt.carbon+tree->excess_carbon)*frac*treepar->fuelfrac[i];
    getoutput(output,LITFALLC,config)+=(tree->ind.sapwood.carbon+tree->ind.heartwood.carbon-
                                  tree->ind.debt.carbon+tree->excess_carbon)*frac*treepar->fuelfrac[i]*pft->stand->frac;
    getoutput(output,LITFALLC_WOOD,config)+=(tree->ind.sapwood.carbon+tree->ind.heartwood.carbon-
                                  tree->ind.debt.carbon+tree->excess_carbon)*frac*treepar->fuelfrac[i]*pft->stand->frac;
    litter->item[pft->litter].agtop.wood[i].nitrogen+=(tree->ind.sapwood.nitrogen+tree->ind.heartwood.nitrogen-
                                  tree->ind.debt.nitrogen)*frac*treepar->fuelfrac[i];
    getoutput(output,LITFALLN,config)+=(tree->ind.sapwood.nitrogen+tree->ind.heartwood.nitrogen-
                                  tree->ind.debt.nitrogen)*frac*treepar->fuelfrac[i]*pft->stand->frac;
    getoutput(output,LITFALLN_WOOD,config)+=(tree->ind.sapwood.nitrogen+tree->ind.heartwood.nitrogen-
                                  tree->ind.debt.nitrogen)*frac*treepar->fuelfrac[i]*pft->stand->frac;
    update_fbd_tree(litter,pft->par->fuelbulkdensity,
                    (tree->ind.sapwood.carbon+tree->ind.heartwood.carbon-tree->ind.debt.carbon)
               *frac*treepar->fuelfrac[i],i);
  }
  litter->item[pft->litter].bg.carbon+=tree->ind.root.carbon*frac;
  getoutput(output,LITFALLC,config)+=tree->ind.root.carbon*frac*pft->stand->frac;
  litter->item[pft->litter].bg.nitrogen+=tree->ind.root.nitrogen*frac;
  getoutput(output,LITFALLN,config)+=tree->ind.root.nitrogen*frac*pft->stand->frac;
  update_fbd_tree(&pft->stand->soil.litter,pft->par->fuelbulkdensity,tree->ind.leaf.carbon*treepar->turnover.leaf*frac,0); //CHECK

} /* of 'litter_update_tree' */

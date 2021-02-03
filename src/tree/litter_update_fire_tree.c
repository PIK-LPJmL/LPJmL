/**************************************************************************************/
/**                                                                                \n**/
/**  l  i  t  t  e  r  _  u  p  d  a  t  e  _  f  i  r  e  _  t  r   e  e  .  c    \n**/
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

/*function called every day when there is fire, if litter_update_tree is called every day, then a numerical bug leads to highly overestimated vegc in some cells */

void litter_update_fire_tree(Litter *litter, /**< Litter pool */
                        Pft *pft,            /**< PFT variables */
                        Real frac            /**< fraction added to litter (0..1) */
                       )
{
  int i;
  const Pfttreepar *treepar;
  Pfttree *tree;
  Output *output;
  tree=pft->data;
  treepar=pft->par->data;
  output=&pft->stand->cell->output; 
  litter->item[pft->litter].ag.leaf.carbon+=tree->ind.leaf.carbon*frac;
  output->alittfall.carbon+=tree->ind.leaf.carbon*frac*pft->stand->frac;
  litter->item[pft->litter].ag.leaf.nitrogen+=tree->ind.leaf.nitrogen*frac;
  output->alittfall.nitrogen+=tree->ind.leaf.nitrogen*frac*pft->stand->frac;
  if(pft->nind>0)
  {
    litter->item[pft->litter].ag.leaf.nitrogen+=(pft->bm_inc.nitrogen+tree->fruit.nitrogen)/pft->nind*frac;
    litter->item[pft->litter].ag.leaf.carbon+=tree->fruit.carbon/pft->nind*frac;
    output->alittfall.nitrogen+=(pft->bm_inc.nitrogen+tree->fruit.nitrogen)/pft->nind*frac*pft->stand->frac;
    output->alittfall.carbon+=tree->fruit.carbon/pft->nind*frac*pft->stand->frac;
    pft->bm_inc.nitrogen*=(pft->nind-frac)/pft->nind;
    tree->fruit.carbon*=(pft->nind-frac)/pft->nind;
    tree->fruit.nitrogen*=(pft->nind-frac)/pft->nind;
  }
  for(i=0;i<NFUELCLASS;i++)
  {
    litter->item[pft->litter].ag.wood[i].carbon+=(tree->ind.sapwood.carbon+tree->ind.heartwood.carbon-
                                  tree->ind.debt.carbon+tree->excess_carbon)*frac*treepar->fuelfrac[i];
    output->alittfall.carbon+=(tree->ind.sapwood.carbon+tree->ind.heartwood.carbon-
                                  tree->ind.debt.carbon+tree->excess_carbon)*frac*treepar->fuelfrac[i]*pft->stand->frac;
    litter->item[pft->litter].ag.wood[i].nitrogen+=(tree->ind.sapwood.nitrogen+tree->ind.heartwood.nitrogen-
                                  tree->ind.debt.nitrogen)*frac*treepar->fuelfrac[i];
    output->alittfall.nitrogen+=(tree->ind.sapwood.nitrogen+tree->ind.heartwood.nitrogen-
                                  tree->ind.debt.nitrogen)*frac*treepar->fuelfrac[i]*pft->stand->frac;
    update_fbd_tree(litter,pft->par->fuelbulkdensity,
                    (tree->ind.sapwood.carbon+tree->ind.heartwood.carbon-tree->ind.debt.carbon)
               *frac*treepar->fuelfrac[i],i);
  }
  litter->item[pft->litter].bg.carbon+=tree->ind.root.carbon*frac;
  output->alittfall.carbon+=tree->ind.root.carbon*frac*pft->stand->frac;
  litter->item[pft->litter].bg.nitrogen+=tree->ind.root.nitrogen*frac;
  output->alittfall.nitrogen+=tree->ind.root.nitrogen*frac*pft->stand->frac;
  update_fbd_tree(&pft->stand->soil.litter,pft->par->fuelbulkdensity,tree->ind.leaf.carbon*treepar->turnover.leaf*frac,0); //CHECK

} /* of 'litter_update_fire_tree' */

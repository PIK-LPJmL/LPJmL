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
  tree=pft->data;
  treepar=pft->par->data;
  
  litter->ag[pft->litter].trait.leaf.carbon+=tree->ind.leaf.carbon*frac;
  litter->ag[pft->litter].trait.leaf.nitrogen+=tree->ind.leaf.nitrogen*frac;
  if(pft->nind>0)
  {
    litter->ag[pft->litter].trait.leaf.nitrogen+=pft->bm_inc.nitrogen/pft->nind*frac;
    pft->bm_inc.nitrogen*=(pft->nind-frac)/pft->nind;
  }
  for(i=0;i<NFUELCLASS;i++)
  {
    litter->ag[pft->litter].trait.wood[i].carbon+=(tree->ind.sapwood.carbon+tree->ind.heartwood.carbon-
                                  tree->ind.debt.carbon+tree->excess_carbon)*frac*treepar->fuelfrac[i];
    litter->ag[pft->litter].trait.wood[i].nitrogen+=(tree->ind.sapwood.nitrogen+tree->ind.heartwood.nitrogen-
                                  tree->ind.debt.nitrogen)*frac*treepar->fuelfrac[i];
    update_fbd_tree(litter,pft->par->fuelbulkdensity,
                    (tree->ind.sapwood.carbon+tree->ind.heartwood.carbon-tree->ind.debt.carbon)
               *frac*treepar->fuelfrac[i],i);
  }
  litter->bg[pft->litter].carbon+=tree->ind.root.carbon*frac;
  litter->bg[pft->litter].nitrogen+=tree->ind.root.nitrogen*frac;
  update_fbd_tree(&pft->stand->soil.litter,pft->par->fuelbulkdensity,tree->ind.leaf.carbon*treepar->turnover.leaf*frac,0); //CHECK

} /* of 'litter_update_fire_tree' */

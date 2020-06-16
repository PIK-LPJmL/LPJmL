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
  
  litter->ag[pft->litter].trait.leaf+=tree->ind.leaf*frac;
  for(i=0;i<NFUELCLASS;i++)
  {
    litter->ag[pft->litter].trait.wood[i]+=(tree->ind.sapwood+tree->ind.heartwood-
                                  tree->ind.debt)*frac*treepar->fuelfrac[i];
    update_fbd_tree(litter,pft->par->fuelbulkdensity,
                    (tree->ind.sapwood+tree->ind.heartwood-tree->ind.debt)
               *frac*treepar->fuelfrac[i],i);
  }
  litter->bg[pft->litter]+=tree->ind.root*frac;
  update_fbd_tree(&pft->stand->soil.litter,pft->par->fuelbulkdensity,tree->ind.leaf*treepar->turnover.leaf*frac,0); //CHECK

} /* of 'litter_update_fire_tree' */

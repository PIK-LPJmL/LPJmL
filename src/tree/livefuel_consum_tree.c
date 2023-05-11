/**************************************************************************************/
/**                                                                                \n**/
/**               l i v e f u e l _ c o n s u m _ t r e e . c                      \n**/
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

Stocks livefuel_consum_tree(Litter *litter,
                            Pft *pft,
                            const Fuel *fuel,
                            Livefuel *livefuel,
                            Bool *isdead,
                            Real surface_fi,
                            Real fire_frac,
                            const Config *config /**< LPJmL configuration */
                           )
{
  Stocks live_consum_tree;
  Pfttree *tree;
  Pfttreepar *treepar;
  Stocks sapwood_consum,heartwood_consum;
  Real fire_nind_kill;
  tree=pft->data;
  treepar=pft->par->data;
 
 
  fire_nind_kill=firemortality_tree(pft,fuel,livefuel,surface_fi,fire_frac);
 
  /* tree biomass consumption from postfire mortality*/
  /*      5% of 1000hr fuel involved in crown kill */
  sapwood_consum.carbon = tree->ind.sapwood.carbon *(treepar->fuelfrac[0] + treepar->fuelfrac[1]  +
                               treepar->fuelfrac[2] * 0.05);
  sapwood_consum.nitrogen = tree->ind.sapwood.nitrogen *(treepar->fuelfrac[0] + treepar->fuelfrac[1]  +
                               treepar->fuelfrac[2] * 0.05);
  heartwood_consum.carbon = tree->ind.heartwood.carbon *(treepar->fuelfrac[0] + treepar->fuelfrac[1] +
                                         treepar->fuelfrac[2] * 0.05);
  heartwood_consum.nitrogen = tree->ind.heartwood.nitrogen *(treepar->fuelfrac[0] + treepar->fuelfrac[1] +
                                         treepar->fuelfrac[2] * 0.05);
  live_consum_tree.carbon = livefuel->disturb * (tree->ind.leaf.carbon + sapwood_consum.carbon + heartwood_consum.carbon)*pft->nind;  /*gC/m2*/
  live_consum_tree.nitrogen = livefuel->disturb * (tree->ind.leaf.nitrogen + sapwood_consum.nitrogen + heartwood_consum.nitrogen)*pft->nind;  /*gC/m2*/
  live_consum_tree.carbon+=pft->bm_inc.carbon*min(1,livefuel->disturb);
  live_consum_tree.nitrogen+=pft->bm_inc.nitrogen*min(1,livefuel->disturb);
  pft->bm_inc.carbon*=(1-min(1,livefuel->disturb));
  pft->bm_inc.nitrogen*=(1-min(1,livefuel->disturb));
  tree->ind.leaf.carbon *= (1-livefuel->disturb);
  tree->ind.sapwood.carbon -= livefuel->disturb*sapwood_consum.carbon;
  tree->ind.heartwood.carbon -= livefuel->disturb*heartwood_consum.carbon;
  tree->ind.leaf.nitrogen *= (1-livefuel->disturb);
  tree->ind.sapwood.nitrogen -= livefuel->disturb*sapwood_consum.nitrogen;
  tree->ind.heartwood.nitrogen -= livefuel->disturb*heartwood_consum.nitrogen;
  if(fabs(pft->bm_inc.carbon)>epsilon)
  { 
    litter->item[pft->litter].agtop.leaf.carbon+=pft->bm_inc.carbon*fire_nind_kill/pft->nind;
    update_fbd_tree(litter,pft->par->fuelbulkdensity,pft->bm_inc.carbon*fire_nind_kill/pft->nind,0);
    pft->bm_inc.carbon*=(pft->nind-fire_nind_kill)/pft->nind;
  }
  litter_update_fire_tree(litter,pft,fire_nind_kill,config);
  pft->nind-=fire_nind_kill;
  if (fire_nind_kill > 0 && pft->nind < epsilon)
  {
    if(pft->bm_inc.carbon>0)
    {
      litter->item[pft->litter].agtop.wood[0].carbon+=pft->bm_inc.carbon;
      update_fbd_tree(litter,pft->par->fuelbulkdensity,pft->bm_inc.carbon,0);
    }
    else
      live_consum_tree.carbon+=pft->bm_inc.carbon;
    *isdead=TRUE;
    if(pft->bm_inc.nitrogen>0)
    {
      litter->item[pft->litter].agtop.wood[0].nitrogen+=pft->bm_inc.nitrogen;
    }
    else
      live_consum_tree.nitrogen+=pft->bm_inc.nitrogen;
    pft->bm_inc.nitrogen=0;
    litter_update_tree(litter,pft,pft->nind,config);
  }
  else
    *isdead=FALSE;
  return live_consum_tree;
} /* of 'livefuel_consum_tree' */

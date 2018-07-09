/**************************************************************************************/
/**                                                                                \n**/
/**                  t  u  r  n  o  v  e  r  _  t  r  e  e  .  c                   \n**/
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

/*
 *
 *  Function reproduction + turnover
 *
 *  Turnover of PFT-specific fraction from each living C pool
 *  Leaf and root C transferred to litter, sapwood C to heartwood
 *
 */

#define CDEBT_PAYBACK_RATE 0.2

Stocks turnover_tree(Litter *litter, /**< Litter pool */
                     Pft *pft        /**< Pointer to PFT */
                    )                /** \return turnover (gC/m2) */
{
  Pfttree *tree;
  const Pfttreepar *treepar;
  Stocks sum={0,0};
  Treephys turn;
  Real reprod,cmass_excess,payback;
  tree=pft->data;
  treepar=getpftpar(pft,data);
  if(pft->nind<epsilon)
    return sum;
  cmass_excess=0;
  /* reproduction */
  if(pft->bm_inc.carbon>=0)
  {
    reprod=pft->bm_inc.carbon*treepar->reprod_cost;
    litter->ag[pft->litter].trait.leaf.carbon+=reprod;
    update_fbd_tree(litter,pft->par->fuelbulkdensity,reprod,0);
    pft->bm_inc.carbon-=reprod;
    reprod=pft->bm_inc.nitrogen*treepar->reprod_cost;
    litter->ag[pft->litter].trait.leaf.nitrogen+=reprod;
    pft->bm_inc.nitrogen-=reprod;
    if(israingreen(pft))
    {
      /* TODO what to do about N here? */
      //cmass_excess=tree->aphen_raingreen/(pft->par->longevity*NDAYYEAR)*pft->bm_inc-pft->bm_inc;
      if (cmass_excess>0.0)
      {
        if (cmass_excess>pft->bm_inc.carbon)
          cmass_excess=pft->bm_inc.carbon;
        litter->ag[pft->litter].trait.leaf.carbon+=cmass_excess;
        update_fbd_tree(litter,pft->par->fuelbulkdensity,cmass_excess,0);
        pft->bm_inc.carbon-=cmass_excess;
      }
    }
    payback=tree->ind.debt.carbon*pft->nind*CDEBT_PAYBACK_RATE>pft->bm_inc.carbon ? pft->bm_inc.carbon : tree->ind.debt.carbon*pft->nind*CDEBT_PAYBACK_RATE;
    pft->bm_inc.carbon-=payback;
    if(pft->nind>0)
      tree->ind.debt.carbon-=payback/pft->nind;
  }
  /* turnover */
  turn.root=tree->turn.root;
  turn.leaf=tree->turn.leaf;
  /* update litter pools to prevent carbon balance error if pft->nind has been changed */
  litter->ag[pft->litter].trait.leaf.carbon+=turn.leaf.carbon*pft->nind-tree->turn_litt.leaf.carbon;
  litter->ag[pft->litter].trait.leaf.nitrogen+=turn.leaf.nitrogen*pft->nind-tree->turn_litt.leaf.nitrogen;
  update_fbd_tree(litter,pft->par->fuelbulkdensity,turn.leaf.carbon*pft->nind-tree->turn_litt.leaf.carbon,0);
  litter->bg[pft->litter].carbon+=turn.root.carbon*pft->nind-tree->turn_litt.root.carbon;
  litter->bg[pft->litter].nitrogen+=turn.root.nitrogen*pft->nind-tree->turn_litt.root.nitrogen;
  tree->turn.root.carbon=tree->turn.leaf.carbon=tree->turn_litt.leaf.carbon=tree->turn_litt.root.carbon=0.0;
  tree->turn.root.nitrogen=tree->turn.leaf.nitrogen=tree->turn_litt.leaf.nitrogen=tree->turn_litt.root.nitrogen=0.0;
  turn.sapwood.carbon=tree->ind.sapwood.carbon*treepar->turnover.sapwood;
  tree->ind.leaf.carbon-= turn.leaf.carbon;
  tree->ind.sapwood.carbon-= turn.sapwood.carbon;
  tree->ind.root.carbon-= turn.root.carbon;
  tree->ind.heartwood.carbon+=turn.sapwood.carbon;
  if (tree->ind.leaf.carbon<epsilon)
  {
    litter->ag[pft->litter].trait.leaf.carbon+=tree->ind.leaf.carbon*pft->nind;
    update_fbd_tree(litter,pft->par->fuelbulkdensity,tree->ind.leaf.carbon*pft->nind,0);
    tree->ind.leaf.carbon=0.0;
  }
  if (tree->ind.root.carbon<epsilon)
  {
    litter->bg[pft->litter].carbon+=tree->ind.root.carbon*pft->nind;
    tree->ind.root.carbon=0.0;
  }
  turn.sapwood.nitrogen=tree->ind.sapwood.nitrogen*treepar->turnover.sapwood*(1-param.sapwood_recovery);   /*0.7 is a bit arbitrary (check for literature values), but not all nitrogen of the sapwood should go to heartwood*/
  tree->ind.leaf.nitrogen-= turn.leaf.nitrogen;
  tree->ind.sapwood.nitrogen-= turn.sapwood.nitrogen;
  tree->ind.root.nitrogen-= turn.root.nitrogen;
  pft->bm_inc.nitrogen+=tree->turn_nbminc;
  tree->turn_nbminc=0;
  tree->ind.heartwood.nitrogen+=turn.sapwood.nitrogen;
  if (tree->ind.leaf.nitrogen<epsilon)
  {
    litter->ag[pft->litter].trait.leaf.nitrogen+=tree->ind.leaf.nitrogen*pft->nind;
    tree->ind.leaf.nitrogen=0.0;
  }
  if (tree->ind.root.nitrogen<epsilon)
  {
    litter->bg[pft->litter].nitrogen+=tree->ind.root.nitrogen*pft->nind;
    tree->ind.root.nitrogen=0.0;
  }

#ifdef DEBUG2
  printf("turn: %s %g %g %g %g %g %g %g %g %g %g\n",pft->par->name,tree->ind.leaf,tree->ind.root,
         tree->ind.sapwood,tree->ind.heartwood,tree->ind.debt,pft->fpc,
         pft->nind,tree->height,tree->crownarea,pft->bm_inc);
#endif
  
  sum.carbon=turn.leaf.carbon+turn.sapwood.carbon+turn.root.carbon;
  sum.nitrogen=turn.leaf.nitrogen+turn.sapwood.nitrogen+turn.root.nitrogen;
  return sum;
} /* of 'turnover_tree' */

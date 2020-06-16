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

Real turnover_tree(Litter *litter, /**< Litter pool */
                   Pft *pft        /**< Pointer to PFT */
                  )                /** \return turnover (gC/m2) */
{
  Pfttree *tree;
  const Pfttreepar *treepar;
  Treephys turn;
  Real reprod,cmass_excess,payback;
  tree=pft->data;
  treepar=getpftpar(pft,data);
  if(pft->nind<epsilon)
    return 0;
  /* reproduction */
  if(pft->bm_inc>=0)
  {
    reprod=pft->bm_inc*treepar->reprod_cost;
    litter->ag[pft->litter].trait.leaf+=reprod;
    update_fbd_tree(litter,pft->par->fuelbulkdensity,reprod,0);
    pft->bm_inc-=reprod;
    if(israingreen(pft))
    {
      cmass_excess=tree->aphen_raingreen/(pft->par->longevity*NDAYYEAR)*pft->bm_inc-pft->bm_inc;
                 /* (tree->ind.leaf+tree->ind.root)*pft->nind-(tree->ind.leaf+tree->ind.root)*pft->nind;*/
      if (cmass_excess>0.0)
      {
        if (cmass_excess>pft->bm_inc)
          cmass_excess=pft->bm_inc;
        litter->ag[pft->litter].trait.leaf+=cmass_excess;
        update_fbd_tree(litter,pft->par->fuelbulkdensity,cmass_excess,0);
        pft->bm_inc-=cmass_excess;
      }
    }
    payback=tree->ind.debt*pft->nind*CDEBT_PAYBACK_RATE>pft->bm_inc ? pft->bm_inc : tree->ind.debt*pft->nind*CDEBT_PAYBACK_RATE;
    pft->bm_inc-=payback;
    if(pft->nind>0)
      tree->ind.debt-=payback/pft->nind;
  }
  /* turnover */
  turn.root=tree->turn.root;
  turn.leaf=tree->turn.leaf;
  /* update litter pools to prevent carbon balance error if pft->nind has been changed */
  litter->ag[pft->litter].trait.leaf+=turn.leaf*pft->nind-tree->turn_litt.leaf;
  update_fbd_tree(litter,pft->par->fuelbulkdensity,turn.leaf*pft->nind-tree->turn_litt.leaf,0);
  litter->bg[pft->litter]+=turn.root*pft->nind-tree->turn_litt.root;
  tree->turn.root=tree->turn.leaf=tree->turn_litt.leaf=tree->turn_litt.root=0.0;
  turn.sapwood=tree->ind.sapwood*treepar->turnover.sapwood;
  tree->ind.leaf-= turn.leaf;
  tree->ind.sapwood-= turn.sapwood;
  tree->ind.root-= turn.root;
  tree->ind.heartwood+=turn.sapwood;
  if (tree->ind.leaf<epsilon)
  {
    litter->ag[pft->litter].trait.leaf+=tree->ind.leaf*pft->nind;
    update_fbd_tree(litter,pft->par->fuelbulkdensity,tree->ind.leaf*pft->nind,0);
    tree->ind.leaf=0.0;
  }
  if (tree->ind.root<epsilon)
  {
    litter->bg[pft->litter]+=tree->ind.root*pft->nind;
    tree->ind.root=0.0;
  }

#ifdef DEBUG2
  printf("turn: %s %g %g %g %g %g %g %g %g %g %g\n",pft->par->name,tree->ind.leaf,tree->ind.root,
         tree->ind.sapwood,tree->ind.heartwood,tree->ind.debt,pft->fpc,
         pft->nind,tree->height,tree->crownarea,pft->bm_inc);
#endif
  return turn.leaf+turn.sapwood+turn.root;
} /* of 'turnover_tree' */

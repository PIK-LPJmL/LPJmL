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
                     Pft *pft,       /**< Pointer to PFT */
                     const Config *config /**< LPJmL configuration */
                    )                /** \return turnover (gC/m2,gN/m2) */
{
  Pfttree *tree;
  const Pfttreepar *treepar;
  Stocks sum={0,0};
  Treephys turn;
  Output *output;
  Real reprod,cmass_excess,payback;
#ifdef CHECK_BALANCE
  Stocks sum_before,sum_after;
  Stocks turn_diff={0,0};
#endif
  tree=pft->data;
  treepar=getpftpar(pft,data);
  if(pft->nind<epsilon)
    return sum;
  output=&pft->stand->cell->output;
#ifdef CHECK_BALANCE
  sum_before=litterstocks(litter);
  sum_before.carbon+=vegc_sum_tree(pft)+pft->bm_inc.carbon-pft->establish.carbon;
  sum_before.nitrogen+=vegn_sum_tree(pft)+pft->bm_inc.nitrogen-pft->establish.nitrogen;
  turn_diff.nitrogen=0;
#endif
  cmass_excess=0;
  /* reproduction */
  if(pft->bm_inc.carbon>=0)
  {
    reprod=pft->bm_inc.carbon*treepar->reprod_cost;
    pft->bm_inc.carbon-=reprod;
    if(pft->establish.carbon<reprod)
    {
      reprod-=pft->establish.carbon;
      getoutput(output,FLUX_ESTABC,config)-=pft->establish.carbon*pft->stand->frac;
      pft->stand->cell->balance.flux_estab.carbon-=pft->establish.carbon*pft->stand->frac;
      pft->establish.carbon=0;
    }
    else
    {
      getoutput(output,FLUX_ESTABC,config)-=reprod*pft->stand->frac;
      pft->stand->cell->balance.flux_estab.carbon-=reprod*pft->stand->frac;
      pft->establish.carbon-=reprod;
      reprod=0;
    }
    litter->item[pft->litter].agtop.leaf.carbon+=reprod;
    getoutput(output,LITFALLC,config)+=reprod*pft->stand->frac;
    update_fbd_tree(litter,pft->par->fuelbulkdensity,reprod,0);

    reprod=pft->bm_inc.nitrogen*treepar->reprod_cost;
    pft->bm_inc.nitrogen-=reprod;
    if(pft->establish.nitrogen<reprod)
    {
      reprod-=pft->establish.nitrogen;
      getoutput(output,FLUX_ESTABN,config)-=pft->establish.nitrogen*pft->stand->frac;
      if(pft->stand->type->landusetype!=NATURAL && pft->stand->type->landusetype!=WOODPLANTATION)
        getoutput(output,FLUX_ESTABN_MG,config)-=pft->establish.nitrogen*pft->stand->frac;
      pft->stand->cell->balance.flux_estab.nitrogen-=pft->establish.nitrogen*pft->stand->frac;
      pft->establish.nitrogen=0;
    }
    else
    {
      getoutput(output,FLUX_ESTABN,config)-=reprod*pft->stand->frac;
      pft->stand->cell->balance.flux_estab.nitrogen-=reprod*pft->stand->frac;
      pft->establish.nitrogen-=reprod;
      reprod=0;
    }
    getoutput(output,LITFALLN,config)+=reprod*pft->stand->frac;
    litter->item[pft->litter].agtop.leaf.nitrogen+=reprod;
//TODO with the Methane Version I removed this because it is for competition not longer needed but had already no effect in master
#if 0
    if(israingreen(pft))
    {
      /* TODO what to do about N here? */
      //cmass_excess=tree->aphen_raingreen/(pft->par->longevity*NDAYYEAR)*pft->bm_inc-pft->bm_inc;
      if (cmass_excess>0.0)
      {
        if (cmass_excess>pft->bm_inc.carbon)
          cmass_excess=pft->bm_inc.carbon;
        litter->item[pft->litter].agtop.leaf.carbon+=cmass_excess;
        getoutput(output,LITFALLC,config)+=cmass_excess*pft->stand->frac;
        update_fbd_tree(litter,pft->par->fuelbulkdensity,cmass_excess,0);
        pft->bm_inc.carbon-=cmass_excess;
      }
    }
#endif
    payback=tree->ind.debt.carbon*pft->nind*CDEBT_PAYBACK_RATE>pft->bm_inc.carbon ? pft->bm_inc.carbon : tree->ind.debt.carbon*pft->nind*CDEBT_PAYBACK_RATE;
    if(pft->nind>0)
    {
      tree->ind.debt.carbon-=payback/pft->nind;
      pft->bm_inc.carbon-=payback;
    }
  }
  /* turnover */
  turn.root=tree->turn.root;
  turn.leaf=tree->turn.leaf;
  /* update litter pools to prevent carbon balance error if pft->nind has been changed */
#ifdef CHECK_BALANCE
  turn_diff.carbon+=(turn.leaf.carbon*pft->nind-tree->turn_litt.leaf.carbon)+(turn.root.carbon*pft->nind-tree->turn_litt.root.carbon);
  turn_diff.nitrogen+=(turn.leaf.nitrogen*pft->nind-tree->turn_litt.leaf.nitrogen)+(turn.root.nitrogen*pft->nind-tree->turn_litt.root.nitrogen);
#endif
  litter->item[pft->litter].agtop.leaf.carbon+=turn.leaf.carbon*pft->nind-tree->turn_litt.leaf.carbon;
  litter->item[pft->litter].agtop.leaf.nitrogen+=turn.leaf.nitrogen*pft->nind-tree->turn_litt.leaf.nitrogen;
  update_fbd_tree(litter,pft->par->fuelbulkdensity,turn.leaf.carbon*pft->nind-tree->turn_litt.leaf.carbon,0);
  litter->item[pft->litter].bg.carbon+=turn.root.carbon*pft->nind-tree->turn_litt.root.carbon;
  litter->item[pft->litter].bg.nitrogen+=turn.root.nitrogen*pft->nind-tree->turn_litt.root.nitrogen;
  /* turnover of excess carbon as root exudates */
  litter->item[pft->litter].bg.carbon+=tree->excess_carbon*pft->nind*treepar->turnover.root;
  getoutput(output,LITFALLC,config)+=tree->excess_carbon*pft->nind*treepar->turnover.root*pft->stand->frac;
  tree->excess_carbon-=tree->excess_carbon*treepar->turnover.root;
  tree->turn.root.carbon=tree->turn.leaf.carbon=tree->turn_litt.leaf.carbon=tree->turn_litt.root.carbon=0.0;
  tree->turn.root.nitrogen=tree->turn.leaf.nitrogen=tree->turn_litt.leaf.nitrogen=tree->turn_litt.root.nitrogen=0.0;
  turn.sapwood.carbon=tree->ind.sapwood.carbon*treepar->turnover.sapwood;
  tree->ind.leaf.carbon-= turn.leaf.carbon;
  tree->ind.sapwood.carbon-= turn.sapwood.carbon;
  tree->ind.root.carbon-= turn.root.carbon;
  tree->ind.heartwood.carbon+=turn.sapwood.carbon;
  if (tree->ind.leaf.carbon<epsilon)
  {
    litter->item[pft->litter].agtop.leaf.carbon+=tree->ind.leaf.carbon*pft->nind;
    getoutput(output,LITFALLC,config)+=tree->ind.leaf.carbon*pft->nind*pft->stand->frac;
    update_fbd_tree(litter,pft->par->fuelbulkdensity,tree->ind.leaf.carbon*pft->nind,0);
    tree->ind.leaf.carbon=0.0;
  }
  if (tree->ind.root.carbon<epsilon)
  {
    litter->item[pft->litter].bg.carbon+=tree->ind.root.carbon*pft->nind;
    getoutput(output,LITFALLC,config)+=tree->ind.root.carbon*pft->nind*pft->stand->frac;
    tree->ind.root.carbon=0.0;
  }
  turn.sapwood.nitrogen=tree->ind.sapwood.nitrogen*treepar->turnover.sapwood*(1-param.sapwood_recovery);   /*0.7 is a bit arbitrary (check for literature values), but not all nitrogen of the sapwood should go to heartwood*/
  tree->ind.leaf.nitrogen-= turn.leaf.nitrogen;
  tree->ind.sapwood.nitrogen-= turn.sapwood.nitrogen;
  tree->ind.root.nitrogen-= turn.root.nitrogen;
  tree->ind.heartwood.nitrogen+=turn.sapwood.nitrogen;
  if (tree->ind.leaf.nitrogen<epsilon)
  {
    litter->item[pft->litter].agtop.leaf.nitrogen+=tree->ind.leaf.nitrogen*pft->nind;
    getoutput(output,LITFALLN,config)+=tree->ind.leaf.nitrogen*pft->nind*pft->stand->frac;
    tree->ind.leaf.nitrogen=0.0;
  }
  if (tree->ind.root.nitrogen<epsilon)
  {
    litter->item[pft->litter].bg.nitrogen+=tree->ind.root.nitrogen*pft->nind;
    getoutput(output,LITFALLN,config)+=tree->ind.root.nitrogen*pft->nind*pft->stand->frac;
    tree->ind.root.nitrogen=0.0;
  }

#ifdef DEBUG3
  printf("turn: %s %g %g %g %g %g %g %g %g %g %g\n",pft->par->name,tree->ind.leaf.carbon,tree->ind.root.carbon,
         tree->ind.sapwood.carbon,tree->ind.heartwood.carbon,tree->ind.debt.carbon,pft->fpc,
         pft->nind,tree->height,tree->crownarea,pft->bm_inc.carbon);
#endif
  
  sum.carbon=turn.leaf.carbon+turn.sapwood.carbon+turn.root.carbon;
  sum.nitrogen=turn.leaf.nitrogen+turn.sapwood.nitrogen+turn.root.nitrogen;
#ifdef CHECK_BALANCE
  sum_after=litterstocks(litter);
  sum_after.carbon+=vegc_sum_tree(pft)+pft->bm_inc.carbon-pft->establish.carbon;
  sum_after.nitrogen+=vegn_sum_tree(pft)+pft->bm_inc.nitrogen-pft->establish.nitrogen;
  if(fabs(sum_after.carbon-sum_before.carbon)>0.001)
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,TRUE,"Invalid carbon balance in %s %g!=%g, bm_inc.carbon=%g for %s pft_establish : %g",
         __FUNCTION__,sum_after.carbon,sum_before.carbon,pft->bm_inc.carbon,pft->par->name,pft->establish.carbon);
  if(fabs(sum_after.nitrogen-sum_before.nitrogen)>0.001)
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,TRUE,"Invalid nitrogen balance in %s %g!=%g, turn_diff=%g for %s",
         __FUNCTION__,sum_after.nitrogen,sum_before.nitrogen,turn_diff.nitrogen,pft->par->name);
#endif
  return sum;
} /* of 'turnover_tree' */

/**************************************************************************************/
/**                                                                                \n**/
/**      e  s  t  a  b  l  i  s  h  m  e  n  t  _  t  r  e  e  .  c                \n**/
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

Stocks establishment_tree(Pft *pft,               /**< pointer to tree PFT */
                          Real UNUSED(fpc_total), /**< total sum of FPCs */
                          Real fpc_type,          /**< FPC of tree PFTs */
                          int n_est               /**< number of established tree PFTs */
                         ) /** \return establishment flux (gC/m2, gN/m2) */
{

  Real nind_old;
  Stocks flux_est={0,0};
  Real est_pft;
  /* establishment rate for a particular PFT on modelled area
   * basis (for trees, indiv/m2; for grasses, fraction of
   * modelled area colonised establishment rate for a particular
   * PFT on modelled area basis (for trees, indiv/m2; for
   * grasses, fraction of modelled area colonised)
   */
  Real est_nind=0;
  Pfttree *tree;
  Pfttreepar *treepar;
  tree=pft->data;
  treepar=getpftpar(pft,data);
  if (fpc_type>=FPC_TREE_MAX || n_est<=epsilon)
  {
    allometry_tree(pft);
    return flux_est;
  }
  if (pft->par->cultivation_type== BIOMASS)
    est_nind=treepar->k_est-pft->nind;
  else
    est_nind=treepar->k_est;
  est_pft=est_nind*(1.0-exp(-5.0*(1.0-fpc_type)))*(1.0-fpc_type)/(Real)n_est;
#ifdef SAFE
  if (est_pft<0.0)
    fail(NEGATIVE_ESTABLISHMENT_ERR,TRUE,
         "establishment_area: negative establishment rate=%g, fpc_type=%g",est_pft,fpc_type);
#endif

  nind_old=pft->nind;
  if (pft->nind<epsilon && vegc_sum(pft)<2.0) /* avoid C-balance error by checking vegc */
    pft->nind=0.0;
  pft->nind+=est_pft;
  /* Account for flux from the atmosphere to new saplings */
  flux_est.carbon=phys_sum_tree(treepar->sapl)*est_pft;
  flux_est.nitrogen=phys_sum_tree_n(treepar->sapl)*est_pft;

  /* Adjust average individual C biomass based on average biomass and density of the new saplings*/
  tree->ind.leaf.carbon=(tree->ind.leaf.carbon*nind_old+treepar->sapl.leaf.carbon*est_pft)/pft->nind;
  tree->ind.root.carbon=(tree->ind.root.carbon*nind_old+treepar->sapl.root.carbon*est_pft)/pft->nind;
  tree->ind.sapwood.carbon=(tree->ind.sapwood.carbon*nind_old+treepar->sapl.sapwood.carbon*est_pft)/pft->nind;
  tree->ind.heartwood.carbon=(tree->ind.heartwood.carbon*nind_old+treepar->sapl.heartwood.carbon*est_pft)/pft->nind;
  tree->ind.debt.carbon=tree->ind.debt.carbon*nind_old/pft->nind;
  tree->excess_carbon*=nind_old/pft->nind;
#if 0
  if((pft->par->phenology==SUMMERGREEN || pft->par->phenology==RAINGREEN) && nind_old==0)
  {
    tree->turn.leaf.carbon+=treepar->sapl.leaf.carbon*treepar->turnover.leaf;
    tree->turn_litt.leaf.carbon+=treepar->sapl.leaf.carbon*treepar->turnover.leaf*pft->nind;
    tree->turn.leaf.nitrogen+=treepar->sapl.leaf.nitrogen*treepar->turnover.leaf;
    tree->turn_litt.leaf.nitrogen+=treepar->sapl.leaf.nitrogen*treepar->turnover.leaf*pft->nind;
    pft->stand->soil.litter.ag[pft->litter].trait.leaf.carbon+=treepar->sapl.leaf.carbon*treepar->turnover.leaf*pft->nind;
    update_fbd_tree(&pft->stand->soil.litter,pft->par->fuelbulkdensity,(treepar->sapl.leaf.carbon*treepar->turnover.leaf*pft->nind),0); //have - before treepar->sapl.leaf deleted
  }
#endif
  /* do the same for N */
  tree->ind.leaf.nitrogen=(tree->ind.leaf.nitrogen*nind_old+treepar->sapl.leaf.nitrogen*est_pft)/pft->nind;
  tree->ind.root.nitrogen=(tree->ind.root.nitrogen*nind_old+treepar->sapl.root.nitrogen*est_pft)/pft->nind;
  tree->ind.sapwood.nitrogen=(tree->ind.sapwood.nitrogen*nind_old+treepar->sapl.sapwood.nitrogen*est_pft)/pft->nind;
  tree->ind.heartwood.nitrogen=(tree->ind.heartwood.nitrogen*nind_old+treepar->sapl.heartwood.nitrogen*est_pft)/pft->nind;
  tree->ind.debt.nitrogen=tree->ind.debt.nitrogen*nind_old/pft->nind;
  allometry_tree(pft);
  fpc_tree(pft);
  pft->establish.carbon+=flux_est.carbon;
  pft->establish.nitrogen+=flux_est.nitrogen;
  return flux_est;
} /* of 'establishment_tree' */

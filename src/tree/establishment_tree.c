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

Real establishment_tree(Pft *pft,Real UNUSED(fpc_total),
                        Real fpc_type,int n_est)
{

  Real nind_old,acflux_est=0;
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
    return 0.0;
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
  acflux_est=phys_sum_tree(treepar->sapl)*est_pft;

  /* Adjust average individual C biomass based on average biomass and density of the new saplings*/
  tree->ind.leaf=(tree->ind.leaf*nind_old+treepar->sapl.leaf*est_pft)/pft->nind;
  tree->ind.root=(tree->ind.root*nind_old+treepar->sapl.root*est_pft)/pft->nind;
  tree->ind.sapwood=(tree->ind.sapwood*nind_old+treepar->sapl.sapwood*est_pft)/pft->nind;
  tree->ind.heartwood=(tree->ind.heartwood*nind_old+treepar->sapl.heartwood*est_pft)/pft->nind;
  tree->ind.debt=tree->ind.debt*nind_old/pft->nind;
  if((pft->par->phenology==SUMMERGREEN || pft->par->phenology==RAINGREEN) && nind_old==0){
    tree->turn.leaf+=treepar->sapl.leaf*treepar->turnover.leaf;
    tree->turn_litt.leaf+=treepar->sapl.leaf*treepar->turnover.leaf*pft->nind;
    pft->stand->soil.litter.ag[pft->litter].trait.leaf+=treepar->sapl.leaf*treepar->turnover.leaf*pft->nind;
    update_fbd_tree(&pft->stand->soil.litter,pft->par->fuelbulkdensity,(treepar->sapl.leaf*treepar->turnover.leaf*pft->nind),0); //have - before treepar->sapl.leaf deleted
  }
  allometry_tree(pft);
  fpc_tree(pft);
  return acflux_est;
} /* of 'establishment_tree' */

/**************************************************************************************/
/**                                                                                \n**/
/**    e  s  t  a  b  l  i  s  h  m  e  n  t  _  g  r  a  s  s  .  c               \n**/
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
#include "grass.h"

Stocks establishment_grass(Pft *pft,              /**< pointer to grass PFT data */
                           Real fpc_total,        /**< total sum of FPC */
                           Real UNUSED(fpc_type), /**< FPC of grass PFTs */
                           int n_est              /**< number of established grass PFTs */
                          ) /** \return establishment flux (gC/m2, gN/m2) */
{

  Pftgrass *grass;
  Pftgrasspar *grasspar;
  Real est_pft;
  /* establishment rate for a particular PFT on modelled area 
   * basis (for trees, indiv/m2; for grasses, fraction of 
   * modelled area colonised establishment rate for a particular
   * PFT on modelled area basis (for trees, indiv/m2; for 
   * grasses, fraction of modelled area colonised)
   */
  Stocks flux_est;
  if(n_est>0)
  {
    grass=pft->data;
    grasspar=getpftpar(pft,data);
    est_pft=(1.0-fpc_total)/(Real)n_est;
    /* Account for flux from atmosphere to grass regeneration */

    flux_est.carbon=(grasspar->sapl.leaf+grasspar->sapl.root)*est_pft;
    flux_est.nitrogen=flux_est.carbon*grasspar->cn_ratio.leaf;

    /* Add regeneration biomass to overall biomass */

    grass->ind.leaf.carbon+=grasspar->sapl.leaf*est_pft;
    grass->ind.root.carbon+=grasspar->sapl.root*est_pft;
    grass->ind.leaf.nitrogen+=grasspar->sapl.leaf*grasspar->cn_ratio.leaf*est_pft;
    grass->ind.root.nitrogen+=grasspar->sapl.root*grasspar->cn_ratio.leaf*est_pft;
    pft->nleaf=grass->ind.leaf.nitrogen;
  }
  else
    flux_est.carbon=flux_est.nitrogen=0;
  fpc_grass(pft);
  pft->establish.carbon+=flux_est.carbon;
  pft->establish.nitrogen+=flux_est.nitrogen;
  return flux_est;
} /* of 'establishment_grass' */

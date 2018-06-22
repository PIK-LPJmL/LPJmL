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

Real establishment_grass(Pft *pft,Real fpc_total,
                         Real UNUSED(fpc_type),int n_est) 
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
  Real acflux_est;
  if(n_est>0)
  {
    grass=pft->data;
    grasspar=getpftpar(pft,data);
    est_pft=(1.0-fpc_total)/(Real)n_est;
    /* Account for flux from atmosphere to grass regeneration */

    acflux_est=phys_sum_grass(grasspar->sapl)*est_pft;

    /* Add regeneration biomass to overall biomass */

    grass->ind.leaf+=grasspar->sapl.leaf*est_pft;
    grass->ind.root+=grasspar->sapl.root*est_pft;
  }
  else
    acflux_est=0;
  fpc_grass(pft);
  return acflux_est;
} /* of 'establishment_grass' */

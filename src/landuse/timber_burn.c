/**************************************************************************************/
/**                                                                                \n**/
/**             t  i  m  b  e  r  _  b  u  r  n  .  c                              \n**/
/**                                                                                \n**/
/**     extension of LPJ to compute timber harvest / burning                       \n**/
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

Stocks timber_burn(const Pft *pft, /**< Pointer to tree PFT */
                 Real fburnt,    /**< fraction burnt (0..1) */
                 Litter *litter,  /**< Litter pools */
                 Real nind       /**< cannot use pft->nind, since pft is on different stand */
                )                /** \return fire emissions (gC/m2) */
{
  Stocks burn={0,0};
  int i;
  const Pfttree *tree;
  const Pfttreepar *treepar;
  tree=pft->data;
  treepar=pft->par->data;
  if(fburnt<epsilon || nind<epsilon)
    return burn;

  /* reducing carbon in litter pool because this carbon is added to
   * the litter pool in update_litter (next function in reclaim_land().
   * We can't substract it from the vegetation carbon as there
   * is no vegetation on stand2.*/
  for(i=0;i<NFUELCLASS;i++)
  {
    litter->ag[pft->litter].trait.wood[i].carbon-=(tree->ind.heartwood.carbon+tree->ind.sapwood.carbon*2.0/3.0+tree->excess_carbon)*fburnt*nind*treepar->fuelfrac[i];
    litter->ag[pft->litter].trait.wood[i].nitrogen-=(tree->ind.heartwood.nitrogen+tree->ind.sapwood.nitrogen*2.0/3.0)*fburnt*nind*treepar->fuelfrac[i];
  }
  /* computing deforestation fire emissions */
  burn.carbon=(tree->ind.heartwood.carbon+tree->ind.sapwood.carbon*2.0/3.0)*fburnt*nind;
  burn.nitrogen=(tree->ind.heartwood.nitrogen+tree->ind.sapwood.nitrogen*2.0/3.0)*fburnt*nind;
  return burn;
} /* of 'timber_burn' */

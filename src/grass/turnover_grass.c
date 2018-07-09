/**************************************************************************************/
/**                                                                                \n**/
/**           t  u  r  n  o  v  e  r  _  g  r  a  s  s  .  c                       \n**/
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

/*
 *
 *  Function reproduction + turnover
 *
 *  Turnover of PFT-specific fraction from each living C pool
 *  Leaf and root C transferred to litter, sapwood C to heartwood
 *
 */

Stocks turnover_grass(Litter *litter, /**< Litter pool */
                      Pft *pft,       /**< Pointer to PFT variables */
                      Bool new_phenology, /**< new phenology (TRUE/FALSE) */
                      Real fraction   /**< fraction of annual turnover (0..1) */
                     )                /** \return turnover (gC/m2,gN/m2) */
{
  Pftgrass *grass;
  const Pftgrasspar *grasspar;
  Real reprod;
  Grassphys gturn;
  grass=pft->data;
  grasspar=getpftpar(pft,data);
  /* reproduction */
  if(pft->bm_inc.carbon>0)
  {
    reprod=pft->bm_inc.carbon*grasspar->reprod_cost*fraction;
    litter->ag[pft->litter].trait.leaf.carbon+=reprod;
    pft->bm_inc.carbon     -= reprod;
  }
  if(pft->bm_inc.nitrogen>0)
  {
    reprod=pft->bm_inc.nitrogen*grasspar->reprod_cost*fraction;
    litter->ag[pft->litter].trait.leaf.nitrogen+=reprod;
    pft->bm_inc.nitrogen     -= reprod;
  }

  /* turnover */
#ifndef YEARLY_TURNOVER
  if (pft->stand->type->landusetype==NATURAL)
  {
    gturn.root=grass->turn.root;
    gturn.leaf=grass->turn.leaf;
    grass->turn.root.carbon=grass->turn.leaf.carbon=0.0;
    grass->turn.root.nitrogen=grass->turn.leaf.nitrogen=0.0;
    if(new_phenology)
      litter->ag[pft->litter].trait.leaf.nitrogen-=gturn.leaf.nitrogen*pft->nind*(1-pft->par->fn_turnover);
  }
  else
#endif 
 {

    gturn.root.carbon=grass->ind.root.carbon*grasspar->turnover.root*fraction;
    gturn.root.nitrogen=grass->ind.root.nitrogen*grasspar->turnover.root*fraction;
    gturn.leaf.carbon=grass->ind.leaf.carbon*grasspar->turnover.leaf*fraction;
    gturn.leaf.nitrogen=grass->ind.leaf.nitrogen*grasspar->turnover.leaf*fraction;
    litter->ag[pft->litter].trait.leaf.carbon+=gturn.leaf.carbon*pft->nind;
    litter->ag[pft->litter].trait.leaf.nitrogen+=gturn.leaf.nitrogen*pft->nind;
    update_fbd_grass(litter,pft->par->fuelbulkdensity,gturn.leaf.carbon*pft->nind);
    litter->bg[pft->litter].carbon+=gturn.root.carbon*pft->nind;
    litter->bg[pft->litter].nitrogen+=gturn.root.nitrogen*pft->nind;
  }
  grass->ind.leaf.carbon-= gturn.leaf.carbon;
  grass->ind.leaf.nitrogen-= gturn.leaf.nitrogen;
  grass->ind.root.carbon-= gturn.root.carbon;
  grass->ind.root.nitrogen-= gturn.root.nitrogen;
  pft->bm_inc.nitrogen+= (gturn.root.nitrogen+gturn.leaf.nitrogen)*pft->nind*(1-pft->par->fn_turnover);
  /* turnover of excess carbon as root exudates */
  litter->bg[pft->litter].carbon+=grass->excess_carbon*pft->nind*grasspar->turnover.root;
  grass->excess_carbon-=grass->excess_carbon*grasspar->turnover.root;
  gturn.leaf.carbon+=gturn.root.carbon;
  gturn.leaf.nitrogen+=gturn.root.nitrogen;
  return gturn.leaf;
} /* of 'turnover_grass' */

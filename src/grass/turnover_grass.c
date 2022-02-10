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
                      Real fraction,   /**< fraction of annual turnover (0..1) */
                      const Config *config /**< LPJmL configuration*/
                     )                /** \return turnover (gC/m2,gN/m2) */
{
  Pftgrass *grass;
  const Pftgrasspar *grasspar;
  Output *output;
  Real reprod;
  Grassphys gturn;
  grass=pft->data;
  grasspar=getpftpar(pft,data);
  output=&pft->stand->cell->output;
  /* reproduction */
  if(pft->bm_inc.carbon>0)
  {
    reprod=pft->bm_inc.carbon*grasspar->reprod_cost*fraction;
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
    litter->item[pft->litter].ag.leaf.carbon+=reprod;
    getoutput(output,LITFALLC,config)+=reprod*pft->stand->frac;
  }
  if(pft->bm_inc.nitrogen>0)
  {
    reprod=pft->bm_inc.nitrogen*grasspar->reprod_cost*fraction;
    pft->bm_inc.nitrogen-=reprod;
    if(pft->establish.nitrogen<reprod)
    {
      reprod-=pft->establish.nitrogen;
      getoutput(output,FLUX_ESTABN,config)-=pft->establish.nitrogen*pft->stand->frac;
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
    litter->item[pft->litter].ag.leaf.nitrogen+=reprod;
    getoutput(output,LITFALLN,config)+=reprod*pft->stand->frac;
  }

  /* turnover */
  if (pft->stand->type->landusetype==NATURAL)
  {
    gturn.root=grass->turn.root;
    gturn.leaf=grass->turn.leaf;
    /* update litter pools to prevent carbon balance error if pft->nind has been changed */
    litter->item[pft->litter].ag.leaf.carbon+=gturn.leaf.carbon*pft->nind-grass->turn_litt.leaf.carbon;
    litter->item[pft->litter].ag.leaf.nitrogen+=gturn.leaf.nitrogen*pft->nind-grass->turn_litt.leaf.nitrogen;
    update_fbd_tree(litter,pft->par->fuelbulkdensity,gturn.leaf.carbon*pft->nind-grass->turn_litt.leaf.carbon,0);
    litter->item[pft->litter].bg.carbon+=gturn.root.carbon*pft->nind-grass->turn_litt.root.carbon;
    litter->item[pft->litter].bg.nitrogen+=gturn.root.nitrogen*pft->nind-grass->turn_litt.root.nitrogen;
    grass->turn.root.carbon=grass->turn_litt.root.carbon=grass->turn.leaf.carbon=grass->turn_litt.leaf.carbon=0.0;
    grass->turn.root.nitrogen=grass->turn_litt.root.nitrogen=grass->turn.leaf.nitrogen=grass->turn_litt.leaf.nitrogen=0.0;
  }
  else
  {
    gturn.root.carbon=grass->ind.root.carbon*grasspar->turnover.root*fraction;
    gturn.root.nitrogen=grass->ind.root.nitrogen*grasspar->turnover.root*fraction;
    gturn.leaf.carbon=grass->ind.leaf.carbon*grasspar->turnover.leaf*fraction;
    gturn.leaf.nitrogen=grass->ind.leaf.nitrogen*grasspar->turnover.leaf*fraction;
    litter->item[pft->litter].ag.leaf.carbon+=gturn.leaf.carbon*pft->nind;
    getoutput(output,LITFALLC,config)+=gturn.leaf.carbon*pft->nind*pft->stand->frac;
    litter->item[pft->litter].ag.leaf.nitrogen+=gturn.leaf.nitrogen*pft->nind*pft->par->fn_turnover;
    getoutput(output,LITFALLN,config)+=gturn.leaf.nitrogen*pft->nind*pft->par->fn_turnover*pft->stand->frac;
    update_fbd_grass(litter,pft->par->fuelbulkdensity,gturn.leaf.carbon*pft->nind);
    litter->item[pft->litter].bg.carbon+=gturn.root.carbon*pft->nind;
    getoutput(output,LITFALLC,config)+=gturn.root.carbon*pft->nind*pft->stand->frac;
    litter->item[pft->litter].bg.nitrogen+=gturn.root.nitrogen*pft->nind*pft->par->fn_turnover;
    getoutput(output,LITFALLN,config)+=gturn.root.nitrogen*pft->nind*pft->par->fn_turnover*pft->stand->frac;
  }
  grass->ind.leaf.carbon-= gturn.leaf.carbon;
  grass->ind.leaf.nitrogen-= gturn.leaf.nitrogen;
  grass->ind.root.carbon-= gturn.root.carbon;
  grass->ind.root.nitrogen-= gturn.root.nitrogen;
  pft->bm_inc.nitrogen+= (gturn.root.nitrogen+gturn.leaf.nitrogen)*pft->nind*(1-pft->par->fn_turnover);
  /* turnover of excess carbon as root exudates */
  litter->item[pft->litter].bg.carbon+=grass->excess_carbon*pft->nind*grasspar->turnover.root;
  getoutput(output,LITFALLC,config)+=grass->excess_carbon*pft->nind*grasspar->turnover.root*pft->stand->frac;
  grass->excess_carbon-=grass->excess_carbon*grasspar->turnover.root;
  gturn.leaf.carbon+=gturn.root.carbon;
  gturn.leaf.nitrogen+=gturn.root.nitrogen;
  return gturn.leaf;
} /* of 'turnover_grass' */

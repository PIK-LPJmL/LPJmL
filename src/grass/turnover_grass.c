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
  Real reprod=0;
  Real reprod_out=0;
  Grassphys gturn={{0,0},{0,0}};
  grass=pft->data;
  grasspar=getpftpar(pft,data);
  output=&pft->stand->cell->output;
#ifdef CHECK_BALANCE
  Real end,litter_old,establish_old,reprod1=0,vegsum_old,bminc_old,ecxess_carbon_old;
  Stocks start={0,0};
  Stocks stocks;
  stocks=litterstocks(litter);
  vegsum_old= vegc_sum(pft);
  start.carbon=vegc_sum(pft)+pft->bm_inc.carbon+stocks.carbon-pft->establish.carbon;
  start.nitrogen=vegn_sum(pft)+pft->bm_inc.nitrogen+stocks.nitrogen-pft->establish.nitrogen;
  establish_old=pft->establish.nitrogen;
  bminc_old=pft->bm_inc.nitrogen;
  ecxess_carbon_old=grass->excess_carbon;
#endif

  /* reproduction */
  if(pft->bm_inc.carbon>0)
  {
    reprod=pft->bm_inc.carbon*grasspar->reprod_cost*fraction;
#ifdef CHECK_BALANCE
    reprod1=reprod;
#endif
    pft->bm_inc.carbon-=reprod;
    reprod_out=reprod;
    if(pft->establish.carbon<reprod)
    {
      reprod-=pft->establish.carbon;
      getoutput(output,FLUX_ESTABC,config)-=pft->establish.carbon*pft->stand->frac;
      pft->stand->cell->balance.flux_estab.carbon-=pft->establish.carbon*pft->stand->frac;
      if(isnatural(pft->stand))
        pft->stand->cell->balance.nat_fluxes-=pft->establish.carbon*pft->stand->frac;
      pft->establish.carbon=0;
    }
    else
    {
      getoutput(output,FLUX_ESTABC,config)-=reprod*pft->stand->frac;
      pft->stand->cell->balance.flux_estab.carbon-=reprod*pft->stand->frac;
      if(isnatural(pft->stand))
        pft->stand->cell->balance.nat_fluxes-=reprod*pft->stand->frac;

      pft->establish.carbon-=reprod;
      reprod=0;
    }
    litter->item[pft->litter].agtop.leaf.carbon+=reprod;
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
      if(!isnatural(pft->stand))
        getoutput(output,FLUX_ESTABN_MG,config)-=reprod*pft->stand->frac;
      pft->stand->cell->balance.flux_estab.nitrogen-=reprod*pft->stand->frac;
      pft->establish.nitrogen-=reprod;
      reprod=0;
    }
    litter->item[pft->litter].agtop.leaf.nitrogen+=reprod;
    getoutput(output,LITFALLN,config)+=reprod*pft->stand->frac;
  }

  /* turnover */
  if(isnatural(pft->stand))
  {
    gturn.root=grass->turn.root;
    gturn.leaf=grass->turn.leaf;
    /* update litter pools to prevent carbon balance error if pft->nind has been changed */
    litter->item[pft->litter].agtop.leaf.carbon+=gturn.leaf.carbon*pft->nind-grass->turn_litt.leaf.carbon;
    litter->item[pft->litter].agtop.leaf.nitrogen+=(gturn.leaf.nitrogen*pft->nind-grass->turn_litt.leaf.nitrogen)*pft->par->fn_turnover;
    pft->bm_inc.nitrogen+=(gturn.leaf.nitrogen*pft->nind-grass->turn_litt.leaf.nitrogen)*(1-pft->par->fn_turnover);
    update_fbd_grass(litter,pft->par->fuelbulkdensity,gturn.leaf.carbon*pft->nind-grass->turn_litt.leaf.carbon);
    litter->item[pft->litter].bg.carbon+=gturn.root.carbon*pft->nind-grass->turn_litt.root.carbon;
    litter->item[pft->litter].bg.nitrogen+=(gturn.root.nitrogen*pft->nind-grass->turn_litt.root.nitrogen)*pft->par->fn_turnover;
    pft->bm_inc.nitrogen+=(gturn.root.nitrogen*pft->nind-grass->turn_litt.root.nitrogen)*(1-pft->par->fn_turnover);
  }
  else if(pft->stand->type->landusetype==BIOMASS_GRASS)
  {
    grass->ind.root.carbon-=grass->turn.root.carbon;
    grass->ind.root.nitrogen-=grass->turn.root.nitrogen;
    gturn.root.carbon=grass->ind.root.carbon*grasspar->turnover.root*fraction;
    gturn.root.nitrogen=grass->ind.root.nitrogen*grasspar->turnover.root*fraction;
    litter->item[pft->litter].bg.carbon+=gturn.root.carbon*pft->nind;
    getoutput(output,LITFALLC,config)+=gturn.root.carbon*pft->nind*pft->stand->frac;
    litter->item[pft->litter].bg.nitrogen+=gturn.root.nitrogen*pft->nind*pft->par->fn_turnover;
    getoutput(output,LITFALLN,config)+=gturn.root.nitrogen*pft->nind*pft->par->fn_turnover*pft->stand->frac;
    pft->bm_inc.nitrogen+=gturn.root.nitrogen*pft->nind*(1-pft->par->fn_turnover);
  }
  else
  {
    grass->ind.root.carbon-=grass->turn.root.carbon;
    grass->ind.leaf.carbon-=grass->turn.leaf.carbon;
    grass->ind.root.nitrogen-=grass->turn.root.nitrogen;
    grass->ind.leaf.nitrogen-=grass->turn.leaf.nitrogen;
    litter->item[pft->litter].agtop.leaf.carbon+=grass->turn.leaf.carbon*pft->nind-grass->turn_litt.leaf.carbon;
    litter->item[pft->litter].agtop.leaf.nitrogen+=(grass->turn.leaf.nitrogen*pft->nind-grass->turn_litt.leaf.nitrogen)*pft->par->fn_turnover;
    pft->bm_inc.nitrogen+=(grass->turn.leaf.nitrogen*pft->nind-grass->turn_litt.leaf.nitrogen)*(1-pft->par->fn_turnover);
    update_fbd_grass(litter,pft->par->fuelbulkdensity,grass->turn.leaf.carbon*pft->nind-grass->turn_litt.leaf.carbon);
    litter->item[pft->litter].bg.carbon+=grass->turn.root.carbon*pft->nind-grass->turn_litt.root.carbon;
    litter->item[pft->litter].bg.nitrogen+=(grass->turn.root.nitrogen*pft->nind-grass->turn_litt.root.nitrogen)*pft->par->fn_turnover;
    pft->bm_inc.nitrogen+=(grass->turn.root.nitrogen*pft->nind-grass->turn_litt.root.nitrogen)*(1-pft->par->fn_turnover);
    gturn.root.carbon=grass->ind.root.carbon*grasspar->turnover.root*fraction;
    gturn.root.nitrogen=grass->ind.root.nitrogen*grasspar->turnover.root*fraction;
    gturn.leaf.carbon=grass->ind.leaf.carbon*grasspar->turnover.leaf*fraction;
    gturn.leaf.nitrogen=grass->ind.leaf.nitrogen*grasspar->turnover.leaf*fraction;
    litter->item[pft->litter].agtop.leaf.carbon+=gturn.leaf.carbon*pft->nind;
    getoutput(output,LITFALLC,config)+=gturn.leaf.carbon*pft->nind*pft->stand->frac;
    litter->item[pft->litter].agtop.leaf.nitrogen+=gturn.leaf.nitrogen*pft->nind*pft->par->fn_turnover;
    getoutput(output,LITFALLN,config)+=gturn.leaf.nitrogen*pft->nind*pft->par->fn_turnover*pft->stand->frac;
    update_fbd_grass(litter,pft->par->fuelbulkdensity,gturn.leaf.carbon*pft->nind);
    litter->item[pft->litter].bg.carbon+=gturn.root.carbon*pft->nind;
    getoutput(output,LITFALLC,config)+=gturn.root.carbon*pft->nind*pft->stand->frac;
    litter->item[pft->litter].bg.nitrogen+=gturn.root.nitrogen*pft->nind*pft->par->fn_turnover;
    getoutput(output,LITFALLN,config)+=gturn.root.nitrogen*pft->nind*pft->par->fn_turnover*pft->stand->frac;
    pft->bm_inc.nitrogen+=(gturn.root.nitrogen+gturn.leaf.nitrogen)*pft->nind*(1-pft->par->fn_turnover);
  }
  grass->turn.root.carbon=grass->turn_litt.root.carbon=grass->turn.leaf.carbon=grass->turn_litt.leaf.carbon=0.0;
  grass->turn.root.nitrogen=grass->turn_litt.root.nitrogen=grass->turn.leaf.nitrogen=grass->turn_litt.leaf.nitrogen=0.0;

  grass->ind.leaf.carbon-= gturn.leaf.carbon;
  grass->ind.leaf.nitrogen-= gturn.leaf.nitrogen;
  grass->ind.root.carbon-= gturn.root.carbon;
  grass->ind.root.nitrogen-= gturn.root.nitrogen;
  /* turnover of excess carbon as root exudates */
  litter->item[pft->litter].bg.carbon+=grass->excess_carbon*pft->nind*grasspar->turnover.root;
  getoutput(output,LITFALLC,config)+=grass->excess_carbon*pft->nind*grasspar->turnover.root*pft->stand->frac;
  grass->excess_carbon-=grass->excess_carbon*grasspar->turnover.root;
  gturn.leaf.carbon+=gturn.root.carbon;
  gturn.leaf.nitrogen+=gturn.root.nitrogen;
  getoutput(&pft->stand->cell->output,RA,config)+=(reprod_out)*pft->stand->frac;
#ifdef CHECK_BALANCE
  litter_old=stocks.nitrogen;
  stocks=litterstocks(litter);
  end = vegc_sum(pft)+pft->bm_inc.carbon+stocks.carbon-pft->establish.carbon;
  if(fabs(end-start.carbon)>0.0001)
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,TRUE,"Invalid carbon balance in turnover_grass(): landusetype: %s error: %g start: %g end: %g bm_inc.carbon: %g\n"
         "=====001: bminc_old: %g PFT:%s nind: %g leaf_turn_litt: %g root_turn_litt: %g root_turn: %g\n"
         "=====002: leaf_turn: %g reprod: %g litter_old: %g litter: %g est.carbon: %g est.carbon_old: %g\n"
         "=====003:  fraction: %g vegsum: %g vegsum_old: %g excess_carbon: %g excess_carbon_old: %g",
         pft->stand->type->name,end-start.carbon, start.carbon,end,pft->bm_inc.carbon,bminc_old,pft->par->name,pft->nind,grass->turn_litt.root.carbon,grass->turn_litt.leaf.carbon,
         grass->turn.root.carbon,grass->turn.leaf.carbon,reprod1,litter_old,stocks.carbon,pft->establish.carbon, establish_old, fraction,vegc_sum(pft),vegsum_old,grass->excess_carbon,ecxess_carbon_old);

  end = vegn_sum(pft)+pft->bm_inc.nitrogen+stocks.nitrogen-pft->establish.nitrogen;
  if(fabs(end-start.nitrogen)>0.00001)
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,TRUE,"Invalid nitrogen balance in turnover_grass(): landusetype:%s error: %g start: %g end: %g\n"
         "=====001: bm_inc.nitrogen: %g bminc_old: %g PFT:%s nind: %g leaf_turn_litt: %g root_turn_litt: %g root_turn: %g\n"
         "=====002: leaf_turn: %g reprod: %g litter_old: %g litter: %g est.nitrogen: %g\n"
         "=====003: est.nitrogen_old: %g fraction: %g vegsum: %g",
         pft->stand->type->name,end-start.nitrogen, start.nitrogen,end,pft->bm_inc.nitrogen,bminc_old,pft->par->name,pft->nind,grass->turn_litt.root.nitrogen,grass->turn_litt.leaf.nitrogen,
         grass->turn.root.nitrogen,grass->turn.leaf.nitrogen,reprod1,litter_old,stocks.nitrogen,pft->establish.nitrogen, establish_old, fraction,vegc_sum(pft));
#endif
  return gturn.leaf;
} /* of 'turnover_grass' */

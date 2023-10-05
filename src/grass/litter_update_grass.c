/**************************************************************************************/
/**                                                                                \n**/
/**     l  i  t  t  e  r  _  u  p  d  a  t  e  _  g  r  a  s  s  .  c              \n**/
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

void litter_update_grass(Litter *litter, /**< Litter pool */
                         Pft *pft,       /**< PFT variables */
                         Real frac,      /**< fraction added to litter (0..1) */
                         const Config *config /**< LPJmL configuration */
                        )
{
  Pftgrass *grass;
  Output *output;
  grass=pft->data;
  output=&pft->stand->cell->output; 
#ifdef CHECK_BALANCE
  Real end,vegcsum_alt,bminc_alt,ecxess_carbon_alt,vegnsum_alt,turn_old,correc_old;
  Stocks start={0,0};
  Stocks stocks,litter_alt;
  stocks=litterstocks(litter);
  litter_alt=stocks;
  vegcsum_alt=vegc_sum(pft)+pft->bm_inc.carbon;
  vegnsum_alt=vegn_sum(pft)+pft->bm_inc.nitrogen;
  start.carbon=vegc_sum(pft)+pft->bm_inc.carbon+stocks.carbon;
  start.nitrogen=vegn_sum(pft)+pft->bm_inc.nitrogen+stocks.nitrogen;
  bminc_alt=pft->bm_inc.nitrogen;
  turn_old=(grass->turn.root.nitrogen+grass->turn.leaf.nitrogen)*pft->nind*(1-pft->par->fn_turnover);
  ecxess_carbon_alt=grass->excess_carbon;
  correc_old=pft->nbalance_cor;
#endif
  grass->ind.leaf.carbon-= grass->turn.leaf.carbon;
  grass->ind.root.carbon-= grass->turn.root.carbon;
  grass->ind.leaf.nitrogen-= grass->turn.leaf.nitrogen;
  grass->ind.root.nitrogen-= grass->turn.root.nitrogen;
  pft->bm_inc.nitrogen+=pft->nbalance_cor;
  pft->nbalance_cor=0;
  litter->item[pft->litter].agtop.leaf.carbon+=grass->turn.leaf.carbon*pft->nind-grass->turn_litt.leaf.carbon;
  litter->item[pft->litter].agtop.leaf.nitrogen+=grass->turn.leaf.nitrogen*pft->nind-grass->turn_litt.leaf.nitrogen;
  update_fbd_grass(litter,pft->par->fuelbulkdensity,grass->turn.leaf.carbon*pft->nind-grass->turn_litt.leaf.carbon);
  litter->item[pft->litter].bg.carbon+=grass->turn.root.carbon*pft->nind-grass->turn_litt.root.carbon;
  litter->item[pft->litter].bg.nitrogen+=grass->turn.root.nitrogen*pft->nind-grass->turn_litt.root.nitrogen;
  grass->turn.root.carbon=grass->turn.leaf.carbon=grass->turn_litt.leaf.carbon=grass->turn_litt.root.carbon=0.0;
  grass->turn.root.nitrogen=grass->turn.leaf.nitrogen=grass->turn_litt.leaf.nitrogen=grass->turn_litt.root.nitrogen=0.0;

  litter->item[pft->litter].agtop.leaf.carbon+=grass->ind.leaf.carbon*frac;
  getoutput(output,LITFALLC,config)+=grass->ind.leaf.carbon*frac*pft->stand->frac;
  litter->item[pft->litter].agtop.leaf.nitrogen+=grass->ind.leaf.nitrogen*frac;
  getoutput(output,LITFALLN,config)+=grass->ind.leaf.nitrogen*frac*pft->stand->frac;
  litter->item[pft->litter].agtop.leaf.nitrogen+=pft->bm_inc.nitrogen*frac;
  getoutput(output,LITFALLN,config)+=pft->bm_inc.nitrogen*frac*pft->stand->frac;
  update_fbd_grass(litter,pft->par->fuelbulkdensity,
                   grass->ind.leaf.carbon*frac);
  litter->item[pft->litter].bg.carbon+=grass->ind.root.carbon*frac;
  getoutput(output,LITFALLC,config)+=grass->ind.root.carbon*frac*pft->stand->frac;
  litter->item[pft->litter].bg.nitrogen+=grass->ind.root.nitrogen*frac;
  getoutput(output,LITFALLN,config)+=grass->ind.root.nitrogen*frac*pft->stand->frac;
#ifdef CHECK_BALANCE
  stocks=litterstocks(litter);
  end = vegc_sum(pft)+pft->bm_inc.carbon+stocks.carbon;
  //vegC needs to be substracted as it is not balanced here
  if(fabs(end-start.carbon-(vegc_sum(pft)+pft->bm_inc.carbon))>0.01)
    fprintf(stderr, "C_ERROR litter_update grass landusetype %s : %g start : %g end : %g  bm_inc.carbon: %g  bminc_alt: %g  PFT:%s nind: %g leaf_turn_litt: %g root_turn_litt: %g  root_turn: %g"
        "  leaf_turn: %g litter_alt: %g  litter: %g est.carbon: %g vegsum: %g vegsum_alt: %g excess_carbon: %g excess_carbon_alt: %g frac: %g\n",
        pft->stand->type->name,end-start.carbon-(vegc_sum(pft)+pft->bm_inc.carbon), start.carbon,end,pft->bm_inc.carbon,bminc_alt,pft->par->name,pft->nind,grass->turn_litt.root.carbon,grass->turn_litt.leaf.carbon,
        grass->turn.root.carbon,grass->turn.leaf.carbon,litter_alt.carbon,stocks.carbon,pft->establish.carbon,vegc_sum(pft)+pft->bm_inc.carbon,vegcsum_alt,grass->excess_carbon,ecxess_carbon_alt,frac);

  end = vegn_sum(pft)+pft->bm_inc.nitrogen+stocks.nitrogen;
  if(fabs(end-start.nitrogen-(vegn_sum(pft)+pft->bm_inc.nitrogen))>0.01)
    fprintf(stderr, "N_ERROR litter_update grass landusetype %s : %g start : %g end : %g  bm_inc.nitrogen: %g  bminc_alt: %g  PFT:%s nind: %g leaf_turn_litt: %g root_turn_litt: %g  root_turn: %g"
        "  leaf_turn: %g turn_old: %g litter_alt: %g  litter: %g est.nitrogen: %g  vegsum: %g veg_sum_alt; %g correction:%g correc_old: %g frac: %g\n \n",
        pft->stand->type->name,end-start.nitrogen-(vegn_sum(pft)+pft->bm_inc.nitrogen), start.nitrogen,end,pft->bm_inc.nitrogen,bminc_alt,pft->par->name,pft->nind,grass->turn_litt.root.nitrogen,grass->turn_litt.leaf.nitrogen,
        grass->turn.root.nitrogen,grass->turn.leaf.nitrogen,turn_old,litter_alt.nitrogen,stocks.nitrogen,pft->establish.nitrogen,vegn_sum(pft)+pft->bm_inc.nitrogen,vegnsum_alt,pft->nbalance_cor,correc_old,frac);
#endif

} /* of 'litter_update_grass' */

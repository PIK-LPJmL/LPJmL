/**************************************************************************************/
/**                                                                                \n**/
/**                s  t  a  n  d  s  t  o  c  k  s  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function computes total carbon and nitrogen in stand                       \n**/
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

Stocks standstocks(const Stand *stand /**< pointer to stand */
                  )                   /** \return stocks sum (gC/m2,gN/m2) */
{
  int p;
  const Pft *pft;
  Pftgrass *grass;
  Pfttree *tree;
  Stocks tot;
  Stocks turn;
  Real turn_all=0;
  tot=soilstocks(&stand->soil); /* get stocks in soil */
  //fprintf(stderr,"type: %d  frac: %g  iswetland: %d soilccarbon: %g ",stand->type->landusetype, stand-> frac, stand->soil.iswetland,tot);
  foreachpft(pft,p,&stand->pftlist)
  {
    turn.carbon=turn.nitrogen=0;
//    if(istree(pft))
//    {
//      tree=pft->data;
//      turn.carbon=tree->turn_litt.leaf.carbon+tree->turn_litt.root.carbon;
//      turn.nitrogen=tree->turn_litt.leaf.nitrogen+tree->turn_litt.root.nitrogen;
//    }
//    if(isgrass(pft))
//    {
//      grass=pft->data;
//      turn.carbon=grass->turn_litt.leaf.carbon+grass->turn_litt.root.carbon;
//      turn.nitrogen=grass->turn_litt.leaf.nitrogen+grass->turn_litt.root.nitrogen;
//    }
    turn_all+=turn.carbon;
    if(stand->type->landusetype!=AGRICULTURE)
     tot.carbon+=vegc_sum(pft)+pft->bm_inc.carbon-turn.carbon; /* sum up carbon in PFTs */
    else
      tot.carbon+=vegc_sum(pft)-turn.carbon; /* sum up carbon in PFTs */

    //fprintf(stdout,"vegetation carbon %g PFT: %s bm_inc.carbon: %g\n",vegc_sum(pft),pft->par->name,pft->bm_inc.carbon);
    tot.nitrogen+=vegn_sum(pft)+pft->bm_inc.nitrogen-turn.nitrogen; /* sum up nitrogen in PFTs */
    if(pft->par->cultivation_type==ANNUAL_CROP)
      tot.nitrogen-=pft->bm_inc.nitrogen;
  }
  //fprintf(stderr,"standstocks:tot_carbon: %g turn_all_carbon %g  \n \n \n",tot.carbon,turn_all);
  return tot;
} /* of 'standstocks' */

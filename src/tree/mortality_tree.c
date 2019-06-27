/**************************************************************************************/
/**                                                                                \n**/
/**                m  o  r  t  a  l  i  t  y  _  t  r  e  e  .  c                  \n**/
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

#define ramp_gddtw 400.0 /* ramp for heat damage function. Above 200      */
                         /* growing degree days above the upper limit tw, */
                         /* establishment is zero and mortality 100%      */

/*
 *
 *     Function mortality
 *
 *     Tree background and stress mortality
 *
 */

Bool mortality_tree(Litter *litter,   /**< Litter                              */
                    Pft *pft,         /**< Pointer to pft                      */
                    Real turnover_ind,/**< indivudual turnover                 */
                    Real mtemp_max,   /**< maximum temperature of month (deg C)*/
                    Bool isdaily      /**< daily temperature data (TRUE/FALSE) */
                   )                  /** \return TRUE on death                */
{
  Real mort,bm_delta,heatstress,nind_kill,mort_max;
  Pfttree *tree;
  tree=pft->data;
  bm_delta=pft->bm_inc/pft->nind-turnover_ind;
  if(bm_delta<0)
   bm_delta=0;
  if (pft->par->cultivation_type==BIOMASS)
    mort_max=0.005;
  else
    mort_max=pft->par->mort_max;

  /* switch off background mortality in case of prescribed land cover */
  if (pft->prescribe_fpc)
    mort = 0.0;
  else
    mort=mort_max/(1+param.k_mort*bm_delta/tree->ind.leaf/pft->par->sla);
  if(mtemp_max>((isdaily) ? pft->par->twmax_daily : pft->par->twmax))
  {
    heatstress=tree->gddtw/ramp_gddtw;
    if(heatstress>1)
      heatstress=1;
    mort+=heatstress;
  }
  else
    heatstress=0;
  nind_kill=(mort>1) ? pft->nind : pft->nind*mort;
  pft->nind-=nind_kill;
  litter_update_tree(litter,pft,nind_kill);
  fpc_tree(pft);
  return isneg_tree(pft);
} /* of 'mortality_tree' */

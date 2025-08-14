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
  grass->ind.leaf.carbon-= grass->turn.leaf.carbon;
  grass->ind.root.carbon-= grass->turn.root.carbon;
  grass->ind.leaf.nitrogen-= grass->turn.leaf.nitrogen;
  grass->ind.root.nitrogen-= grass->turn.root.nitrogen;
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
} /* of 'litter_update_grass' */

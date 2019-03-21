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
                         Real frac       /**< fraction added to litter (0..1) */
                        )
{
  Pftgrass *grass;
  Output *output;
  grass=pft->data;
  output=&pft->stand->cell->output; 
  litter->ag[pft->litter].trait.leaf.carbon+=grass->ind.leaf.carbon*frac;
  output->alittfall.carbon+=grass->ind.leaf.carbon*frac*pft->stand->frac;
  litter->ag[pft->litter].trait.leaf.nitrogen+=grass->ind.leaf.nitrogen*frac;
  output->alittfall.nitrogen+=grass->ind.leaf.nitrogen*frac*pft->stand->frac;
  litter->ag[pft->litter].trait.leaf.nitrogen+=pft->bm_inc.nitrogen*frac;
  output->alittfall.nitrogen+=pft->bm_inc.nitrogen*frac*pft->stand->frac;
  update_fbd_grass(litter,pft->par->fuelbulkdensity,
                   grass->ind.leaf.carbon*frac);
  litter->bg[pft->litter].carbon+=grass->ind.root.carbon*frac;
  output->alittfall.carbon+=grass->ind.root.carbon*frac*pft->stand->frac;
  litter->bg[pft->litter].nitrogen+=grass->ind.root.nitrogen*frac;
  output->alittfall.nitrogen+=grass->ind.root.nitrogen*frac*pft->stand->frac;
} /* of 'litter_update_grass' */

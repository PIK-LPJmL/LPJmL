/**************************************************************************************/
/**                                                                                \n**/
/**     l  i  t  t  e  r  _  u  p  d  a  t  e  _  c  r  o  p  .  c                 \n**/
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
#include "crop.h"

void litter_update_crop(Litter *litter, /**< Litter pools */
                        Pft *pft,       /**< PFT variables */
                        Real frac       /**< fraction (0..1) */
                       )
{
  Pftcrop *crop;
  Output *output;  
  crop=pft->data;
  output=&pft->stand->cell->output; 
  litter->item[pft->litter].ag.leaf.carbon+=(crop->ind.leaf.carbon+crop->ind.pool.carbon+crop->ind.so.carbon)*frac;
  output->alittfall.carbon+=(crop->ind.leaf.carbon+crop->ind.pool.carbon+crop->ind.so.carbon)*frac*pft->stand->frac;
  litter->item[pft->litter].ag.leaf.nitrogen+=(crop->ind.leaf.nitrogen+crop->ind.pool.nitrogen+crop->ind.so.nitrogen)*frac;
  output->alittfall.nitrogen+=(crop->ind.leaf.nitrogen+crop->ind.pool.nitrogen+crop->ind.so.nitrogen)*frac*pft->stand->frac;
  update_fbd_grass(litter,pft->par->fuelbulkdensity,
                   (crop->ind.leaf.carbon+crop->ind.so.carbon+crop->ind.pool.carbon)*frac);
  litter->item[pft->litter].bg.carbon+=crop->ind.root.carbon*frac;
  output->alittfall.carbon+=crop->ind.root.carbon*frac*pft->stand->frac;
  litter->item[pft->litter].bg.nitrogen+=crop->ind.root.nitrogen*frac;
  output->alittfall.nitrogen+=crop->ind.root.nitrogen*frac*pft->stand->frac;

#ifdef DEBUG3
  printf("%s ag=%.2f bg=%.2f ind.so=%.2f ind.leaf=%.2f ind.pool=%.2f ind.root=%.2f pft->bm_inc_%.2f\n",
         pft->par->name,litter->item[pft->litter].ag.leaf.carbon,litter->item[pft->litter].bg.carbon,crop->ind.so.carbon,
         crop->ind.leaf.carbon,crop->ind.pool.carbon,crop->ind.root.carbon,pft->bm_inc.carbon);
#endif
} /* of 'litter_update_crop' */

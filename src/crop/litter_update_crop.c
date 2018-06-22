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
  
  crop=pft->data;
  
  litter->ag[pft->litter].trait.leaf+=(crop->ind.leaf+crop->ind.pool+crop->ind.so)*frac;
  update_fbd_grass(litter,pft->par->fuelbulkdensity,
                   (crop->ind.leaf+crop->ind.so+crop->ind.pool)*frac);
  litter->bg[pft->litter]+=crop->ind.root*frac;

#ifdef DEBUG3
  printf("%s ag=%.2f bg=%.2f ind.so=%.2f ind.leaf=%.2f ind.pool=%.2f ind.root=%.2f pft->bm_inc_%.2f\n",
         pft->par->name,litter->ag[pft->litter].trait.leaf,litter->bg[pft->litter],crop->ind.so,
         crop->ind.leaf,crop->ind.pool,crop->ind.root,pft->bm_inc);
#endif
} /* of 'litter_update_crop' */

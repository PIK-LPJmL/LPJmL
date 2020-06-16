/**************************************************************************************/
/**                                                                                \n**/
/**               r  e  d  u  c  e  _  t  r  e  e  .  c                            \n**/
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

void reduce_tree(Litter *litter,Pft *pft,Real factor) 
{
  pft->nind/=factor;
  pft->fpc/=factor;
  litter_update_tree(litter,pft,pft->nind*factor-pft->nind); 
  
} /* of 'reduce_tree' */

/**************************************************************************************/
/**                                                                                \n**/
/** CALLED FROM:                                                                   \n**/
/**   establishment.c (foreachpft)                                                 \n**/
/** PURPOSE:                                                                       \n**/
/**   reduce nind and fpc by a factor                                              \n**/
/** DETAILED DESCRIPTION:                                                          \n**/
/**   litter_update_tree is used to transfer the "waste" carbon into the           \n**/
/**   litter pool in order to maintain carbon balance                              \n**/
/**                                                                                \n**/
/**************************************************************************************/

/**************************************************************************************/
/**                                                                                \n**/
/**               r  e  d  u  c  e  _  g  r  a  s  s  .  c                         \n**/
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

void reduce_grass(Litter * UNUSED(litter),Pft *pft,Real factor) 
{
  pft->fpc/=factor;
  
} /* of 'reduce_grass' */

/**************************************************************************************/
/**                                                                                \n**/
/** CALLED FROM:                                                                   \n**/
/**   establishment.c (foreachpft)                                                 \n**/
/** PURPOSE:                                                                       \n**/
/**   reduce fpc by factor                                                         \n**/
/**                                                                                \n**/
/**************************************************************************************/

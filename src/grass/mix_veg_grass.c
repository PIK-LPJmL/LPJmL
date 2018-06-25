/**************************************************************************************/
/**                                                                                \n**/
/**     m  i  x  _  v  e  g  _  g  r  a  s  s  .  c                                \n**/
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

void mix_veg_grass(Pft *pft,Real scaler)
{
  Pftgrass *grass;
  grass=pft->data;
  
  grass->ind.leaf*=scaler;
  grass->ind.root*=scaler;
  grass->turn.root*=scaler;
  grass->turn.leaf*=scaler;

  pft->bm_inc*=scaler;
  
} /* of 'mix_veg_grass' */

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
  
  grass->ind.leaf.carbon*=scaler;
  grass->ind.leaf.nitrogen*=scaler;
  grass->ind.root.carbon*=scaler;
  grass->ind.root.nitrogen*=scaler;
  grass->turn.root.carbon*=scaler;
  grass->turn.root.nitrogen*=scaler;
  grass->turn.leaf.carbon*=scaler;
  grass->turn.leaf.nitrogen*=scaler;
  grass->turn_litt.leaf.carbon*=scaler;
  grass->turn_litt.leaf.nitrogen*=scaler;
  //pft->nind*=scaler;  only if nind is really used TODO rm root and leaf carbon/nitrogen turn.leaf here

  pft->bm_inc.carbon*=scaler;
  pft->bm_inc.nitrogen*=scaler;

  pft->establish.carbon*=scaler;
  pft->establish.nitrogen*=scaler;

  grass->excess_carbon*=scaler;
  grass->max_leaf*=scaler;
} /* of 'mix_veg_grass' */

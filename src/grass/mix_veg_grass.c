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
  //pft->nind*=scaler;

  grass->ind.leaf.carbon*=scaler;
  grass->ind.leaf.nitrogen*=scaler;
  grass->ind.root.carbon*=scaler;
  grass->ind.root.nitrogen*=scaler;
  grass->turn.root.carbon*=scaler;
  grass->turn.root.nitrogen*=scaler;
  grass->turn.leaf.carbon*=scaler;
  grass->turn.leaf.nitrogen*=scaler;

  grass->turn_litt.root.carbon*=scaler;
  grass->turn_litt.root.nitrogen*=scaler;
  grass->turn_litt.leaf.carbon*=scaler;
  grass->turn_litt.leaf.nitrogen*=scaler;
  //pft->nind*=scaler;  only if nind is really used TODO rm root and leaf carbon/nitrogen turn.leaf here
  pft->bm_inc.carbon*=scaler;
  pft->bm_inc.nitrogen*=scaler;
  pft->establish.carbon*=scaler;
  pft->establish.nitrogen*=scaler;

  grass->excess_carbon*=scaler;
} /* of 'mix_veg_grass' */

Bool mix_veg_stock_grass(Pft *pft1, Pft *pft2, Real frac1, Real frac2,const Config *config)  /*mix pft1 into pft2*/
{
  Pftgrass *grass1;
  Pftgrass *grass2;
  Bool isdead=FALSE;
  Real nind,test;
  grass1=pft1->data;
  grass2=pft2->data;
  nind=(pft1->nind*frac1+pft2->nind*frac2)/(frac1+frac2);
  test=pft1->nind*frac1+pft2->nind*frac2;
  if (test>epsilon)
  {
    pft1->bm_inc.carbon=(pft1->bm_inc.carbon*frac1+pft2->bm_inc.carbon*frac2)/(frac1+frac2);
    pft1->bm_inc.nitrogen=(pft1->bm_inc.nitrogen*frac1+pft2->bm_inc.nitrogen*frac2)/(frac1+frac2);
    pft1->establish.carbon=(pft1->establish.carbon*frac1+pft2->establish.carbon*frac2)/(frac1+frac2);
    pft1->establish.nitrogen=(pft1->establish.nitrogen*frac1+pft2->establish.nitrogen*frac2)/(frac1+frac2);

    grass1->excess_carbon=(grass1->excess_carbon*pft1->nind*frac1+grass2->excess_carbon*pft2->nind*frac2)/(pft1->nind*frac1+pft2->nind*frac2);
    grass1->ind.leaf.carbon=(grass1->ind.leaf.carbon*pft1->nind*frac1 + grass2->ind.leaf.carbon*pft2->nind*frac2)/(pft1->nind*frac1+pft2->nind*frac2);
    grass1->ind.root.carbon=(grass1->ind.root.carbon*pft1->nind*frac1 + grass2->ind.root.carbon*pft2->nind*frac2)/(pft1->nind*frac1+pft2->nind*frac2);
    grass1->ind.leaf.nitrogen=(grass1->ind.leaf.nitrogen*pft1->nind*frac1 + grass2->ind.leaf.nitrogen*pft2->nind*frac2)/(pft1->nind*frac1+pft2->nind*frac2);
    grass1->ind.root.nitrogen=(grass1->ind.root.nitrogen*pft1->nind*frac1 + grass2->ind.root.nitrogen*pft2->nind*frac2)/(pft1->nind*frac1+pft2->nind*frac2);
    grass1->turn.leaf.carbon=(grass1->turn.leaf.carbon*pft1->nind*frac1+grass2->turn.leaf.carbon*pft2->nind*frac2)/(pft1->nind*frac1+pft2->nind*frac2);
    grass1->turn.root.carbon=(grass1->turn.root.carbon*pft1->nind*frac1+grass2->turn.root.carbon*pft2->nind*frac2)/(pft1->nind*frac1+pft2->nind*frac2);
    grass1->turn.leaf.nitrogen=(grass1->turn.leaf.nitrogen*pft1->nind*frac1+grass2->turn.leaf.nitrogen*pft2->nind*frac2)/(pft1->nind*frac1+pft2->nind*frac2);
    grass1->turn.root.nitrogen=(grass1->turn.root.nitrogen*pft1->nind*frac1+grass2->turn.root.nitrogen*pft2->nind*frac2)/(pft1->nind*frac1+pft2->nind*frac2);
    grass1->turn_litt.leaf.carbon=(grass1->turn_litt.leaf.carbon*frac1+grass2->turn_litt.leaf.carbon*frac2)/(frac1+frac2);
    grass1->turn_litt.root.carbon=(grass1->turn_litt.root.carbon*frac1+grass2->turn_litt.root.carbon*frac2)/(frac1+frac2);
    grass1->turn_litt.leaf.nitrogen=(grass1->turn_litt.leaf.nitrogen*frac1+grass2->turn_litt.leaf.nitrogen*frac2)/(frac1+frac2);
    grass1->turn_litt.root.nitrogen=(grass1->turn_litt.root.nitrogen*frac1+grass2->turn_litt.root.nitrogen*frac2)/(frac1+frac2);
    pft1->nind=nind;
  }
  else
  {
    litter_update(&pft1->stand->soil.litter,pft1,pft1->nind,config);
    litter_update(&pft2->stand->soil.litter,pft2,pft2->nind,config);
    isdead=TRUE;
  }
  return isdead;
  //fpc_grass(pft1);
} /* of 'mix_veg_stock_grass' */

/**************************************************************************************/
/**                                                                                \n**/
/**               l i v e f u e l _ c o n s u m _ c r o p . c                      \n**/
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

Stocks livefuel_consum_crop(Litter * UNUSED(litter),Pft *pft,
                             const Fuel * UNUSED(fuel),
                             Livefuel *livefuel,
                             Bool *isdead,Real UNUSED(surface_fi),
                             Real fire_frac,
                             const Config * UNUSED(config)
                            )
{

  Pftcrop *crop;
  Stocks livefuel_consum,livefuel_consum_total;
  /*live crop consumption */
  crop=pft->data;
  livefuel_consum.carbon = livefuel_consum_total.carbon=fuel_consumption_1hr(livefuel->M[0]/livefuel->char_moisture,fire_frac)*crop->ind.leaf.carbon*pft->nind;
  livefuel_consum.nitrogen = livefuel_consum_total.nitrogen=fuel_consumption_1hr(livefuel->M[0]/livefuel->char_moisture,fire_frac)*crop->ind.leaf.nitrogen*pft->nind;
  /* live crop update */
  if (pft->nind > epsilon)
  {
    crop->ind.leaf.carbon -= livefuel_consum.carbon / pft->nind;
    crop->ind.leaf.nitrogen -= livefuel_consum.nitrogen / pft->nind;
  }
  livefuel_consum.carbon = fuel_consumption_1hr(livefuel->M[0]/livefuel->char_moisture,fire_frac)*crop->ind.so.carbon*pft->nind;
  livefuel_consum.nitrogen = fuel_consumption_1hr(livefuel->M[0]/livefuel->char_moisture,fire_frac)*crop->ind.so.nitrogen*pft->nind;
  if (pft->nind > epsilon)
  {
    crop->ind.so.carbon -= livefuel_consum.carbon / pft->nind;
    crop->ind.so.nitrogen -= livefuel_consum.nitrogen / pft->nind;
  }
  livefuel_consum_total.carbon+=livefuel_consum.carbon;
  livefuel_consum_total.nitrogen+=livefuel_consum.nitrogen;
  livefuel_consum.carbon = fuel_consumption_1hr(livefuel->M[0]/livefuel->char_moisture,fire_frac)*crop->ind.pool.carbon*pft->nind;
  livefuel_consum.nitrogen = fuel_consumption_1hr(livefuel->M[0]/livefuel->char_moisture,fire_frac)*crop->ind.pool.nitrogen*pft->nind;
  if (pft->nind > epsilon)
  {
    crop->ind.pool.carbon -= livefuel_consum.carbon / pft->nind;
    crop->ind.pool.nitrogen -= livefuel_consum.nitrogen / pft->nind;
  }
  livefuel_consum_total.carbon+=livefuel_consum.carbon;
  livefuel_consum_total.nitrogen+=livefuel_consum.nitrogen;
  pft->bm_inc.carbon-=livefuel_consum_total.carbon;
  pft->bm_inc.nitrogen-=livefuel_consum_total.nitrogen;
  *isdead=FALSE;
  return livefuel_consum_total;
} /* of 'livefuel_consum_crop' */

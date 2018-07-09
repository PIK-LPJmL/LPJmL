/**************************************************************************************/
/**                                                                                \n**/
/**               l i v e f u e l _ c o n s u m _ g r a s s . c                    \n**/
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

Stocks livefuel_consum_grass(Litter * UNUSED(litter),Pft *pft,
                             const Fuel * UNUSED(fuel),
                             Livefuel *livefuel,
                             Bool *isdead,Real UNUSED(surface_fi),
                             Real fire_frac)
{

  Pftgrass *grass;
  Stocks livefuel_consum;
  /*  livegrass consumption */
  grass=pft->data;
  livefuel_consum.carbon = fuel_consumption_1hr(livefuel->dlm_livegrass,fire_frac)*grass->ind.leaf.carbon*pft->nind;
  livefuel_consum.nitrogen = fuel_consumption_1hr(livefuel->dlm_livegrass,fire_frac)*grass->ind.leaf.nitrogen*pft->nind;
  /* livegrass update */
  if (pft->nind > 0)
  {
    grass->ind.leaf.carbon -= livefuel_consum.carbon / pft->nind;
    grass->ind.leaf.nitrogen -= livefuel_consum.nitrogen / pft->nind;
  }
  *isdead=FALSE;
  return livefuel_consum;
} /* of 'livefuel_consum_grass' */

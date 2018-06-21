/**************************************************************************************/
/**                                                                                \n**/
/**                      i  n  i  t  f  u  e  l  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Kirsten Thonicke                                                           \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void initfuel(Fuel *fuel)
{
  int i;
  fuel->char_moist_factor=fuel->char_alpha_fuel=fuel->char_net_fuel=0; 
  fuel->char_dens_fuel_ave=0.00001;
  fuel->cf=fuel->daily_litter_moist=0;   
  for(i=0;i<=NFUELCLASS;++i)
    fuel->deadfuel_consum[i]=0;
  fuel->gamma=0;
  fuel->moist_1hr=fuel->moist_10_100hr=fuel->mw_weight=1.0;
  fuel->sigma=0.00001; 
} /* of 'initfuel' */

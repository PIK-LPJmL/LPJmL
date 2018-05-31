/**************************************************************************************/
/**                                                                                \n**/
/**               f u e l _ c o n s u m p t i o n _ 1 h r . c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real fuel_consumption_1hr(Real moist_par,Real fire_frac)
{
  Real fuel_consum_1hr;

  if(moist_par <= 0.18) 
    fuel_consum_1hr = 1.0*(1.0-MINER_TOT)* fire_frac;
  else if(moist_par <= 0.73) 
    fuel_consum_1hr=(1.20-0.62*moist_par)*(1.0-MINER_TOT)*fire_frac;
  else if(moist_par<1.0) 
    fuel_consum_1hr=(2.45-2.45*moist_par)*(1.0-MINER_TOT)*fire_frac;
  else
    fuel_consum_1hr=0.0;
  return fuel_consum_1hr;
} /* of 'fuel_consumption_1hr' */

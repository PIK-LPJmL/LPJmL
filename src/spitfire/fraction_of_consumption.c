/**************************************************************************************/
/**                                                                                \n**/
/**            f r a c t i o n _ o f _ c o n s u m p t i o n . c                   \n**/
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

void fraction_of_consumption(Fuel *fuel)
{
  if(fuel->moist_1hr <= 0.18) 
    fuel->cf=1.0;  
  else if(fuel->moist_1hr <= 0.73) 
    fuel->cf=1.10-0.62*fuel->moist_1hr;
  else if(fuel->moist_1hr<1.0) 
    fuel->cf=2.45-2.45*fuel->moist_1hr;
  else 
    fuel->cf=0.0;

  /*  10hr fuel consumption */
  if(fuel->moist_10_100hr <= 0.12)
    fuel->cf+=1.0;
  else if(fuel->moist_10_100hr <= 0.51) 
    fuel->cf+=1.09-0.72*fuel->moist_10_100hr;
  else if(fuel->moist_10_100hr<1.0) 
    fuel->cf+=1.47-1.47*fuel->moist_10_100hr;

  /* 100hr fuel consumption */
  if(fuel->moist_10_100hr <= 0.38) 
    fuel->cf+=0.98-0.85*fuel->moist_10_100hr;
  else if(fuel->moist_10_100hr<=1.0) 
    fuel->cf+=1.06-1.06*fuel->moist_10_100hr;

  /* calculating Cf from fuel classes, tau_r=Cf/Gamma tau_l=2*tau_r */
  fuel->cf /= 3;  
} /* of 'fraction_of_consumption' */

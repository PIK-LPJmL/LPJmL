/**************************************************************************************/
/**                                                                                \n**/
/**               d e a d f u e l _ c o n s u m p t i o n . c                      \n**/
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

Real deadfuel_consumption(const Litter *litter, /* litter pools */
                          Fuel *fuel,
                          Real fire_frac /* fire fraction (0..1) */
                         )
{
  Real fuel_consum;
  int i,l;
 
  for(i=0;i<NFUELCLASS;i++)
    fuel->deadfuel_consum[i]=0;
    
    /* 1hr dead fuel consumption [gC/m2]*/
  fuel->deadfuel_consum[0] = fuel_consumption_1hr(fuel->moist_1hr,fire_frac);

   /*  10hr fuel consumption */
  if(fuel->moist_10_100hr <= 0.12)
    fuel->deadfuel_consum[1]=1.0*(1.0-MINER_TOT)*fire_frac;
  else if(fuel->moist_10_100hr <= 0.51) 
    fuel->deadfuel_consum[1]=(1.09-0.72* fuel->moist_10_100hr)*
                             (1.0-MINER_TOT)*fire_frac;
  else if(fuel->moist_10_100hr<1.0) 
    fuel->deadfuel_consum[1]=(1.47-1.47* fuel->moist_10_100hr)*
                               (1.0-MINER_TOT)*fire_frac;
  else
    fuel->deadfuel_consum[1]=0;

  /* 100hr fuel consumption */
  if(fuel->moist_10_100hr <= 0.38) 
    fuel->deadfuel_consum[2]=(0.98-0.85*fuel->moist_10_100hr)*(1.0-MINER_TOT)*fire_frac;
  else if(fuel->moist_10_100hr<1.0) 
    fuel->deadfuel_consum[2]=(1.06-1.06*fuel->moist_10_100hr)*(1.0-MINER_TOT)*fire_frac;
  else
    fuel->deadfuel_consum[2]=0.0;

  /*1000hr fuel consumption, not influencing rate of spread or I_surface (Rothermel 1972)*/
  /*Approximate form. No data. */
  fuel->deadfuel_consum[3]=(-0.8*fuel->mw_weight+0.8);

  /* total fuel consumption (without 1000hr fuel) in g Biomass per m2 i.e. "/0.45"!!!
   * Used to calculate fire intensity in the FLAMING FRONT.
   */
  fuel_consum=0;
  for(l=0;l<litter->n;l++)
  {
    fuel_consum += fuel->deadfuel_consum[0]*litter->ag[l].trait.leaf;
    for (i=0; i<NFUELCLASS-1;i++)
    {
      fuel_consum += fuel->deadfuel_consum[i]*litter->ag[l].trait.wood[i];
    }
  }   
  return c2biomass(fuel_consum);   
} /* of 'deadfuel_consumption' */

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

  /* These are empirical constants based on: Peterson, D. L. and Ryan, K. C.: Modeling
   * Postfire Conifer Mortality for Long-range Planning, Environ. Manage., 10, 797–808, 1986
   */

   /* 1hr dead fuel consumption [gC/m2]*/
  fuel->deadfuel_consum[0] = (fuel->char_moist_factor > 0) ? fuel_consumption_1hr(fuel->M[0]/fuel->char_moist_factor,fire_frac) : 0;

   /*  10hr fuel consumption */
  if(fuel->char_moist_factor > 0)
    if(fuel->M[1]/fuel->char_moist_factor <= 0.12)
      fuel->deadfuel_consum[1]=1.0*(1.0-MINER_TOT)*fire_frac;
    else if(fuel->M[1]/fuel->char_moist_factor <= 0.51)
      fuel->deadfuel_consum[1]=(1.09-0.72* fuel->M[1]/fuel->char_moist_factor)*
                               (1.0-MINER_TOT)*fire_frac;
    else if(fuel->M[1]/fuel->char_moist_factor<1.0)
      fuel->deadfuel_consum[1]=(1.47-1.47* fuel->M[1]/fuel->char_moist_factor)*
                               (1.0-MINER_TOT)*fire_frac;
    else
      fuel->deadfuel_consum[1]=0;
  else
    fuel->deadfuel_consum[1]=0;

  /* 100hr fuel consumption */
  if(fuel->char_moist_factor > 0)
    if(fuel->M[2]/fuel->char_moist_factor <= 0.38)
      fuel->deadfuel_consum[2]=(0.98-0.85*fuel->M[2]/fuel->char_moist_factor)*(1.0-MINER_TOT)*fire_frac;
    else if(fuel->M[2]/fuel->char_moist_factor<1.0)
      fuel->deadfuel_consum[2]=(1.06-1.06*fuel->M[2]/fuel->char_moist_factor)*(1.0-MINER_TOT)*fire_frac;
    else
      fuel->deadfuel_consum[2]=0.0;
  else
    fuel->deadfuel_consum[2]=0.0;
  /*1000hr fuel consumption, not influencing rate of spread or I_surface (Rothermel 1972)*/
  /*Approximate form. No data. */
  fuel->deadfuel_consum[3]=(-0.8*fuel->mw_weight+0.8)*(1.0-MINER_TOT)*fire_frac;

  /* total fuel consumption (without 1000hr fuel) in g Biomass per m2 i.e. "/0.45"!!!
   * Used to calculate fire intensity in the FLAMING FRONT.
   */
  fuel_consum=0;
  for(l=0;l<litter->n;l++)
  {
    fuel_consum += fuel->deadfuel_consum[0]*litter->item[l].agtop.leaf.carbon;
    for (i=0; i<NFUELCLASS-1;i++)
      fuel_consum += fuel->deadfuel_consum[i]*litter->item[l].agtop.wood[i].carbon;
  }
  return c2biomass(fuel_consum);
} /* of 'deadfuel_consumption' */

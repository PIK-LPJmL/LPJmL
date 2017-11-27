/**************************************************************************************/
/**                                                                                \n**/
/**                        r a t e o f s p r e a d . c                             \n**/
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

#define MINER_DAMP 0.41739
#define PART_DENS 513.0
#define heat_content_fuel 18000.0
  
Real rateofspread(Real windsp_cover,Fuel *fuel)
{
  Real beta_fire, beta_op, bet;
  Real q_ig, eps, a, b, c, e;
  Real wind_forward;
  Real phi_wind, xi, dummy, gamma_max, gamma_aptr;
  Real moist_damp, ir, U_front;

  /* Packing ratio*/
  beta_fire = fuel->char_dens_fuel_ave / PART_DENS;

  /* Optimum packing ratio */
  beta_op = 0.200395*pow(fuel->sigma,-0.8189);
  bet = (beta_op<=0) ? 0 : beta_fire / beta_op;
  /* Heat of pre-ignition */
  q_ig = 581.0 + 2594.0 * fuel->daily_litter_moist;

  /* Effective heating number */
  eps=(fuel->sigma <= 0.00001) ?  0 : exp(-4.528 / fuel->sigma);

  /* Parameters dependent of surface to volume ratio */
  a=8.9033*pow(fuel->sigma, -0.7913);
  b=0.15988*pow(fuel->sigma, 0.54);
  c=7.47*exp(-0.8711 * pow(fuel->sigma, 0.55));
  e=0.715*exp(-0.01094 * fuel->sigma);

  /* converts wind_speed (m/min) to ft/min
   * for input into Rothermel's formula for phi_wind in the ROS S/R */
  wind_forward=3.281*windsp_cover; 

  /* Effect of wind speed */
  phi_wind=(bet <= 0) ?  0 : c*pow(wind_forward,b)*pow(bet,-e); 
    
  /* Propagating flux ratio */
  if (fuel->sigma <= 0.00001)
    xi = 0.0;
  else  if ((0.792 + 3.7597 * (pow(fuel->sigma,0.5))) * (beta_fire + 0.1)>100)
    xi =exp(100) / (192 + 7.9095 * fuel->sigma);   /*TODO to avoid NAN*/
  else
    xi = (exp((0.792 + 3.7597 * (pow(fuel->sigma,0.5))) * (beta_fire + 0.1))) / (192 + 7.9095 * fuel->sigma);
  /*printf( "xi= %.5f \n",xi);*/

  /* Optimum Reaction Velocity */
  dummy=(fuel->sigma <= 0.0001) ?  0.0 : exp(a*(1.0-bet));
  gamma_max = 1.0/(0.0591+2.926*(pow(fuel->sigma,-1.5)));
  gamma_aptr=(bet <= 0) ?  0 : gamma_max*pow(bet,a)*dummy;

  /* Moisture dampening coefficient */
  moist_damp = (0.0 > (1.0-(2.59*fuel->mw_weight)+ (5.11*(pow(fuel->mw_weight,2.0)))-(3.52*(pow(fuel->mw_weight,3.0)))) ? 
    0 : (1.0-(2.59*fuel->mw_weight)+ (5.11*(pow(fuel->mw_weight,2.0)))-(3.52*(pow(fuel->mw_weight,3.0)))));

  /* Reaction intensity */
  ir=gamma_aptr * fuel->char_net_fuel * heat_content_fuel * moist_damp * MINER_DAMP;
  /* For use in post fire mortality */
  fuel->gamma = gamma_aptr * moist_damp * MINER_DAMP;

  /* Forward Rate of Spread */
  if (fuel->char_dens_fuel_ave <= 0 || eps <= 0 || q_ig <= 0)
    U_front = 0;
  else
    U_front=(ir * xi * (1.0 + phi_wind)) / (fuel->char_dens_fuel_ave * eps * q_ig);

  return U_front;
} /* of 'rateofspread' */

/**************************************************************************************/
/**                                                                                \n**/
/**                        r a t e o f s p r e a d . c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** Function calculates rate of spread using Rothermel/Albini model for use in     \n**/
/** fire size and mortality calculations                                           \n**/
/**                                                                                \n**/
/** Derived from                                                                   \n**/
/** Andrews, Patricia L. 2018. The Rothermel surface fire spread model and         \n**/
/** associated developments: A comprehensive explanation. Gen. Tech. Rep.          \n**/
/** RMRS-GTR-371. Fort Collins, CO: U.S. Department of Agriculture, Forest         \n**/
/** Service, Rocky Mountain Research Station. 121 p.                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define MINER_DAMP 0.41739        /* mineral damping factor,
                                    0.17*pow(Se,-0.19), Se=1% (effective mineral content) */
#define heat_content_fuel 18608.0 /* amount of heat released by fuel (kJ/kg) */
#define 

Real rateofspread(Real windsp_cover, /**< mid-flame wind speed (m/min) */
                  Fuel *fuel,        /**< fuel characteristics */
                  Livefuel *livefuel /**< live fuel characteristics */
                 )                   /** \return rate of spread (m/min) */
{
  Real beta_fire, beta_op, bet;
  Real a, b, c, e;
  Real wind_forward;
  Real phi_wind, xi, dummy, gamma_max, gamma_aptr;
  Real moist_damp_live,moist_damp_dead, ignition_rate, U_front;
  Real char_sigma_dead,char_sigma_live,wndead,wnlive,char_sigma;
  Real w,wsum,msum,wsum_live,mfdead,Mdead,Mlive;
  Real hs_sum_live,hs_sum_dead,hs,mw_weight_live;
  int i;

  /* ==== calculate characteristic values using weighting factors ==== */

  char_sigma_dead=0;
  wndead=0;
  char_sigma_live=0;
  wnlive=0;
  Mdead=0;
  for(i=0;i<NFUELCLASS;++i)
  {
    char_sigma_dead += fuel->f[i]*sigma_dead[i];
    wndead+=fuel->g[i]*fuel->w[i]*(1-MINER_TOT);
    Mdead += fuel->f[i]*fuel->M[i];
  }
  Mlive=0;
  for(i=0;i<2;++i)
  {
    char_sigma_live += livefuel->f[i]*sigma_live[i];
    wnlive+=livefuel->g[i]*livefuel->w[i]*(1-MINER_TOT);
    Mlive += livefuel->f[i]*livefuel->M[i];
  }
  char_sigma=livefuel->fi*char_sigma_live+fuel->fi*char_sigma_dead;
  wsum=msum=0;
  for(i=0;i<NFUELCLASS;i++)
  {
    wsum+=fuel->w[i]*exp(-138./ft2cm(sigma_dead[i]));
    msum+=fuel->M[i]*fuel->w[i]*exp(-138./ft2cm(sigma_dead[i]));
  }
  wsum_live=0;
  for(i=0;i<2;i++)
    if(sigma_live[i]>0)
      wsum_live+=livefuel->w[i]*exp(-500./ft2cm(sigma_live[i]));
  if(wsum_live>0)
  {
    w=wsum/wsum_live;
    mfdead=msum/wsum;
  }
  else
  {
    w=0;
    mfdead=0;
  }
  if(fuel->char_moist_factor>0)
    livefuel->char_moisture=max(2.9*w*(1-mfdead/fuel->char_moist_factor)-0.226,fuel->char_moist_factor);
  else
    livefuel->char_moisture=0;
  /* Packing ratio*/
  /* TODO: develop accurate fuel bulk density calculation */
  beta_fire = fuel->char_dens_fuel_ave / PART_DENS;

  /* ==== implementing Rothermel model  ==== */

  /* Optimum packing ratio */
  beta_op = (char_sigma <= 0.0001) ? 0.0 : 0.200395*pow(char_sigma,-0.8189);
  bet = (beta_op<=0.0001) ? 0 : beta_fire / beta_op;
  /* Parameters dependent of surface to volume ratio */
  a=(char_sigma <= 0.0001) ? 0.0 : 8.9033*pow(char_sigma, -0.7913);
  b=0.15988*pow(char_sigma, 0.54);
  c=7.47*exp(-0.8711 * pow(char_sigma, 0.55));
  e=0.715*exp(-0.01094 * char_sigma);

  /* Propagating flux ratio */
  if (char_sigma <= 0.00001)
    xi = 0.0;
  else  if ((0.792 + 3.7597 * (pow(char_sigma,0.5))) * (beta_fire + 0.1)>100)
    xi =exp(100) / (192 + 7.9095 * char_sigma);   /*TODO to avoid NAN*/
  else
    xi = (exp((0.792 + 3.7597 * (pow(char_sigma,0.5))) * (beta_fire + 0.1))) / (192 + 7.9095 * char_sigma);

  /* Optimum Reaction Velocity */
  dummy=(char_sigma <= 0.0001) ?  0.0 : exp(a*(1.0-bet));
  gamma_max = (char_sigma <= 0.0001) ? 1.0/(0.0591) : 1.0/(0.0591+2.926*(pow(char_sigma,-1.5)));
  gamma_aptr=(bet <= 0) ?  0 : gamma_max*pow(bet,a)*dummy;

  /* Moisture dampening coefficient */
  fuel->mw_weight=(fuel->char_moist_factor > 0) ? min(Mdead/fuel->char_moist_factor,1) : 1;
  moist_damp_dead = (0.0 > (1.0-(2.59*fuel->mw_weight)+ (5.11*(pow(fuel->mw_weight,2.0)))-(3.52*(pow(fuel->mw_weight,3.0)))) ?
    0 : (1.0-(2.59*fuel->mw_weight)+ (5.11*(pow(fuel->mw_weight,2.0)))-(3.52*(pow(fuel->mw_weight,3.0)))));
  mw_weight_live=(fuel->char_moist_factor > 0) ? min(Mlive/livefuel->char_moisture,1) : 1;
  moist_damp_live = (0.0 > (1.0-(2.59*mw_weight_live)+ (5.11*(pow(mw_weight_live,2.0)))-(3.52*(pow(mw_weight_live,3.0)))) ?
    0 : (1.0-(2.59*mw_weight_live)+ (5.11*(pow(mw_weight_live,2.0)))-(3.52*(pow(mw_weight_live,3.0)))));
  /* Reaction intensity */
  ignition_rate=gamma_aptr*(wndead*1e-3*heat_content_fuel*moist_damp_dead*MINER_DAMP
                 +wnlive*1e-3*heat_content_fuel*moist_damp_live*MINER_DAMP);
  /* For use in post fire mortality */
  fuel->ignition_rate =ignition_rate;


  /* converts wind_speed (m/min) to ft/min
  * for input into Rothermel's formula for phi_wind in the ROS S/R */
  wind_forward=3.281*windsp_cover;

  /* wind speed limit according to Patricia L. Andrews et al. 2012 */
  if(wind_forward > 96.8*pow(ignition_rate*0.0881,1.0/3.0))
    wind_forward = 96.8*pow(ignition_rate*0.0881,1.0/3.0);

  /* Effect of wind speed */
  phi_wind=(bet <= 0) ?  0 : c*pow(wind_forward,b)*pow(bet,-e);

  /* Heat sink term */
  hs_sum_dead=0;
  for(i=0;i<NFUELCLASS;i++)
    hs_sum_dead+=fuel->f[i]*exp(-138./ft2cm(sigma_dead[i]))*(581.0 + 2594.0 * fuel->M[i]);
  hs_sum_live=0;
  for(i=0;i<2;i++)
    if(sigma_live[i]>0)
      hs_sum_live+=livefuel->f[i]*exp(-138./ft2cm(sigma_live[i]))*(581.0 + 2594.0 * livefuel->M[i]);
  hs = fuel->char_dens_fuel_ave*(fuel->fi*hs_sum_dead+livefuel->fi*hs_sum_live);

  /* Forward Rate of Spread */
  if (hs <= 0)
    U_front = 0;
  else
    U_front=(ignition_rate * xi * (1.0 + phi_wind)) / hs;
  fuel->char_sigma=char_sigma;
  return U_front;
} /* of 'rateofspread' */

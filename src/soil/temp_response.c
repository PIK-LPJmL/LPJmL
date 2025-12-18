/**************************************************************************************/
/**                                                                                \n**/
/**                 t  e  m  p  _  r  e  s  p  o  n  s  e  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     DESCRIPTION                                                                \n**/
/**                                                                                \n**/
/**     Calculates g(T), response of respiration rate to temperature (T),          \n**/
/**     based on empirical relationship for temperature response of soil           \n**/
/**     temperature across ecosystems, incorporating damping of Q10                \n**/
/**     response due to temperature acclimation (Eqn 11, Lloyd & Taylor            \n**/
/**     1994)                                                                      \n**/
/**                                                                                \n**/
/**     r    = r10 * g(t)                                                          \n**/
/**     g(T) = EXP[308.56 * (1 / 56.02 - 1 / (T - 227.13))] (T in Kelvin)          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define e0 308.56 /* parameter in Arrhenius temp response function 308.56 (Sitch)*/
#define Q10 1.8
//#define USE_Q10

Real temp_response(Real temp, /**< air or soil temperature (deg C) */
                   Real temp_mean
                  )          /** \return respiration temperature response */
{

  Real gtemp;
#ifdef USE_Q10
  if (temp>30) temp = 30;
  gtemp = (temp >= -30.0) ? pow(Q10, (temp - temp_mean) / 10) : 0.0;
#else
  if (temp>40) temp = 40;
  gtemp= (temp>=-15.0) ? exp(e0*(1.0/(param.temp_response+10)-1.0/(temp+param.temp_response))) : 0.0;
#endif
  return gtemp;
} /* of 'temp_response' */

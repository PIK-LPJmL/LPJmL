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
#define e0 308.56   /* parameter in Arrhenius temp response function */

Real temp_response(Real temp /**< air or soil temperature (deg C) */
                  )          /** \return respiration temperature response */
{
  return (temp>=-40.0) ? exp(e0*(1.0/56.02-1.0/(temp+46.02))) : 0.0;
} /* of 'temp_response' */

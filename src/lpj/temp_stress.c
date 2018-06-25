/**************************************************************************************/
/**                                                                                \n**/
/**                t  e  m  p  _  s  t  r  e  s  s  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates temperature stress for photosynthesis                  \n**/
/**     Adapted from Farquhar (1982) photosynthesis model, as simplified           \n**/
/**     by  Collatz et al 1991, Collatz et al 1992 and Haxeltine &                 \n**/
/**     Prentice 1996                                                              \n**/ 
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define tmc3 45.0   /* maximum temperature for C3 photosynthesis (deg C) */
#define tmc4 55.0   /* maximum temperature for C4 photosynthesis (deg C) */

Real temp_stress(const Pftpar *pftpar, /**< PFT parameter */
                 Real temp,            /**< Temperature (deg C) */
                 Real daylength        /**< day length (h) */
                )                      /** \return temperature stress */
{
  Real low,high;
  if(daylength<0.01 || (pftpar->path==C3 && temp>tmc3) 
                    || (pftpar->path==C4 && temp>tmc4))
    return 0.0;
  if(temp<pftpar->temp_co2.high)
  {
    low=1/(1+exp(pftpar->k1*(pftpar->k2-temp)));
    high=1-0.01*exp(pftpar->k3*(temp-pftpar->temp_photos.high));
    return low*high;
  }
  return 0.0;
} /* of 'temp_stress' */

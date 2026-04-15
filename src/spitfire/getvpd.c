/**************************************************************************************/
/**                                                                                \n**/
/**               g  e  t  v  p  d  .  c                                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates vapour-pressure deficit from temperature and humidity  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define a -7.90298
#define b 5.02808
#define c -1.3816e-7
#define d 11.344
#define f 8.1328e-3
#define h 3.49149
#define Ts 373.16  /* water boiling point temperature in Kelvin Todo: dependend on altitude (pressure) */

Real getvpd(const Dailyclimate  *climate, /**< daily climate data */
            Bool relative_humidity        /**< humidity is relative humidity (TRUE/FALSE) */
           )                              /** \return VPD (Pa) */
{
  Real temperature, rh, Z;
  /*Goff and Gratch: coefficient z of saturation vapor pressure*/
  temperature = climate->temp + 273.16;
  Z =( a * (Ts/temperature -1) + b * log(Ts/temperature)/log(10.0) + c * (pow(10,pow(d,(1-(temperature/Ts))))-1) + f * (pow(10,-pow(h,(Ts/temperature)-1))-1));
  if(relative_humidity)
    rh=climate->humid;
  else
    rh= getrh(climate->temp,climate->humid);
  if (rh > 1)
    rh=1;
  return pow(10,Z) * (1-rh)*p_atm;
} /* of 'getvpd' */

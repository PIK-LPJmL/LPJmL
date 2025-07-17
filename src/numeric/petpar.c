/**************************************************************************************/
/**                                                                                \n**/
/**                   p  e  t  p  a  r  .  c                                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates daily photosynthetically active                        \n**/
/**     radiation flux, daylength and daily potential evapotranspiration           \n**/
/**     given temperature, sunshine percentage and latitude                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <math.h>
#include "types.h"
#include "date.h"
#include "hash.h"
#include "bstruct.h"
#include "numeric.h"
#include "units.h"

#define a 107.0
#define b 0.2
#define qoo 1360.0      /* solar constant (1360 W/m2) */
#define c 0.25
#define d 0.5
#define k 13750.98708   /* conversion factor from solar angular units to
                           seconds (12/pi*3600) */
#define dayseconds 86400        /* seconds per day */
/*
 *       Function petpar
 *
 *
 *       CALCULATION OF NET DOWNWARD SHORT-WAVE RADIATION FLUX
 *       (also known as insolation or incident solar radiation)
 *       Refs: Prentice et al 1993, Monteith & Unsworth 1990,
 *             Henderson-Sellers & Robinson 1886
 *
 *        (1) rs = (c + d*ni) * (1 - beta) * Qo * cos Z * k
 *              (Eqn 7, Prentice et al 1993)
 *        (2) Qo = Qoo * ( 1 + 2*0.01675 * cos ( 2*pi*i / 365) )
 *              (Eqn 8, Prentice et al 1993; angle in radians)
 *        (3) cos Z = sin(lat) * sin(delta) + cos(lat) * cos(delta) * cos h
 *              (Eqn 9, Prentice et al 1993)
 *        (4) delta = -23.4 * pi / 180 * cos ( 2*pi*(i+10) / 365 )
 *              (Eqn 10, Prentice et al 1993, angle in radians)
 *        (5) h = 2 * pi * t / 24 = pi * t / 12
 *
 *            where rs    = instantaneous net downward shortwave radiation
 *                          flux (W/m2 = J/m2/s)
 *                  c, d  = empirical constants (c+d = clear sky
 *                          transmissivity)
 *                  ni    = proportion of bright sunshine
 *                  beta  = average 'global' value for shortwave albedo
 *                         (not associated with any particular vegetation)
 *                  i     = julian date, (1-365, 1=1 Jan)
 *                  Qoo   = solar constant, 1360 W/m2
 *                 Z     = solar zenith angle (angular distance between the
 *                          sun's rays and the local vertical)
 *                  k     = conversion factor from solar angular units to
 *                          seconds, 12 / pi * 3600
 *                  lat   = latitude (+=N, -=S, in radians)
 *                  delta = solar declination (angle between the orbital
 *                         plane and the Earth's equatorial plane) varying
 *                          between +23.4 degrees in northern hemisphere
 *                          midsummer and -23.4 degrees in N hemisphere
 *                          midwinter
 *                  h     = hour angle, the fraction of 2*pi (radians) which
 *                          the earth has turned since the local solar noon
 *                  t     = local time in hours from solar noon
 *
 *       From (1) and (3), shortwave radiation flux at any hour during the
 *       day, any day of the year and any latitude given by
 *        (6) rs = (c + d*ni) * (1 - beta) * Qo * ( sin(lat) * sin(delta) +
 *                 cos(lat) * cos(delta) * cos h ) * k
 *       Solar zenith angle equal to -pi/2 (radians) at sunrise and pi/2 at
 *       sunset.  For Z=pi/2 or Z=-pi/2,
 *        (7) cos Z = 0
 *       From (3) and (7),
 *        (8)  cos hh = - sin(lat) * sin(delta) / ( cos(lat) * cos(delta) )
 *             where hh = half-day length in angular units
 *       Define
 *        (9) u = sin(lat) * sin(delta)
 *       (10) v = cos(lat) * cos(delta)
 *       Thus
 *       (11) hh = acos (-u/v)
 *       To obtain the net DAILY downward short-wave radiation flux, integrate
 *       equation (6) from -hh to hh with respect to h,
 *       (12) rs_day = 2 * (c + d*ni) * (1 - beta) * Qo *
 *                     ( u*hh + v*sin(hh) ) * k
 *       Define
 *       (13) w = (c + d*ni) * (1 - beta) * Qo
 *       From (12) & (13),
 *         (14) rs_day = 2 * w * ( u*hh + v*sin(hh) ) * k
 */

#define gamma_t(temp)  (65.05+temp*0.064)
#define lambda(temp) (2.495e6-temp*2380)

void petpar(Real *daylength, /**< daylength (h) */
            Real *par,       /**< photosynthetic active radiation flux */
            Real *eeq,       /**< equilibrium evapotranspiration */
            Real *swdown,    /**< downwards shortwave radiation (W/m2) */
            Real lat,        /**< latitude (deg) */
            int day,         /**< day (1..365) */
            Real temp,       /**< temperature (deg C) */
            Real sun,        /**< sunshine (%) */
            Real beta        /**< Albedo */
           )
{
  Real delta,u,v,hh,w,s;
  sun*=0.01;
  delta=deg2rad(-23.4*cos(2*M_PI*(day+10.0)/NDAYYEAR));
  u=sin(deg2rad(lat))*sin(delta);
  v=cos(deg2rad(lat))*cos(delta);
  w=(c+d*sun)*(1-beta)*qoo*(1.0+2.0*0.01675*cos(2.0*M_PI*day/NDAYYEAR)); /*net short wave*/
  if(u>=v)
  {
    *daylength=24;
    *par=w/(1-beta)*u*M_PI*k;
  }
  else if(u<=-v)
    *daylength=*par=0;
  else
  {
    hh=acos(-u/v);
    *par=w/(1-beta)*(u*hh+v*sin(hh))*k;
    *daylength=24*hh*M_1_PI;
  }
  *swdown=2**par/dayseconds; /* taken from line 65 in petpar2.c  NOTE beta is NOT included in par*/
  u=w*u-(b+(1-b)*sun)*(a-temp); /* (b+(1-b)*sun)*(a-temp) eq. 11 Haxeltine & Prentice 1993 lw net*/
  v*=w;
  if(u<=-v) /*polar night*/
    *eeq=0;
  else
  {
    s=2.503e6*exp(17.269*temp/(237.3+temp))/
            ((237.3+temp)*(237.3+temp));
    if(u>=v)
      *eeq=2*(s/(s+gamma_t(temp))/lambda(temp))*u*M_PI*k;
    else
    {
      hh=acos(-u/v);
      *eeq=2*(s/(s+gamma_t(temp))/lambda(temp))*(u*hh+v*sin(hh))*k;
    }
  }
} /* of 'petpar' */

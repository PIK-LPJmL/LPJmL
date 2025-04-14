/**************************************************************************************/
/**                                                                                \n**/
/**                   p  e  t  p  a  r  3  .  c                                    \n**/
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

#include <stdio.h>
#include <math.h>
#include "types.h"
#include "date.h"
#include "bstruct.h"
#include "numeric.h"
#include "units.h"

#define dayseconds 86400   /* seconds per day */
#define a 107.0
#define b 0.2
#define qoo 1376.0         /* solar constant (1360 W/m2) */
#define c 0.25
#define d 0.5
#define k 13750.98708      /* conversion factor from solar angular units to
                              seconds (12/pi*3600) */

#define gamma_t(temp)  (65.05+temp*0.064)
#define lambda(temp) (2.495e6-temp*2380)

void petpar3(Real *daylength, /**< daylength (h) */
             Real *par,       /**< photosynthetic active radiation flux */
             Real *eeq,       /**< equilibrium evapotranspiration */
             Real lat,        /**< latitude (deg) */
             int day,         /**< day (1..365) */
             Real temp,       /**< temperature (deg C) */
             Real swdown,     /**< shortwave downward flux (W m-2) */
             Real beta        /**< albedo */
            )
{
  Real swnet,sun,lw_net;
  Real delta,u,v,hh,s,w;
  delta=deg2rad(-23.4*cos(2*M_PI*(day+10.0)/NDAYYEAR));
  u=sin(deg2rad(lat))*sin(delta);
  v=cos(deg2rad(lat))*cos(delta);
  swnet=(1-beta)*swdown; /* shortwave net flux, downward positive (W m-2) */
  if(u>=v)
  {
    *daylength=24;
    w=swnet*dayseconds/(2*(u*M_PI)*k);
  }
  else if(u<=-v)
  {
    w=0;
    *daylength=0;
  }
  else
  {
    hh=acos(-u/v);
    *daylength=24*hh*M_1_PI;
    w=swnet*dayseconds/(2*(u*hh+v*sin(hh))*k);
  }
  sun=((w/((1-beta)*qoo*(1.0+2.0*0.01675*cos(2.0*M_PI*day/365))))-c)/d;
  if(sun<0)
    sun=0;
  else if (sun>1)
    sun=1;
  lw_net=(b+(1-b)*sun)*(a-temp)*(*daylength)/24*k;

  //*par=dayseconds*swnet/2;
  *par=dayseconds*swdown/2; /* PAR based on SWdown instead of SWnet because albedo will be removed with PFT-dependent leaf albedo in water_stressed */
  s=2.503e6*exp(17.269*temp/(237.3+temp))/
          ((237.3+temp)*(237.3+temp));
  *eeq=2*(s/(s+(65.05+temp*0.064))/(2.495e6-temp*2380))*(swnet*dayseconds/2-lw_net);
  if(*eeq<0)
    *eeq=0;
} /* of 'petpar3' */


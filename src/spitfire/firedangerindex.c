/**************************************************************************************/
/**                                                                                \n**/
/**               f i r e d a n g e r i n d e x . c                                \n**/
/**                                                                                \n**/
/**     Function calculates fire dange index                                       \n**/
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

//#define alpha_fuelp 0.000337

#define a -7.90298
#define b 5.02808
#define c -1.3816e-7
#define d 11.344
#define f 8.1328e-3
#define h 3.49149
#define Ts 373.16  /* water boiling point temperature in Kelvin Todo: dependend on altitude (pressure) */
#define cR 2   /* day/mm */

Real firedangerindex(Real char_moist_factor,
                     Real nesterov_accum,
                     const Pftlist *pftlist, /**< PFT list */
		     Real humidity,          /**< specific humidity (kg/kg) */
		     Real avgprec,           /**< monthly averaged precipitation (mm/day) */
		     int fid,                /**< fire danger index method (NESTEROV_INDEX,WVPD_INDEX) */
		     Real temp               /**< air temperature (Celsius) */
                    )                        /** \return fire danged index */
{
  Real d_fdi,alpha_fuelp_ave,fpc_sum=0;
  Real temperature, RH, VD, R, Z, vpd_sum;

  const Pft *pft;
  int p,n;
  n=getnpft(pftlist);
  switch(fid)
  {
    case NESTEROV_INDEX:
      alpha_fuelp_ave=0;
      if(n>0)
      {
        foreachpft(pft,p,pftlist)
          alpha_fuelp_ave+=pft->par->alpha_fuelp;
        alpha_fuelp_ave/=n;
      } 
      /* Calculate Fire Danger Index */
      if(nesterov_accum <= 0 || char_moist_factor <=0)
        d_fdi = 0;
      else
        d_fdi = (0.0 > (1.0-(1.0 / char_moist_factor * (exp(-alpha_fuelp_ave * nesterov_accum)))) ?
                 0 : (1.0-(1.0 / char_moist_factor * (exp(-alpha_fuelp_ave * nesterov_accum)))));
      break;
    case WVPD_INDEX:
      vpd_sum=0;
      fpc_sum=0;

      /*Goff and Gratch: coefficient z of saturation vapor pressure*/
      temperature = temp + 273.16;
      Z =( a * (Ts/temperature -1) + b * log(Ts/temperature) + c * (pow(10,pow(d,(1-(Ts/temperature))))-1) + f * (pow(10,-pow(h,(Ts/temperature)-1))-1));
  
      /*conversion of specific humidity to relative humidity*/
      RH= 0.263 * 1013.25 * humidity *1/(exp(17.67*temp/(temperature-29.65)));
  
      /* average precipitation over one month to avoid unrealistically high flammability fluctuations in time steps with very low or zero precipitation */
       R = avgprec; /* letzten monat aufsummieren und durch num month teilen (units: mm/day) */
       if (RH > 1)
         RH = 1;
  
      /*calculation of vegetation density and average alpha_fuelp as skaling factor for VPD*/
      if(n>0)
      {
        foreachpft(pft,p,pftlist)
        {
          vpd_sum+=pft->par->vpd_par*pft->fpc;
          fpc_sum+=pft->fpc;
        }
        vpd_sum/=fpc_sum;
      }
      VD = fpc_sum; /* todo implement lai or fpc?*/
   
      /*calculation of Vapor Pressure Deficite (VPD) */
      d_fdi = pow(10,Z) * (1-RH) * VD * exp(-cR * R);
      d_fdi*= vpd_sum;
      break;
  }  /* of 'switch' */
  return d_fdi;
} /* of 'firedangerindex' */

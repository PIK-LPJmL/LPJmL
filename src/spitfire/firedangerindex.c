/**************************************************************************************/
/**                                                                                \n**/
/**               f i r e d a n g e r i n d e x . c                                \n**/
/**                                                                                \n**/
/**     Function calculates fire danger index                                      \n**/
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

#define cR 2   /* day/mm */

Real firedangerindex(Real char_moist_factor,
                     const Stand *stand,           /**< pointer to stand */
                     const Dailyclimate  *climate, /**< daily climate data */
                     Real avgprec,                 /**< monthly averaged precipitation (mm/day) */
                     int fid,                      /**< fire danger index method (NESTEROV_INDEX,WVPD_INDEX) */
                     Bool relative_humidity        /**< humidity is relative humidity (TRUE/FALSE) */
                    )                              /** \return fire danger index (0..1) */
{
  Real d_fdi,alpha_fuelp_ave,fpc_sum=0;
  Real VD, vpd_sum;
  Real nesterov_accum;
  const Pft *pft;
  int p,n;
  n=getnpft(&stand->pftlist);
  switch(fid)
  {
    case NESTEROV_INDEX:
      nesterov_accum=stand->cell->ignition.nesterov_max;
      alpha_fuelp_ave=0;
      if(n>0)
      {
        foreachpft(pft,p,&stand->pftlist)
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

      /*calculation of vegetation density and average alpha_fuelp as skaling factor for VPD*/
      if(n>0)
      {
        foreachpft(pft,p,&stand->pftlist)
        {
          vpd_sum+=pft->par->vpd_par*pft->fpc;
          fpc_sum+=pft->fpc;
        }
        vpd_sum/=fpc_sum;
      }
      VD = fpc_sum; /* todo implement lai or fpc?*/
   
      /*calculation of Vapor Pressure Deficite (VPD) */
      d_fdi = getvpd(climate,relative_humidity)/p_atm * VD * exp(-cR * avgprec);
      d_fdi*= vpd_sum;
      d_fdi = min(d_fdi,1);
      break;
    default:
      d_fdi=0;
      fail(INVALID_FIRE_INDEX_ERR,TRUE,TRUE,"Invalid fire index %d",fid);
  }  /* of 'switch' */
  return d_fdi;
} /* of 'firedangerindex' */

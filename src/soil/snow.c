/**************************************************************************************/
/**                                                                                \n**/
/**                    s  n  o  w  .  c                                            \n**/
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

#define km 3.0
#define c_roughness 0.06 /* Roughness height of vegetation below the canopy. Source: FOAM/LPJ */

/*
 *    Function snow
 *
 *    Adjust daily precipitation by snowmelt and accumulation in snowpack
 *    Ref: Haxeltine & Prentice 1996
 *
 */

Real snow_old(Real *snowpack, /**< snowpack depth (mm) */
              Real *prec,     /**< Precipitation (mm) */
              Real *snowmelt, /**< snowmelt (mm) */
              Real temp       /**< temperature (deg C) */
             )                /** \return runoff (mm) */
{
  Real runoff=0;

  if(temp<tsnow)
  {
    *snowpack+=*prec;
    if(*snowpack>param.maxsnowpack)
    {
      runoff=(*snowpack)-param.maxsnowpack;
      *snowpack=param.maxsnowpack;
    }
    *snowmelt=*prec=0.0;
  }
  else
  {
    /* *snowmelt=km*(temp-tsnow);*/
    /* following Gerten et al. 2004 */
    *snowmelt=(1.5+0.007**prec)*(temp-tsnow);
    if(*snowmelt>*snowpack)
      *snowmelt=*snowpack;
    *snowpack-=*snowmelt;
  }
  return runoff;
} /* of 'snow_old' */

Real snow(Soil *soil,       /**< pointer to soil data */
          Real *prec,       /**< Precipitation (mm) */
          Real *snowmelt,   /**< snowmelt (mm) */
          Real temp,        /**< air temperature (deg C) */
          Real *evap        /**< evaporation (mm) */
         )                  /** \return runoff (mm) */
{
  Real runoff=0;
  Real melt_heat,      /*[J/m2]*/
       dT,             /* change in temperature [K]*/
       heatflux,
       depth;
  Real frsg;  /* Snow cover of the ground below the canopy (0-1) */
  Real HS;    /* Height of the Snow (m) */

  *snowmelt=0.0;
  /* precipitation falls as snow */
  if(temp<tsnow)
  {
    soil->snowpack+=*prec;
    if(soil->snowpack>param.maxsnowpack)
    {
      runoff=soil->snowpack-param.maxsnowpack;
      soil->snowpack=param.maxsnowpack;
    }
    *prec=0.0;
  }
  /* sublimation of snow */
  if(soil->snowpack > 0.1)
  {
    soil->snowpack -= 0.1;
    *evap = 0.1;
  }
  else
    *evap=0;
  /* snow layer is insulating */
  if(soil->snowpack>epsilon)
  {
    /* TODO: temp_bsnow should be a composite from snow / soil to avoid steps, see SPEEDY version*/
    /* TODO: incorporate snow density for dt and dT?*/
    /* melting at the top */
    if(temp > T_zero)
    {
      /* TODO: snow-T to 0 before melting */
      depth=min(soil->snowpack,snow_skin_depth);
      dT=th_diff_snow*timestep2sec(1.0,NSTEP_DAILY)/(depth*depth)*1000000.0
         *(temp-tsnow);
      heatflux=lambda_snow*(tsnow-T_zero+dT)/depth*1000;
      melt_heat=min(heatflux*timestep2sec(1.0,NSTEP_DAILY),depth*1e-3*c_water2ice);/*[J/m2]*/
      *snowmelt+=melt_heat/c_water2ice*1000; /* [J/m2]/[J/m3]*1000 = [mm] */
      soil->snowpack-=melt_heat/c_water2ice*1000;
      if (soil->snowpack<epsilon)
      {
        soil->snowpack=0.0;
        soil->snowheight=0.0;
        soil->snowfraction=0.0;
        return runoff;
      }
    }
  } /* snow present?*/

  /* calculate snow height and fraction of snow coverage */
  if(soil->snowpack>epsilon)
  {
    HS = c_watertosnow * (soil->snowpack/1000.0); /* mm -> m */
    frsg = HS / (HS+0.5*c_roughness);

    soil->snowheight = HS;
    soil->snowfraction = frsg;
  }
  else
  {
    soil->snowpack=0.0;
    soil->snowheight=0.0;
    soil->snowfraction=0.0;
  }

  return runoff;
} /* of 'snow' */

/**************************************************************************************/
/**                                                                                \n**/
/**                 s  o  i  l  c  o  n  d  u  c  t  .  c                          \n**/
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

#define JOHANSEN
#define K_DRY 0.237 /* assuming a bulk density of 1600 kg/m3, following Farouki (1981)*/
#define K_SOLID 8
#define K_ICE 2.2
#define K_WATER 0.57

Real soilconduct(const Soil *soil, /**< pointer to soil data */
                 int layer         /**< soil layer */
                )                  /** \return soil conductivity (W/m/K) */
{
  Real conduct;
  /* assuming a linear dependence of conductivity on water / ice content beyond 
   * the permanent wilting point and additivity of the water / ice component;
   * see E.D. Yershov: General Geocryology (1998) and
   * G.S. Campbell / J.M. Norman: An Introduction to Environmental Biophysics,
   * 2nd Ed. (1998)for further reference and a more accurate treatment
   */
#ifdef JOHANSEN
   Real k_sat,sat,ke,
        por;
   por=soil->par->wsats[layer]/soildepth[layer];  /*porosity*/
   k_sat = pow(K_SOLID,(1-por))*pow(K_ICE,(por*soil->freeze_depth[layer]/soildepth[layer]))
           *pow(K_WATER,(por*(1-soil->freeze_depth[layer]/soildepth[layer])));
   /* saturation */
   sat=(soil->w[layer]*soil->par->whcs[layer]+soil->ice_depth[layer]+soil->par->wpwps[layer]
        +soil->w_fw[layer]+soil->ice_fw[layer])/soil->par->wsats[layer];
   /*Kersten number*/
   ke=(soil->freeze_depth[layer]>epsilon || sat < 0.1) ? sat : log10(sat)+1;
   conduct = (k_sat-soil->par->k_dry[layer])*ke+soil->par->k_dry[layer];
#else
  Real conduct_w,conduct_ice;
  conduct_w=(soil->w[layer]*soil->par->whcs[layer]+soil->w_fw[layer]+soil->par->wpwps[layer])
             /(soil->par->wsats[layer])*(soil->par->tcond_100-soil->par->tcond_pwp);
  conduct_ice=(soil->ice_depth[layer]+soil->ice_fw[layer]+soil->par->wpwps[layer])/soil->par->wsats[layer]*(soil->par->tcond_100_ice-soil->par->tcond_pwp);
  conduct = soil->par->tcond_pwp + conduct_w + conduct_ice;
#endif
  return conduct;
} /* of 'soilconduct' */

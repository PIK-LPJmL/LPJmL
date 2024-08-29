/**************************************************************************************/
/**                                                                                \n**/
/**                    freezefrac2soil.c                                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void freezefrac2soil(Soil *soil,                       /**< pointer to soil to be modified */
                     const Real freezfrac[NSOILLAYER]  /**< vector with a fraction for each layer that is frozen */
                    )
{
  int layer;

  Real allwaterice;   /* total water+ice content */
  Real a_wi;          /* plant available soil water+ice [mm] */
  Real ua_wi;         /* water+ice below wilting point [mm] */
  Real f_wi;          /* free soil water+ice [mm] */
  Real ice_target;    /* target ice content [mm] */
  Real rest_ice;      /* target ice to freeze in f_wi [mm] */
  Real a_ua_ice_frac; /* target fraction of ice in a_wi and ua_wi */
  Real f_ice_frac;    /* target fraction of ice in f_wi */

  for(layer=0; layer<NSOILLAYER; ++layer)
  {
    soil->freeze_depth[layer] = freezfrac[layer] * soildepth[layer];
    
    /* get the absolute quantities */
    allwaterice = allwater(soil,layer) + allice(soil,layer);
    if(allwaterice<epsilon)
      continue; /* without any water there is nothing to do */
    a_wi  = soil->w[layer] * soil->whcs[layer] + soil->ice_depth[layer];
    ua_wi  = soil->wpwps[layer];
    f_wi  = soil->w_fw[layer] + soil->ice_fw[layer];

    /* get target ice content */
    ice_target = freezfrac[layer] * allwaterice;

    /* The target ice is distributed first to a_wi and ua_wi.
     * Only if the target ice is higher than the sum of a_wi and ua_wi is the rest distributed to f_wi.
     * 
     * This guarantees that free ice can only occur when all other water is also frozen,
     * which means that water losses due to percolation, evaporation, and transpiration — which only affect liquid water — 
     * will always first remove all free water (of any phase) before reducing plant-available water (of any phase).
     * Thus, at any point, free water (of any phase) can only exist if the total amount of water (of any phase) is above field capacity. */

    a_ua_ice_frac = min(1, ice_target / (a_wi + ua_wi)); /* fraction of ice in a_wi and ua_wi */
    if(ice_target > a_wi + ua_wi)
    {
      rest_ice = ice_target - a_wi - ua_wi; /* remaining ice to freeze */
      f_ice_frac = rest_ice / f_wi; /* fraction of ice in f_wi */
    }
    else
      f_ice_frac = 0;

    /* set new soil water and ice variables */
    soil->ice_depth[layer] = a_ua_ice_frac * a_wi;
    soil->w[layer] = (1 - a_ua_ice_frac) * a_wi / soil->whcs[layer];
    soil->ice_pwp[layer] = a_ua_ice_frac;
    soil->ice_fw[layer] = f_ice_frac * f_wi;
    soil->w_fw[layer] = (1 - f_ice_frac) * f_wi;
  }
} /* of 'freezefrac2soil' */

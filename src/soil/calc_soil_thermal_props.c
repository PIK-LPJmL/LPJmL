/**************************************************************************************/
/**                                                                                \n**/
/**                    calc_soil_thermal_props.c                                   \n**/
/**                                                                                \n**/
/** The function calculates thermal properties of the soil for the different       \n**/
/** vertical layers. It returs volumetric heat capacity, and thermal conductivity  \n**/
/** for the soil in frozen and unfrozen state based on soil texture and water      \n**/
/** content. For the conductivity it uses the approach described by                \n**/
/** Johansen (1977)                                                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void calc_soil_thermal_props(Uniform_temp_sign uniform_temp_sign,  /**< flag to indicate if the temperatures are all positive all negative or mixed */
                             Soil_thermal_prop *th,                     /**< Soil thermal property structure that is set or modified */
                             const Soil *soil,                          /**< Soil structure from which water content etc is obtained  */
                             const Real *waterc_abs,                    /**< Absolute total water content of soillayers (including ice) */
                             const Real *solc_abs,                      /**< Absolute total content of solids of soillayers*/
                             Bool johansen,                             /**< Flag to activate johansen method */
                             Bool with_conductivity                     /**< Flag to activate conductivity update  */
                            )
{
  int  layer, j, gp;
  Real c_froz, c_unfroz;             /* frozen and unfrozen heat capacities */
  Real lam_froz, lam_unfroz;         /* frozen and unfrozen conductivities */
  Real latent_heat;                  /* latent heat of fusion depending on water content */
  Real lam_sat_froz, lam_sat_unfroz; /* frozen and unfrozen conductivities for saturated soil */
  Real waterc_abs_layer;             /* total absolute water content of soil */
  Real solidc_abs_layer;             /* total absolute solid content of soil */
  Real sat;                          /* saturation of soil */
  Real ke_unfroz, ke_froz;           /* kersten number for unfrozen and frozen soil */
  Real por;                          /* porosity of the soil */
  Bool calc_frozen_values;
  Bool calc_unfrozen_values;

  /* thermal resistance from last (resp first) layer to layer border*/
  Real resistance_froz_prev=0, resistance_froz_cur=0,resistance_unfroz_prev=0,resistance_unfroz_cur=0;
  Real prev_length_to_border =0, cur_length_to_border;
  Real tmp;                          /* temporary variable */

  if(uniform_temp_sign == ALL_BELOW_0)
    calc_unfrozen_values = FALSE;
  else
    calc_unfrozen_values = TRUE;

  if(uniform_temp_sign == ALL_ABOVE_0)
    calc_frozen_values = FALSE;
  else
    calc_frozen_values = TRUE;

  for (layer = 0; layer < NSOILLAYER; ++layer)
  {

    /* get absolute water and solid content of soil */
    if(waterc_abs == NULL)
    {
      waterc_abs_layer = allwater(soil,layer)+allice(soil,layer);
    }
    else
      waterc_abs_layer = waterc_abs[layer];

    if(solc_abs == NULL)
      solidc_abs_layer = soildepth[layer] - soil->wsats[layer];
    else
      solidc_abs_layer = solc_abs[layer];

    if(with_conductivity)
    {
      if(soil->wsats[layer]<epsilon)
        sat = 0;
      else
        sat = waterc_abs_layer / soil->wsats[layer];
      if (johansen)
      {
        /* get frozen and unfrozen conductivity with johansens approach */
        por            = soil -> wsat[layer];
        tmp =  K_SOLID_LOG* (1 - por);
        if(calc_frozen_values) 
          lam_sat_froz   = pow(10, tmp + K_ICE_LOG * por); /* geometric mean  */
        if(calc_unfrozen_values)
          lam_sat_unfroz = pow(10, tmp + K_WATER_LOG * por);
        ke_unfroz  = (sat < 0.1 ? 0 : log10(sat) + 1); /* fine soil parametrisation of Johansen */
        ke_froz    =  sat;
        if(calc_frozen_values)
          lam_froz   = (lam_sat_froz   - soil->k_dry[layer]) * ke_froz   + soil->k_dry[layer];
        if(calc_unfrozen_values)
          lam_unfroz = (lam_sat_unfroz - soil->k_dry[layer]) * ke_unfroz + soil->k_dry[layer];
      }
      else
      {
        lam_froz = sat * soil->par->tcond_100_ice + (1 - sat) * soil->par->tcond_pwp;
        lam_unfroz = sat * soil->par->tcond_100 + (1 - sat) * soil->par->tcond_pwp;
      }
    }
    /* get frozen and unfrozen volumetric heat capacity */
    c_froz   = (c_mineral * solidc_abs_layer + c_ice   * waterc_abs_layer) / soildepth[layer];
    c_unfroz = (c_mineral * solidc_abs_layer + c_water * waterc_abs_layer) / soildepth[layer];

    /* get volumetric latent heat   */
    latent_heat = waterc_abs_layer / soildepth[layer] * c_water2ice;

    /* calculate thermal resistance of parts of the top and bottom elements that belong to current to layer */
    /* (assume that the distance from last gridpoint to bottom layer border is the same as from first gridpoint to top layer border) */
    if(with_conductivity)
    {
      cur_length_to_border = (soildepth[layer]/1000)/(GPLHEAT*2);
      if(calc_frozen_values)
        resistance_froz_cur =  cur_length_to_border/lam_froz;
      if(calc_unfrozen_values)
        resistance_unfroz_cur = cur_length_to_border/lam_unfroz;
    }
    for (j = 0; j < GPLHEAT; ++j)
    { /* iterate through gridpoints of the layer */
      gp = GPLHEAT * layer + j;
      if(with_conductivity)
      {
        /* set properties of j-th layer element */
        /* maximum element refernced = GPLHEAT*(NSOILLAYER-1)+GPLHEAT-1 = NHEATGRIDP-1 */
        if(j==0)
        {
          /* element crosses layers, hence thermal resistance is used to calculate conductivity of element */
          /* (make use of the fact that the resistance of a piece of composite material is the sum of the resitances of the pieces)*/
          if(calc_frozen_values)
            th->lam_frozen[gp]   =
            (prev_length_to_border+cur_length_to_border)/(resistance_froz_cur+resistance_froz_prev);
          if(calc_unfrozen_values)
            th->lam_unfrozen[gp] =
            (prev_length_to_border+cur_length_to_border)/(resistance_unfroz_cur+resistance_unfroz_prev);
        }
        else
        {
          if(calc_frozen_values)
            th->lam_frozen[gp]   = lam_froz;
          if(calc_unfrozen_values)
            th->lam_unfrozen[gp] = lam_unfroz;
        }

      }
      /* set properties of j-th layer gridpoint */
      th->c_frozen [gp]    = c_froz;
      th->c_unfrozen[gp]   = c_unfroz;
      th->latent_heat[gp]  = latent_heat;
    }
    if(with_conductivity)
    {
      /* save thermal resistance of part of last element of current layer for next iteration */
      resistance_froz_prev=resistance_froz_cur;
      resistance_unfroz_prev=resistance_unfroz_cur;
      prev_length_to_border=cur_length_to_border;
    }
  }
} /* of 'calc_soil_thermal_props' */

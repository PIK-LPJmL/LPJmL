/**************************************************************************************/
/**                                                                                \n**/
/**              s  o  i  l  h  e  a  t  c  a  p  .  c                             \n**/
/**                                                                                \n**/
/**     Computation of heat capacity of the upper soil layer taking into           \n**/
/**     account the amount of water and ice in the soil.                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real soilheatcap(const Soil *soil, /**< pointer to soil data */
                 int layer         /**< soil layer */
                )                  /** \return heat capacity (J/m2/K) */
{
  Real water_layer, ice_layer, dry_soil_layer; /* Layers in millimeter */
  Real heatcap; /* heat capacity of the soil (J/m2) */
  /* plant available water and water below permanent wilting point*/
  water_layer =soil->par->whcs[layer]*soil->w[layer]+soil->w_fw[layer]+soil->par->wpwps[layer]*(1-soil->ice_pwp[layer]);/*[mm]*/
  ice_layer = soil->ice_depth[layer]+soil->ice_fw[layer]+soil->par->wpwps[layer]*soil->ice_pwp[layer];
  dry_soil_layer=soildepth[layer]-soil->par->wsats[layer];
  heatcap = (c_mineral*dry_soil_layer+c_water*water_layer+c_ice*ice_layer)/1000; /* [J/m2/K] */
  return heatcap;
} /* of 'soilheatcap' */

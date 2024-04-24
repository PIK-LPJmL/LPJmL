/**************************************************************************************/
/**                                                                                \n**/
/**                    compute_mean_layer_temps_from_enth.c                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void compute_mean_layer_temps_from_enth(Real temp[],                /**< temperature vector that is written (°C) */
                                        const Real enth[],          /**< input enthalpy vector (J/m^3) */
                                        const Soil_thermal_prop *th /**< soil thermal properties */
                                       )
{
  int layer, j;
  int gp; /* gridpoint */
  Real T = 0.0, Tmean = 0.0;
  for(layer=0; layer<NSOILLAYER; ++layer)
  {
    for(j=0; j<GPLHEAT; ++j)
    {
      gp = layer*GPLHEAT+j;
      T = ENTH2TEMP(enth, th, gp);
      Tmean += T/GPLHEAT;
    }
    temp[layer] = Tmean; /* assign the resulting mean */
    Tmean = 0.0;
  }
} /* of 'compute_mean_layer_temps_from_enth' */

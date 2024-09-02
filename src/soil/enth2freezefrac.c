/**************************************************************************************/
/**                                                                                \n**/
/**                    enth2freezefrac.c                                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void enth2freezefrac(Real freeze_frac[],         /**< vector with a fraction for each layer that is frozen */
                     const Real enth[],          /**< enthalpy vector (J/m^3) */
                     const Soil_thermal_prop *th /**< thermal properties of soil */
                    )
{
  int layer,j;
  int gp;                /* gridpoint  */
  Real ff_layer;         /* frozen fraction of layer */

  for(layer=0; layer<NSOILLAYER; ++layer)
  {
    ff_layer=0;
    for(j=0; j<GPLHEAT; ++j)
    {
      gp=layer*GPLHEAT+j;
      if(th->latent_heat[gp]<epsilon)
        ff_layer += (enth[gp]>0?0.0:1.0)/GPLHEAT;
      else
        ff_layer += (1 - min(max((enth[gp]/th->latent_heat[gp]), 0), 1)) / GPLHEAT;
    }

    /* Compensate for numerical precision errors during summation */
    if (ff_layer > 1-epsilon)
    {
        ff_layer = 1;
    }
    freeze_frac[layer]=ff_layer;
  }
} /* of 'enth2freezefrac' */

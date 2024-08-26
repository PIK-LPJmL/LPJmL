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

  Real n_wi;      /* normal (i.e. all except for free) soil waterice, absolute  */
  Real f_wi;      /* free soil waterice, absolute  */

  for(layer=0; layer<NSOILLAYER; ++layer)
  {
    soil->freeze_depth[layer] = freezfrac[layer] * soildepth[layer];

    /* get the absolute quantities */
    if(allwater(soil,layer) + allice(soil,layer)<epsilon)
      continue; /* Without any water there is nothing to do */

    n_wi  = soil->w[layer] * soil->whcs[layer] + soil->ice_depth[layer];
    f_wi  = soil->w_fw[layer] + soil->ice_fw[layer];

    /* divide n_wi and f_wi to water and ice pools using the freezefrac */
    soil->ice_depth[layer] = n_wi * freezfrac[layer];
    soil->w[layer]         = n_wi * (1-freezfrac[layer]) / soil->whcs[layer];
    soil->ice_pwp[layer]   = freezfrac[layer];
    soil->ice_fw[layer]    = f_wi * freezfrac[layer];
    soil->w_fw[layer]      = f_wi * (1-freezfrac[layer]);
    if (soil->w[layer]< -epsilon || soil->w_fw[layer]< -epsilon )
    {   fprintf(stderr,"\n\nfreezefrac2soil soilwater=%.6f soilice=%.6f wsats=%.6f agtop_moist=%.6f\n",
            allwater(soil,layer),allice(soil,layer),soil->wsats[layer],soil->litter.agtop_moist);
        fflush(stderr);
        fprintf(stderr,"Soil-moisture layer %d negative: w:%g, fw:%g, soil_type %s \n\n",
            layer,soil->w[layer],soil->w_fw[layer],soil->par->name);
    }
  }
} /* of 'freezefrac2soil' */

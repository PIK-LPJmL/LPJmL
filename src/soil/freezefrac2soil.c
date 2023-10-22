#include "lpj.h"
#include <errno.h>

void freezefrac2soil(Soil *soil, 
                     const Real *freezfrac
                    )
{
  int layer;

  Real n_wi;      /* normal soil waterice, absolute  */
  Real f_wi;      /* free soil waterice, absolute  */
  Real wp_wi;     /* below wiltingpoint soil waterice, absolute  */
  
  for(layer=0;layer<NSOILLAYER;++layer){

    soil->freeze_depth[layer] = freezfrac[layer] * soildepth[layer];

    /* get the absolute quantities */
    if(allwater(soil,layer) + allice(soil,layer)<epsilon)
      continue; /* Without any water there is nothing to do */
    
    n_wi  = soil->w[layer] * soil->whcs[layer] + soil->ice_depth[layer];
    wp_wi = soil->wpwps[layer];
    f_wi  = soil->w_fw[layer] + soil->ice_fw[layer];

    soil->ice_depth[layer] = n_wi * freezfrac[layer];
    soil->w[layer] = n_wi * (1-freezfrac[layer]) / soil->whcs[layer];
    soil->ice_pwp[layer] = freezfrac[layer];
    soil->ice_fw[layer] = f_wi * freezfrac[layer];
    soil->w_fw[layer] = f_wi * (1-freezfrac[layer]);
  }
}
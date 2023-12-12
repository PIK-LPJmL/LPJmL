#include "lpj.h"

void enth2freezefrac(Real *freeze_frac,         /**< vector with a fraction for each layer that is frozen */
                     const Real *enth,          /**< enthalpy vector, if NULL enth of soil is used */
                     const Soil_thermal_prop *th /**< thermal properties of soil */
                    )
{
  int layer,j;
  int gp;                /* upper and lower gridpoint of an element  */
  Real ff_layer;                 /* frozen fraction of layer */

  /******Get fraction of the layer that is frozen*******/

  for(layer=0;layer<NSOILLAYER;++layer)
  {
    ff_layer=0;
    for(j=0; j<GPLHEAT; ++j)
    {
      /* get upper an lower gripoints of the element */
      gp=layer*GPLHEAT+j;
      if(th->latent_heat[gp]==0.0)
        ff_layer += (enth[gp]>0? 0.0:1.0)/GPLHEAT;
      else
        ff_layer += (1-min( max((enth[gp]/th->latent_heat[gp]),0) ,1))/GPLHEAT;
    }

    /* Compensate for numerical precision errors druing summation */
    if (ff_layer > 1 - epsilon)
    {
        ff_layer = 1;
    }
    freeze_frac[layer]=ff_layer;
  }
}
 /* of 'enth2freezefrac' */

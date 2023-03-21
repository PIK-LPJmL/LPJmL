#include "lpj.h"


void enth2freezefrac( Real *freeze_frac,            /**< vector with a fraction for each layer that is frozen */
                            const Real *enth,         /**< enthalpy vector, if NULL enth of soil is used */
                            const Soil_thermal_prop th    /**< thermal properties of soil */
                           )
{
  int layer,j;
  int gp_u,gp_l;                /* upper and lower gridpoint of an element  */
  Real enth_element,lat_element; /* total enthalpy, maximum latent heat */
  Real ff_element;               /* frozen fraction of an element */
  Real ff_layer;                 /* frozen fraction of layer */

  /******Get fraction of the layer that is frozen*******/

  for(layer=0;layer<NSOILLAYER;++layer)
  {
    ff_layer=0;
    for(j=0; j<GPLHEAT; ++j)
    {
      /* get upper an lower gripoints of the element */
      gp_u=layer*GPLHEAT+j;
      gp_l=(gp_u>0 ? gp_u-1: 0 );

      /* get the enthalpy and maximum latent heat of the element */
      enth_element = (enth[gp_u]           + enth[gp_l]          ) / 2;
      lat_element  = (th.latent_heat[gp_u] + th.latent_heat[gp_l]) / 2;

      /* get frozen fraction of element and add contribution to frozen fraction of layer*/
      ff_element   = 1 - max(min( enth_element / lat_element, 1),0);
      ff_layer    += ff_element/GPLHEAT;
    }
    freeze_frac[layer]=ff_layer;
  }
}
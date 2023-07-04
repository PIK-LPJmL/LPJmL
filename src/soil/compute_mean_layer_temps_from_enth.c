/*
*/

#include "lpj.h"

void compute_mean_layer_temps_from_enth(Real *temp,          /*< temperature vector that is written to N=NSOILLAYER */
                    const Real *enth,    /*< input enthalpy vector N=NHEATGRID */
                    Soil_thermal_prop th /*< soil thermal properties */
                    )
{
  int layer, j;
  int gp; /* gridpoint */
  Real T =0.0, Tmean=0.0; 
  
  for(layer=0; layer<NSOILLAYER; ++layer)
  {
    for(j=0; j<GPLHEAT; ++j)
    {
      gp=layer*GPLHEAT+j;                 
      T = ENTH2TEMP(enth,th,gp);
      Tmean += T/GPLHEAT;
    }
    temp[layer]= Tmean; /* assign the resulting mean */
    Tmean=0;
  }

}

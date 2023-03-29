/*
The function takes an enthalpy vector and thermal properties of soil
defined at the refined heat grid as input and computes mean layer 
temperatures.
Schematically it does this in two steps: 
1. Compute the temperatures at each gridpoint corresponding to the enthalpy at the gridpoints
2. Aggregate the nodal temperatures to mean layer temperatures
The second step uses linear interpolation between the temperature at the gridpoints of 1.
*/

#include "lpj.h"

void enth2layertemp(Real *temp,          /*< temperature vector that is written to N=NSOILLAYER */
                    const Real *enth,    /*< input enthalpy vector N=NHEATGRID */
                    Soil_thermal_prop th /*< soil thermal properties */
                    )
{
  int layer, j;
  int gp; /* gridpoint */
  Real T =0.0, Tmean=0.0; 
  for(layer=0; layer<NSOILLAYER; ++layer)
  {
    /* gridpoint at the upper layer border */
    gp=(layer>0 ? layer*GPLHEAT-1 : 0); /* Exception for layer=0: take first gridpoint inside the layer */
    T = ENTH2TEMP(enth,th,gp);          /* get corresponding temperature */
    Tmean += T/(GPLHEAT*2);             /* Contribution of the first gridpoint to Tmean */
    for(j=0; j<GPLHEAT-1; ++j)
    {
      /* gridpoints inside of the layer */
      gp=layer*GPLHEAT+j;                 
      T = ENTH2TEMP(enth,th,gp);
      Tmean += T/GPLHEAT;
    }
    /* gridpoint at the lower layer border */
    gp=layer*GPLHEAT+GPLHEAT-1;
    T = ENTH2TEMP(enth,th,gp);
    Tmean += T/(GPLHEAT*2); 
    temp[layer]= Tmean; /* assign the resulting mean */
    Tmean=0;
  }
}

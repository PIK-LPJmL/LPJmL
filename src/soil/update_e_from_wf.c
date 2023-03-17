#include "lpj.h"

/*
Function to update enthalpy from incoming and outgoing waterflow
A change in water content due to inflow or ouflow of water
normally goes in hand with a change of volumetric enthalpy of the soil, 
since the inflowing or outflowing water normally carries some enthalpy.
This function updates the enthalpy vector based on changes in volumetric water content
and the assumption that the volumetric enthalpy of outgoing and incoming water 
equals the volumetric enthalpy of the water that stays at the respective gridpoint.
The function can be used if one does not know the energy content of the outgoing or incoming water
or does not want the water flow to influence soil temperatures. 
*/

void update_e_from_wf(Real *enth,             /*< enthalpy vector that is updated*/
                      const Real *water_diff, /*< change of volumetric water content */
                      Soil_thermal_prop th    /*< soil thermal properties */
                      )
{
    int l,j;
    int gp;
    Real gp_water_energy,T;
    for(l=0;l<NSOILLAYER;++l){
        for(j=0;j<GPLHEAT;++j){
            gp=GPLHEAT*l+j;          /* gridpoint */
            T=ENTH2TEMP(enth,th,gp); /* temperature at gridpoint */
            /* get volumetric enthalpy of current water at the gridpoint */
            gp_water_energy=(enth[gp] > th.latent_heat[gp] ? c_water*T + c_water2ice : 0)+
                            (enth[gp] < 0                  ? c_ice*T                 : 0)+
                            (enth[gp] <=th.latent_heat[gp] && enth[gp] >=0 ? 
                                           enth[gp]/th.latent_heat[gp] * c_water2ice : 0);
            /* add the energy from the inflowing water assuming is has the 
               the same volumetric enthalpy as the water already there  */
            enth[gp]+=gp_water_energy*water_diff[l]; 
        }
    }
}

#include "lpj.h"

void apply_perc_enthalpy(Soil * soil
                      )
{
  int l,j;   /* l=layer, j is for sublayer */
  int gp;    /* gridpoint */


  for(l=0;l<NSOILLAYER;l++){
    Real vol_enth_change = soil->perc_energy[l]/(soildepth[l]/1000);
        
        for (j = 0; j < GPLHEAT; j++) {
          soil->enth[l*GPLHEAT + j] += vol_enth_change;
        }
    soil->perc_energy[l]=0;
  }
  Soil_thermal_prop therm;
  calc_soil_thermal_props(&therm,soil,soil->wi_abs_enth_adj,soil->sol_abs_enth_adj,TRUE,FALSE);
  compute_mean_layer_temps_from_enth(soil->temp, soil->enth, therm);
}
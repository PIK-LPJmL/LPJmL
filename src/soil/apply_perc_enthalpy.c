#include "lpj.h"

void apply_perc_enthalpy(Soil * soil
                      )
{
  int l,j;   /* l=layer, j is for sublayer */
  int gp;    /* gridpoint */


  for(l=0;l<NSOILLAYER;l++){
    Real vol_enth_change = soil->perc_energy[l]/(soildepth[l]/1000);
    if (gp_status_dep_water_heat_flow) {
      Real enth_change=vol_enth_change*GPLHEAT;
      if (enth_change>0) {
        int num_pos_enth_points = 0;
        if (full_gp_status_dep) {
          for ( j = 0; j < GPLHEAT; ++j) {
            if (soil->enth[l*GPLHEAT + j] >= 0) { 
              ++num_pos_enth_points;
            }
          }
          if (num_pos_enth_points==0)
            num_pos_enth_points=GPLHEAT;
          Real change_per_point = enth_change/num_pos_enth_points ;
          for ( j = 0; j < GPLHEAT; j++) {
            if(soil->enth[l*GPLHEAT + j]>=0)
              soil->enth[l*GPLHEAT + j] += change_per_point;
          }
        }
        else {
          Real change_per_point = enth_change/GPLHEAT ;
          for ( j = 0; j < GPLHEAT; j++) {
            soil->enth[l*GPLHEAT + j] += change_per_point;
          }
        }
      } 
      else 
      {
        // distribute until reaching zero
        while (enth_change < -1e-8) {
          int num_pos_enth_points = 0;
          for(j = 0; j < GPLHEAT; ++j) {
            if (soil->enth[l*GPLHEAT + j] > 0) {
                ++num_pos_enth_points;
            }
          }

          if (num_pos_enth_points == 0) {
            num_pos_enth_points=GPLHEAT;
            Real change_per_point = enth_change/num_pos_enth_points ;
            for (j = 0; j < GPLHEAT; j++) {
              //if(soil->enth[l*GPLHEAT + j]>=0)
                soil->enth[l*GPLHEAT + j] += change_per_point;
            }
            enth_change=0;
          }
          Real change_per_point = enth_change / num_pos_enth_points;
          for (j = 0; j < GPLHEAT; ++j) {
            if(soil->enth[l*GPLHEAT + j]>0){
              if (soil->enth[l*GPLHEAT + j] + change_per_point < 0) {
                enth_change += soil->enth[l*GPLHEAT + j];  // remove this point's energy from the total
                soil->enth[l*GPLHEAT + j] = 0;
              } else {
                soil->enth[l*GPLHEAT + j] += change_per_point;
                enth_change -= change_per_point;
              }
            }
          }
        }
      }
    }
    else {              
        for (j = 0; j < GPLHEAT; j++) {
          soil->enth[l*GPLHEAT + j] += vol_enth_change;
        }
    }
    soil->perc_energy[l]=0;
  }
    Soil_thermal_prop therm;
    calc_soil_thermal_props(&therm,soil,soil->wi_abs_enth_adj,soil->sol_abs_enth_adj,TRUE,FALSE);
    compute_mean_layer_temps_from_enth(soil->temp, soil->enth, therm);
}
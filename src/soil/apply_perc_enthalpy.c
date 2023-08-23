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
}
/**************************************************************************************/
/**                                                                                \n**/
/**                    apply_perc_enthalpy.c                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void apply_perc_enthalpy(Soil * soil /**< pointer to soil data */
                        )
{
  int l,j;   /* l=layer, j is for sublayer */

  for(l=0;l<NSOILLAYER;l++)
  {
    Real vol_enth_change = soil->perc_energy[l]/(soildepth[l]/1000);

    for (j = 0; j < GPLHEAT; j++)
    {
      soil->enth[l*GPLHEAT + j] += vol_enth_change;
    }
    soil->perc_energy[l]=0;
  }
} /* of 'apply_perc_enthalpy' */

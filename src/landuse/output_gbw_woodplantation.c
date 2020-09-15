/**************************************************************************************/
/**                                                                                \n**/
/**          o u t p u t _ g b w _ w o o d p l a n t a t i o n . c                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "agriculture.h"
#include "woodplantation.h"

void output_gbw_woodplantation(Output *output, /**< output data */
                               const Stand *stand,  /**< stand pointer */
                               Real frac_g_evap,
                               Real evap,           /**< evaporation (mm) */
                               Real evap_blue,      /**< evaporation of irrigation water (mm) */
                               Real return_flow_b,  /**< irrigation return flows from surface runoff, lateral runoff and percolation (mm) */
                               const Real aet_stand[LASTLAYER],
                               const Real green_transp[LASTLAYER],
                               Real intercep_stand,  /**< stand interception (mm) */
                               Real intercep_stand_blue,  /**< stand interception from irrigation(mm) */
                               int ncft,            /**< number of CROPS */
                               Bool pft_output_scaled
                              )
{
  int l,index;
  Real total_g,total_b;
  Irrigation *data;
  data=stand->data;
  total_g=total_b=0;

  total_g+=intercep_stand-intercep_stand_blue;
  total_b+=intercep_stand_blue;

  total_g+=evap*frac_g_evap;
  total_b+=evap_blue;
  forrootsoillayer(l)
  {
    total_g+=green_transp[l];
    total_b+=aet_stand[l]-green_transp[l];
  }
#if defined IMAGE || defined INCLUDEWP
  index=rwp(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE+NWPTYPE);
  if(pft_output_scaled)
  {
    output->cft_consump_water_g[index]+=total_g*stand->cell->ml.landfrac[data->irrigation].woodplantation;
    output->cft_consump_water_b[index]+=total_b*stand->cell->ml.landfrac[data->irrigation].woodplantation;
    forrootsoillayer(l)
    {
      output->cft_transp[index]+=aet_stand[l]*stand->cell->ml.landfrac[data->irrigation].woodplantation;
      output->cft_transp_b[index]+=(aet_stand[l]-green_transp[l])*stand->cell->ml.landfrac[data->irrigation].woodplantation;
    }

    output->cft_evap[index]+=evap*stand->cell->ml.landfrac[data->irrigation].woodplantation;
    output->cft_evap_b[index]+=evap_blue*stand->cell->ml.landfrac[data->irrigation].woodplantation;
    output->cft_interc[index]+=intercep_stand*stand->cell->ml.landfrac[data->irrigation].woodplantation;
    output->cft_interc_b[index]+=intercep_stand_blue*stand->cell->ml.landfrac[data->irrigation].woodplantation;
    output->cft_return_flow_b[index]+=return_flow_b*stand->cell->ml.landfrac[data->irrigation].woodplantation;
  }
  else
  {
    output->cft_consump_water_g[index]+=total_g;
    output->cft_consump_water_b[index]+=total_b;
    forrootsoillayer(l)
    {
      output->cft_transp[index] += aet_stand[l];
      output->cft_transp_b[index] += aet_stand[l];
    }

    output->cft_evap[index] += evap;
    output->cft_evap_b[index] += evap_blue;
    output->cft_interc[index] += intercep_stand;
    output->cft_interc_b[index] += intercep_stand_blue;
    output->cft_return_flow_b[index] += return_flow_b;


    if (data->irrigation)
    {
      output->mgcons_irr += total_g*stand->cell->ml.landfrac[1].woodplantation;
      output->mbcons_irr += total_b*stand->cell->ml.landfrac[1].woodplantation;
    }
    else
    {
      output->mgcons_rf += total_g*stand->cell->ml.landfrac[0].woodplantation;
      output->mgcons_rf += total_b*stand->cell->ml.landfrac[0].woodplantation;
    }
  }
#endif
} /* of 'output_gbw_woodplantation' */

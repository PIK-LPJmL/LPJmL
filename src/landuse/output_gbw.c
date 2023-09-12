/**************************************************************************************/
/**                                                                                \n**/
/**                         o u t p u t _ g b w . c                                \n**/
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

void output_gbw(Output *output,      /**< output data */
                const Stand *stand,  /**< stand pointer */
                Real frac_g_evap,
                Real evap,           /**< evaporation (mm) */
                Real evap_blue,      /**< evaporation of irrigation water (mm) */
                Real return_flow_b,  /**< irrigation return flows from surface runoff, lateral runoff and percolation (mm) */
                const Real aet_stand[LASTLAYER],
                const Real green_transp[LASTLAYER],
                Real intercep_stand,  /**< stand interception (mm) */
                Real intercep_stand_blue, /**< stand interception from irrigation (mm) */
                int index,            /**< index of output */
                Bool irrigation,      /**< stand is irrigated (TRUE/FALSE) */
                const Config *config /**< LPJmL configuration */
               )
{
  int l;
  Real total_g,total_b;
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
  if(config->pft_output_scaled)
  {
    getoutputindex(output,CFT_CONSUMP_WATER_G,index,config)+=total_g*stand->frac;
    getoutputindex(output,CFT_CONSUMP_WATER_B,index,config)+=total_b*stand->frac;
    forrootsoillayer(l)
    {
      getoutputindex(output,CFT_TRANSP,index,config)+=aet_stand[l]*stand->frac;
      getoutputindex(output,CFT_TRANSP_B,index,config)+=(aet_stand[l]-green_transp[l])*stand->frac;
    }

    getoutputindex(output,CFT_EVAP,index,config)+=evap*stand->frac;
    getoutputindex(output,CFT_EVAP_B,index,config)+=evap_blue*stand->frac;
    getoutputindex(output,CFT_INTERC,index,config)+=intercep_stand*stand->frac;
    getoutputindex(output,CFT_INTERC_B,index,config)+=intercep_stand_blue*stand->frac;
    getoutputindex(output,CFT_RETURN_FLOW_B,index,config)+=return_flow_b*stand->frac;
  }
  else
  {
    getoutputindex(output,CFT_CONSUMP_WATER_G,index,config)+=total_g;
    getoutputindex(output,CFT_CONSUMP_WATER_B,index,config)+=total_b;
    forrootsoillayer(l)
    {
      getoutputindex(output,CFT_TRANSP,index,config)+=aet_stand[l];
      getoutputindex(output,CFT_TRANSP_B,index,config)+=aet_stand[l]-green_transp[l];
    }

    getoutputindex(output,CFT_EVAP,index,config)+=evap;
    getoutputindex(output,CFT_EVAP_B,index,config)+=evap_blue;
    getoutputindex(output,CFT_INTERC,index,config)+=intercep_stand;
    getoutputindex(output,CFT_INTERC_B,index,config)+=intercep_stand_blue;
    getoutputindex(output,CFT_RETURN_FLOW_B,index,config)+=return_flow_b;
  }

  if(irrigation)
  {
    getoutput(output,GCONS_IRR,config)+=total_g*stand->frac;
    getoutput(output,BCONS_IRR,config)+=total_b*stand->frac;
  }
  else
  {
    getoutput(output,GCONS_RF,config)+=total_g*stand->frac;
    getoutput(output,GCONS_RF,config)+=total_b*stand->frac;
  }
} /* of 'output_gbw' */

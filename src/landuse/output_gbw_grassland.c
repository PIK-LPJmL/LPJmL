/**************************************************************************************/
/**                                                                                \n**/
/**  o  u  t  p  u  t  _  g  b  w  _  g  r  a  s  s  l  a  n  d  .  c              \n**/
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
#include "grassland.h"

void output_gbw_grassland(Output *output,      /**< output data */
                          const Stand *stand,  /**< stand pointer */
                          Real frac_g_evap,
                          Real evap,           /**< evaporation (mm) */
                          Real evap_blue,      /**< evaporation of irrigation water (mm) */
                          Real return_flow_b,  /**< irrigation return flows from surface runoff, lateral runoff and percolation (mm) */
                          const Real aet_stand[LASTLAYER],
                          const Real green_transp[LASTLAYER],
                          Real intercep_stand, /**< stand interception (mm) */
                          Real intercep_stand_blue, /**< stand interception from irrigation (mm) */
                          int ncft,            /**< number of CROPS */
                          const Config *config /**< LPJmL configuration */
                         )
{
  int l,irrigation,index;
  Real total_g,total_b;
  Grassland *data;
  data=stand->data;
  irrigation=data->irrigation.irrigation;
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
  index=irrigation*getnirrig(ncft,config);
  if(config->pft_output_scaled)
  {
    getoutputindex(output,CFT_CONSUMP_WATER_G,rothers(ncft)+index,config)+=total_g*stand->cell->ml.landfrac[irrigation].grass[0];
    getoutputindex(output,CFT_CONSUMP_WATER_G,rmgrass(ncft)+index,config)+=total_g*stand->cell->ml.landfrac[irrigation].grass[1];
    getoutputindex(output,CFT_CONSUMP_WATER_B,rothers(ncft)+index,config)+=total_b*stand->cell->ml.landfrac[irrigation].grass[0];
    getoutputindex(output,CFT_CONSUMP_WATER_B,rmgrass(ncft)+index,config)+=total_b*stand->cell->ml.landfrac[irrigation].grass[1];
    forrootsoillayer(l)
    {
      getoutputindex(output,CFT_TRANSP,rothers(ncft)+index,config)+=aet_stand[l]*stand->cell->ml.landfrac[irrigation].grass[0];
      getoutputindex(output,CFT_TRANSP,rmgrass(ncft)+index,config)+=aet_stand[l]*stand->cell->ml.landfrac[irrigation].grass[1];
      getoutputindex(output,CFT_TRANSP_B,rothers(ncft)+index,config)+=(aet_stand[l]-green_transp[l])*stand->cell->ml.landfrac[irrigation].grass[0];
      getoutputindex(output,CFT_TRANSP_B,rmgrass(ncft)+index,config)+=(aet_stand[l]-green_transp[l])*stand->cell->ml.landfrac[irrigation].grass[1];
    }
    getoutputindex(output,CFT_EVAP,rothers(ncft)+index,config)+=evap*stand->cell->ml.landfrac[irrigation].grass[0];
    getoutputindex(output,CFT_EVAP,rmgrass(ncft)+index,config)+=evap*stand->cell->ml.landfrac[irrigation].grass[1];
    getoutputindex(output,CFT_EVAP_B,rothers(ncft)+index,config)+=evap_blue*stand->cell->ml.landfrac[irrigation].grass[0];
    getoutputindex(output,CFT_EVAP_B,rmgrass(ncft)+index,config)+=evap_blue*stand->cell->ml.landfrac[irrigation].grass[1];
    getoutputindex(output,CFT_INTERC,rothers(ncft)+index,config)+=intercep_stand*stand->cell->ml.landfrac[irrigation].grass[0];
    getoutputindex(output,CFT_INTERC,rmgrass(ncft)+index,config)+=intercep_stand*stand->cell->ml.landfrac[irrigation].grass[1];
    getoutputindex(output,CFT_INTERC_B,rothers(ncft)+index,config)+=intercep_stand_blue*stand->cell->ml.landfrac[irrigation].grass[0];
    getoutputindex(output,CFT_INTERC_B,rmgrass(ncft)+index,config)+=intercep_stand_blue*stand->cell->ml.landfrac[irrigation].grass[1];
    getoutputindex(output,CFT_RETURN_FLOW_B,rothers(ncft)+index,config)+=return_flow_b*stand->cell->ml.landfrac[irrigation].grass[0];
    getoutputindex(output,CFT_RETURN_FLOW_B,rmgrass(ncft)+index,config)+=return_flow_b*stand->cell->ml.landfrac[irrigation].grass[1];
  }
  else
  {
    getoutputindex(output,CFT_CONSUMP_WATER_G,rothers(ncft)+index,config)+=total_g;
    getoutputindex(output,CFT_CONSUMP_WATER_G,rmgrass(ncft)+index,config)+=total_g;
    getoutputindex(output,CFT_CONSUMP_WATER_B,rothers(ncft)+index,config)+=total_b;
    getoutputindex(output,CFT_CONSUMP_WATER_B,rmgrass(ncft)+index,config)+=total_b;
    forrootsoillayer(l)
    {
      getoutputindex(output,CFT_TRANSP,rothers(ncft)+index,config)+=aet_stand[l];
      getoutputindex(output,CFT_TRANSP,rmgrass(ncft)+index,config)+=aet_stand[l];
      getoutputindex(output,CFT_TRANSP_B,rothers(ncft)+index,config)+=aet_stand[l]-green_transp[l];
      getoutputindex(output,CFT_TRANSP_B,rmgrass(ncft)+index,config)+=aet_stand[l]-green_transp[l];
    }
    getoutputindex(output,CFT_EVAP,rothers(ncft)+index,config)+=evap;
    getoutputindex(output,CFT_EVAP,rmgrass(ncft)+index,config)+=evap;
    getoutputindex(output,CFT_EVAP_B,rothers(ncft)+index,config)+=evap_blue;
    getoutputindex(output,CFT_EVAP_B,rmgrass(ncft)+index,config)+=evap_blue;
    getoutputindex(output,CFT_INTERC,rothers(ncft)+index,config)+=intercep_stand;
    getoutputindex(output,CFT_INTERC,rmgrass(ncft)+index,config)+=intercep_stand;
    getoutputindex(output,CFT_INTERC_B,rothers(ncft)+index,config)+=intercep_stand_blue;
    getoutputindex(output,CFT_INTERC_B,rmgrass(ncft)+index,config)+=intercep_stand_blue;
    getoutputindex(output,CFT_RETURN_FLOW_B,rothers(ncft)+index,config)+=return_flow_b;
    getoutputindex(output,CFT_RETURN_FLOW_B,rmgrass(ncft)+index,config)+=return_flow_b;
  }

  if(irrigation)
  {
    getoutput(output,GCONS_IRR,config)+=total_g*(stand->cell->ml.landfrac[1].grass[0]+stand->cell->ml.landfrac[1].grass[1]);
    getoutput(output,BCONS_IRR,config)+=total_b*(stand->cell->ml.landfrac[1].grass[0]+stand->cell->ml.landfrac[1].grass[1]);
  }
  else
  {
    getoutput(output,GCONS_RF,config)+=total_g*(stand->cell->ml.landfrac[0].grass[0]+stand->cell->ml.landfrac[0].grass[1]);
    getoutput(output,GCONS_RF,config)+=total_b*(stand->cell->ml.landfrac[0].grass[0]+stand->cell->ml.landfrac[0].grass[1]);
  }
} /* of 'output_gbw_grassland' */

/**************************************************************************************/
/**                                                                                \n**/
/** o  u  t  p  u  t  _  g  b  w  _  a  g  r  i  c  u  l  t  u  r  e  .  c         \n**/
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

void output_gbw_agriculture(Output *output,      /**< output data */
                            const Stand *stand,  /**< stand pointer */
                            Real frac_g_evap,
                            Real evap,           /**< evaporation (mm) */
                            Real evap_blue,      /**< evaporation of irrigation water (mm) */
                            Real return_flow_b,  /**< irrigation return flows from surface runoff, lateral runoff and percolation (mm) */
                            const Real aet_stand[LASTLAYER],
                            const Real green_transp[LASTLAYER],
                            Real intercep_stand,      /**< total stand interception (mm) */
                            Real intercep_stand_blue, /**< stand interception from irrigation (mm) */
                            int npft,            /**< number of natural PFTs */
                            int ncft,            /**< number of CROPS */
                            const Config *config /**< LPJmL configuration */
                           )
{
  int l,p,index;
  Pft *pft;
  Pftcrop *crop;
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
  foreachpft(pft,p,&stand->pftlist)
  {
    crop=pft->data;
    index=pft->par->id-npft+data->irrigation*getnirrig(ncft,config);
    if(config->pft_output_scaled)
    {
      getoutputindex(output,CFT_CONSUMP_WATER_G,index,config)+=total_g*stand->frac;
      getoutputindex(output,CFT_CONSUMP_WATER_B,index,config)+=total_b*stand->frac;
      forrootsoillayer(l)
      {
        if(crop->dh!=NULL)
          crop->dh->transpsum+=aet_stand[l]*stand->frac;
        else
          getoutputindex(output,CFT_TRANSP,index,config)+=aet_stand[l]*stand->frac;
        getoutputindex(output,CFT_TRANSP_B,index,config)+=(aet_stand[l]-green_transp[l])*stand->frac;
      }
      if(crop->dh!=NULL)
      {
        crop->dh->evapsum+=evap*stand->frac;
        crop->dh->intercsum+=intercep_stand*stand->frac;
      }
      else
      {
        getoutputindex(output,CFT_EVAP,index,config)+=evap*stand->frac;
        getoutputindex(output,CFT_INTERC,index,config)+=intercep_stand*stand->frac;
      }
      getoutputindex(output,CFT_EVAP_B,index,config)+=evap_blue*stand->frac;
      getoutputindex(output,CFT_INTERC_B,index,config)+=intercep_stand_blue*stand->frac;
      getoutputindex(output,CFT_RETURN_FLOW_B,index,config)+=return_flow_b*stand->frac;
    }
    else
    {
      getoutputindex(output,CFT_CONSUMP_WATER_G,index,config)+=total_g;
      getoutputindex(output,CFT_CONSUMP_WATER_B,index,config)+=total_b;
      forrootsoillayer(l)
      {
        if(crop->dh!=NULL)
          crop->dh->transpsum+=aet_stand[l];
        else
          getoutputindex(output,CFT_TRANSP,index,config)+=aet_stand[l];
        getoutputindex(output,CFT_TRANSP_B,index,config)+=(aet_stand[l]-green_transp[l]);
      }
      if(crop->dh!=NULL)
      {
        crop->dh->evapsum+=evap;
        crop->dh->intercsum+=intercep_stand;
      }
      else
      {
        getoutputindex(output,CFT_EVAP,index,config)+=evap;
        getoutputindex(output,CFT_INTERC,index,config)+=intercep_stand;
      }
      getoutputindex(output,CFT_EVAP_B,index,config)+=evap_blue;
      getoutputindex(output,CFT_INTERC_B,index,config)+=intercep_stand_blue;
      getoutputindex(output,CFT_RETURN_FLOW_B,index,config)+=return_flow_b;
    }

    if(data->irrigation)
    {
      getoutput(output,GCONS_IRR,config)+=total_g*stand->frac;
      getoutput(output,BCONS_IRR,config)+=total_b*stand->frac;
    }
    else
    {
      getoutput(output,GCONS_RF,config)+=total_g*stand->frac;
      getoutput(output,GCONS_RF,config)+=total_b*stand->frac;
    }

  } /* of 'foreachpft' */
} /* of 'output_gbw_agriculture' */

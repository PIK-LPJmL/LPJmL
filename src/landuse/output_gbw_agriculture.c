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
                            Bool pft_output_scaled
                           )
{
  Pft *pft;
#ifdef DOUBLE_HARVEST
  Pftcrop *crop;
#endif
  int l,p;
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
#ifdef DOUBLE_HARVEST
    crop=pft->data;
#endif
    if(pft_output_scaled)
    {
      output->cft_consump_water_g[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=total_g*stand->frac;
      output->cft_consump_water_b[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=total_b*stand->frac;
      forrootsoillayer(l)
      {
#ifdef DOUBLE_HARVEST
        crop->transpsum+=aet_stand[l]*stand->frac;
#else
        output->cft_transp[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=aet_stand[l]*stand->frac;
#endif
        output->cft_transp_b[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=(aet_stand[l]-green_transp[l])*stand->frac;
      }
#ifdef DOUBLE_HARVEST
      crop->evapsum+=evap*stand->frac;
      crop->intercsum+=intercep_stand*stand->frac;
#else
      output->cft_evap[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=evap*stand->frac;
      output->cft_interc[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=intercep_stand*stand->frac;
#endif
      output->cft_evap_b[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=evap_blue*stand->frac;
      output->cft_interc_b[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=intercep_stand_blue*stand->frac;
      output->cft_return_flow_b[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=return_flow_b*stand->frac;
    }
    else
    {
      output->cft_consump_water_g[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=total_g;
      output->cft_consump_water_b[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=total_b;
      forrootsoillayer(l)
      {
#ifdef DOUBLE_HARVEST
        crop->transpsum+=aet_stand[l];
#else
        output->cft_transp[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=aet_stand[l];
#endif
        output->cft_transp_b[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=(aet_stand[l]-green_transp[l]);
      }
#ifdef DOUBLE_HARVEST
      crop->evapsum+=evap;
      crop->intercsum+=intercep_stand;
#else
      output->cft_evap[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=evap;
      output->cft_interc[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=intercep_stand;
#endif
      output->cft_evap_b[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=evap_blue;
      output->cft_interc_b[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=intercep_stand_blue;
      output->cft_return_flow_b[pft->par->id-npft+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=return_flow_b;
    }

    if(data->irrigation)
    {
      output->mgcons_irr+=total_g*stand->frac;
      output->mbcons_irr+=total_b*stand->frac;
    }
    else
    {
      output->mgcons_rf+=total_g*stand->frac;
      output->mgcons_rf+=total_b*stand->frac;
    }

  } /* of 'foreachpft' */
} /* of 'output_gbw_agriculture' */

/**************************************************************************************/
/**                                                                                \n**/
/**         i  n  i  t  o  u  t  p  u  t  _  a  n  n  u  a  l  .  c                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initializes annual output data to zero                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void initoutput_annual(Output *output, /**< Output data */
                       int npft,       /**< number of natural PFTs */
                       int nbiomass,   /**< number of biomass PFTs */
                       int ncft        /**< number of crop PFTs */
                      )
{
  int i;
  output->firec=output->firef=output->flux_harvest=output->flux_estab=0;
  output->input_lake=output->flux_firewood=output->flux_rharvest_burnt=output->flux_rharvest_burnt_in_field=0;
  output->atransp=output->aevap=output->ainterc=output->airrig=output->aconv_loss_evap=output->aconv_loss_drain=output->awateruse_hil=0;
  output->awd_unsustainable=output->aevap_lake=output->aevap_res=0;
  output->soil_storage=output->aburntarea=0;
  output->prod_turnover=output->deforest_emissions=output->fburn=output->ftimber=output->timber_harvest=0;
  output->product_pool_fast=output->product_pool_slow=output->trad_biofuel=0;

  /* memory allocation now in newgrid.c */

  for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    output->pft_harvest[i].harvest=output->pft_harvest[i].residual=output->cftfrac[i]=
    output->cft_consump_water_g[i]=output->cft_consump_water_b[i]=
    output->cft_transp[i]=output->cft_transp_b[i]=output->cft_evap[i]=output->cft_evap_b[i]=
    output->cft_interc[i]=output->cft_interc_b[i]=output->cft_return_flow_b[i]=output->cft_nir[i]=
#ifdef DOUBLE_HARVEST
    output->pft_harvest2[i].harvest=output->pft_harvest2[i].residual=output->cftfrac2[i]=
    output->cft_transp2[i]=output->cft_evap2[i]=output->cft_interc2[i]=
    output->cft_nir2[i]=output->cft_airrig2[i]=
#endif
    output->cft_airrig[i]=output->cft_fpar[i]=output->cft_luc_image[i]=output->cft_conv_loss_evap[i]=output->cft_conv_loss_drain[i]=output->cft_irrig_events[i]=0;
  for(i=0;i<(ncft+NGRASS)*2;i++)
    output->growing_period[i]=output->cft_pet[i]=
    output->cft_temp[i]=output->cft_prec[i]=output->cft_srad[i]=
#ifdef DOUBLE_HARVEST
    output->growing_period2[i]=output->cft_pet2[i]=
    output->cft_temp2[i]=output->cft_prec2[i]=output->cft_srad2[i]=
    output->cft_aboveground_biomass2[i]=
#endif
    output->cft_aboveground_biomass[i]=0;
  for(i=0;i<(ncft*2);i++){
#ifdef DOUBLE_HARVEST
    output->sdate2[i]=output->hdate2[i]=
    output->syear[i]=output->syear2[i]=
#endif
    output->sdate[i]=output->hdate[i]=0;
  }
  for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2+npft-nbiomass;i++)
  {
    output->pft_npp[i]=0;
    output->pft_gcgp[i]=0;
    output->gcgp_count[i]=0;
  }
  for (i=0; i<npft-nbiomass+1;++i)
    output->fpc[i] = 0;
} /* of 'initoutput_annual' */

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
  output->neg_fluxes.carbon= output->neg_fluxes.nitrogen=
  output->fire.carbon=output->fire.nitrogen=output->firef=
  output->flux_harvest.carbon=output->flux_harvest.nitrogen=
  output->flux_estab.carbon=output->flux_estab.nitrogen=0;
  output->input_lake=output->flux_firewood.carbon=output->flux_firewood.nitrogen=
  output->flux_rharvest_burnt.carbon=output->flux_rharvest_burnt.nitrogen=
  output->flux_rharvest_burnt_in_field.carbon= output->flux_rharvest_burnt_in_field.nitrogen=0;
  output->atransp=output->aevap=output->ainterc=output->airrig=output->aconv_loss_evap=output->aconv_loss_drain=output->awateruse_hil=0;
  output->awd_unsustainable=output->aevap_lake=output->aevap_res=0;
  output->soil_storage=output->aburntarea=0;
  output->soil_storage=output->alittfall.carbon=output->alittfall.nitrogen=0;
  output->prod_turnover=output->deforest_emissions.carbon=output->deforest_emissions.nitrogen=output->fburn=output->ftimber=output->timber_harvest.carbon=output->timber_harvest.nitrogen=0;
  output->product_pool_fast=output->product_pool_slow=output->trad_biofuel=output->mean_vegc_mangrass=0;

  /* memory allocation now in newgrid.c */

  for(i=0;i<(ncft+NGRASS+NBIOMASSTYPE)*2;i++)
    output->pft_harvest[i].harvest.carbon=output->pft_harvest[i].residual.carbon=
    output->pft_harvest[i].harvest.nitrogen=output->pft_harvest[i].residual.nitrogen=
    output->cftfrac[i]=
    output->cft_consump_water_g[i]=output->cft_consump_water_b[i]=
    output->cft_transp[i]=output->cft_transp_b[i]=output->cft_evap[i]=output->cft_evap_b[i]=
    output->cft_interc[i]=output->cft_interc_b[i]=output->cft_return_flow_b[i]=output->cft_nir[i]=
#ifdef DOUBLE_HARVEST
    output->pft_harvest2[i].harvest.carbon=output->pft_harvest2[i].residual.carbon=
    output->pft_harvest2[i].harvest.nitrogen=output->pft_harvest2[i].residual.nitrogen=
    output->cftfrac2[i]=
    output->cft_transp2[i]=output->cft_evap2[i]=output->cft_interc2[i]=
    output->cft_nir2[i]=output->cft_airrig2[i]=
#endif
    output->cft_nlimit[i]=output->cft_laimax[i]=
    output->cft_airrig[i]=output->cft_fpar[i]=output->cft_luc_image[i]=output->cft_conv_loss_evap[i]=output->cft_conv_loss_drain[i]=
    output->cft_leaf[i].nitrogen=output->cft_leaf[i].carbon=output->cft_root[i].carbon=output->cft_root[i].nitrogen=
    output->cft_veg[i].carbon=output->cft_veg[i].nitrogen=output->cft_irrig_events[i]=0;
  for(i=0;i<(ncft+NGRASS)*2;i++)
    output->growing_period[i]=output->cft_pet[i]=
    output->cft_temp[i]=output->cft_prec[i]=output->cft_srad[i]=
#ifdef DOUBLE_HARVEST
    output->growing_period2[i]=output->cft_pet2[i]=
    output->cft_temp2[i]=output->cft_prec2[i]=output->cft_srad2[i]=
    output->cft_aboveground_biomass2[i].carbon=output->cft_aboveground_biomass2[i].nitrogen=
#endif
    output->cft_aboveground_biomass[i].carbon=output->cft_aboveground_biomass[i].nitrogen=0;
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
    output->pft_nuptake[i]=0;
    output->pft_ndemand[i]=0;
  }
  for (i=0; i<npft-nbiomass+1;++i)
    output->fpc[i] = 0;
} /* of 'initoutput_annual' */

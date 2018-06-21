/**************************************************************************************/
/**                                                                                \n**/
/**        f  r  e  e  o  u  t  p  u  t  .  c                                      \n**/
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

void freeoutput(Output *output /**< Output data */
               )
{
  free(output->sdate);
  free(output->hdate);
  free(output->pft_harvest);
  free(output->cft_consump_water_g);
  free(output->cft_consump_water_b);
  free(output->growing_period);  
  free(output->pft_npp);
  free(output->pft_gcgp);
  free(output->gcgp_count);
  free(output->fpc);
  free(output->cftfrac);
  free(output->cft_pet);
  free(output->cft_transp);
  free(output->cft_transp_b);
  free(output->cft_evap);
  free(output->cft_evap_b);
  free(output->cft_interc);
  free(output->cft_interc_b);
  free(output->cft_return_flow_b);
  free(output->cft_nir);
  free(output->cft_fpar);
  free(output->cft_temp);
  free(output->cft_prec);
  free(output->cft_srad);
  free(output->cft_aboveground_biomass);
  free(output->cft_airrig);
  free(output->cft_conv_loss_evap);
  free(output->cft_conv_loss_drain);
  free(output->cft_luc_image);
  free(output->cft_irrig_events);
#ifdef DOUBLE_HARVEST
  free(output->cftfrac2);
  free(output->sdate2);
  free(output->hdate2);
  free(output->pft_harvest2);
  free(output->cft_pet2);
  free(output->cft_transp2);
  free(output->cft_evap2);
  free(output->cft_interc2);
  free(output->cft_nir2);
  free(output->cft_temp2);
  free(output->cft_prec2);
  free(output->cft_srad2);
  free(output->cft_aboveground_biomass2);
  free(output->cft_airrig2);
  free(output->syear);
  free(output->syear2);
  output->sdate2=output->hdate2=output->syear=output->syear2=NULL;
  output->cft_transp2=output->cft_evap2=output->cft_interc2=output->cft_nir2=
    output->cft_pet2=output->cftfrac2=output->cft_airrig2=NULL;
  output->pft_harvest2=NULL;
  output->cft_temp2=output->cft_prec2=output->cft_srad2=
    output->cft_aboveground_biomass2=NULL;
#endif
  output->sdate=output->hdate=NULL;
  output->pft_harvest=NULL;
  output->cft_consump_water_g=output->cft_consump_water_b=output->cft_pet=NULL;
  output->cft_transp=output->cft_transp_b=output->cft_evap=output->cft_evap_b=output->cft_interc=output->cft_interc_b=
    output->cft_return_flow_b=output->cft_nir=output->cft_temp=output->cft_prec=output->cft_srad=output->cft_fpar=
    output->cft_aboveground_biomass=output->cft_conv_loss_evap=output->cft_conv_loss_drain=NULL;
  output->growing_period=NULL;
  output->cft_irrig_events=NULL;
  output->pft_npp=output->fpc=output->cftfrac=output->cft_airrig=output->cft_luc_image=NULL;
} /* of 'freeoutput' */

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
  free(output->husum);
  free(output->pft_harvest);
  free(output->cft_consump_water_g);
  free(output->cft_consump_water_b);
  free(output->growing_period);  
  free(output->pft_npp);
  free(output->pft_nuptake);
  free(output->pft_ndemand);
  free(output->mpft_lai);
  free(output->pft_gcgp);
  free(output->gcgp_count);
  free(output->fpc);
  free(output->fpc_bft);
  free(output->nv_lai);
  free(output->pft_mort);
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
  free(output->pft_laimax);
  free(output->pft_root);
  free(output->pft_leaf);
  free(output->pft_sapw);
  free(output->pft_hawo);
  free(output->pft_veg);
  free(output->pft_nlimit);
  free(output->wft_vegc);
  free(output->cft_irrig_events);
  free(output->cft_mswc);
  free(output->nday_month);
  free(output->cft_runoff);
  free(output->cft_n2o_denit);
  free(output->cft_n2o_nit);
  free(output->cft_n2_emis);
  free(output->cft_leaching);
  free(output->cft_c_emis);
  if(output->dh!=NULL)
  {
    free(output->dh->cftfrac2);
    free(output->dh->sdate2);
    free(output->dh->hdate2);
    free(output->dh->husum2);
    free(output->dh->pft_harvest2);
    free(output->dh->cft_pet2);
    free(output->dh->cft_transp2);
    free(output->dh->cft_evap2);
    free(output->dh->cft_interc2);
    free(output->dh->cft_nir2);
    free(output->dh->cft_temp2);
    free(output->dh->cft_prec2);
    free(output->dh->cft_srad2);
    free(output->dh->cft_aboveground_biomass2);
    free(output->dh->growing_period2);
    free(output->dh->cft_airrig2);
    free(output->dh->syear);
    free(output->dh->syear2);
    free(output->dh->pft_nuptake2);
    free(output->dh->cft_runoff2);
    free(output->dh->cft_n2o_denit2);
    free(output->dh->cft_n2o_nit2);
    free(output->dh->cft_n2_emis2);
    free(output->dh->cft_leaching2);
    free(output->dh->cft_c_emis2);
    free(output->dh);
  }
} /* of 'freeoutput' */

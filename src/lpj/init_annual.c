/**************************************************************************************/
/**                                                                                \n**/
/**             i  n  i  t  _  a  n  n  u  a  l  .  c                              \n**/
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

void init_annual(Cell *cell, /**< Pointer to cell */
                 int npft,   /**< number of natural pfts */
                 int nbiomass, /**< number of biomass pfts */
                 int ncft,    /**< number of crop pfts */
                 const Config *config
                )
{
  int s,p;
  Pft *pft;
  Stand *stand;
  init_climbuf(&cell->climbuf);
  cell->balance.aprec=cell->balance.anpp=cell->balance.arh=cell->balance.awater_flux=0.0;
  cell->afire_frac=cell->balance.biomass_yield.carbon=cell->balance.biomass_yield.nitrogen=0.0;
  cell->balance.total_irrig_from_reservoir=cell->balance.total_reservoir_out=0.0;
  cell->balance.n_influx=cell->balance.n_outflux=cell->balance.n_demand=cell->balance.n_uptake=0.0;
  cell->balance.ainterc=cell->balance.atransp=cell->balance.aevap=cell->balance.aevap_lake=
  cell->balance.aevap_res=cell->balance.soil_storage=cell->balance.airrig=cell->balance.awateruse_hil=0;
  cell->balance.adischarge= cell->balance.awd_unsustainable=0;
  cell->balance.aconv_loss_evap=cell->balance.aconv_loss_drain=cell->balance.trad_biofuel=0;
  cell->balance.fire.carbon=cell->balance.fire.nitrogen=0;
  cell->balance.flux_firewood.carbon=cell->balance.flux_firewood.nitrogen=0;
  cell->balance.flux_estab.carbon=cell->balance.flux_estab.nitrogen=0;
  cell->balance.flux_harvest.carbon=cell->balance.flux_harvest.nitrogen=0;
  cell->balance.deforest_emissions.carbon=cell->balance.deforest_emissions.nitrogen=0;
  cell->balance.prod_turnover.carbon=cell->balance.prod_turnover.nitrogen=0;
  cell->balance.neg_fluxes.carbon=cell->balance.neg_fluxes.nitrogen=0;
  foreachstand(stand,s,cell->standlist)
  {
#ifdef DEBUG3
    printf("init npft=%d\n",stand->pftlist.n);
#endif
    stand->fire_sum=0;
    stand->soil.maxthaw_depth=0;
    foreachpft(pft,p,&stand->pftlist)
      init(pft);
  } /* of foreachstand */
  //initoutput_annual(&cell->output,npft,nbiomass,config->ngrass,ncft);
#ifdef IMAGE
    cell->ml.image_data->prod_turn_fast=
    cell->ml.image_data->prod_turn_slow=0.0;
#endif
} /* of 'init_annual' */

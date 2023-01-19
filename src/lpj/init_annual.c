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

void init_annual(Cell *cell,          /**< Pointer to cell */
                 int ncft,            /**< number of crop pfts */
                 const Config *config /**< LPJ configuration */
                )
{
  int s,p;
#if defined IMAGE && defined COUPLED
  int m;
#endif
  Pft *pft;
  Stand *stand;
  init_climbuf(&cell->climbuf,ncft);
  cell->balance.aprec=cell->balance.anpp=cell->balance.arh=cell->balance.awater_flux=0.0;
  cell->afire_frac=cell->balance.biomass_yield.carbon=cell->balance.biomass_yield.nitrogen=0.0;
  cell->balance.total_irrig_from_reservoir=cell->balance.total_reservoir_out=0.0;
  cell->balance.influx.nitrogen=cell->balance.influx.carbon=
  cell->balance.n_outflux=cell->balance.n_demand=cell->balance.n_uptake=0.0;
  cell->balance.ainterc=cell->balance.atransp=cell->balance.aevap=cell->balance.aevap_lake=
  cell->balance.aevap_res=cell->balance.soil_storage=cell->balance.airrig=cell->balance.awateruse_hil=0;
  cell->balance.awd_unsustainable=0;
  cell->balance.aconv_loss_evap=cell->balance.aconv_loss_drain=0;
  cell->balance.trad_biofuel.carbon=cell->balance.trad_biofuel.nitrogen=0;
  cell->balance.fire.carbon=cell->balance.fire.nitrogen=0;
  cell->balance.flux_firewood.carbon=cell->balance.flux_firewood.nitrogen=0;
  cell->balance.flux_estab.carbon=cell->balance.flux_estab.nitrogen=0;
  cell->balance.flux_harvest.carbon=cell->balance.flux_harvest.nitrogen=0;
  cell->balance.timber_harvest.carbon=cell->balance.timber_harvest.nitrogen=0;
  cell->balance.deforest_emissions.carbon=cell->balance.deforest_emissions.nitrogen=0;
  cell->balance.prod_turnover.fast.carbon=cell->balance.prod_turnover.fast.nitrogen=0;
  cell->balance.prod_turnover.slow.carbon=cell->balance.prod_turnover.slow.nitrogen=0;
  cell->balance.neg_fluxes.carbon=cell->balance.neg_fluxes.nitrogen=0;
  cell->balance.excess_water=cell->balance.agpp=0;
  if(config->double_harvest)
    for(p=0;p<2*ncft;p++)
      cell->output.syear2[p]=cell->output.syear[p]=0;
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
#if defined IMAGE && defined COUPLED
  cell->npp_nat=cell->npp_wp=cell->flux_estab_nat=cell->flux_estab_wp=cell->rh_nat=cell->rh_wp=0.0;
  for(p=0;p<config->outputsize[PFT_HARVESTC];p++)
    cell->pft_harvest[p]=0;
  cell->ydischarge=0;
  cell->npp_grass=0;
  if(config->sim_id==LPJML_IMAGE)
  {
    for(m=0;m<NMONTH;m++)
    {
      cell->ml.image_data->mirrwatdem[m]=0.0;
      cell->ml.image_data->mevapotr[m] = 0.0;
      cell->ml.image_data->mpetim[m] = 0.0;
    }
  }
#endif
} /* of 'init_annual' */

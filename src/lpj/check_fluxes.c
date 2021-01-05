/**************************************************************************************/
/**                                                                                \n**/
/**             c  h  e  c  k  _  f  l  u  x  e  s  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function checks for water, nitrogen and carbon balance in a cell           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void check_fluxes(Cell *cell,          /**< cell pointer */
                  int year,            /**< simulation year (AD) */
                  int cellid,          /**< cell index */
                  const Config *config /**< LPJmL configuration */
                 )
{
  Stocks tot={0,0},stocks,delta_tot,balance;
  Real balanceW,totw;
  Stand *stand;
  String line;
  int s,i,startyear;

  int p;
  Pft *pft;
  /* carbon balance check */

  foreachstand(stand,s,cell->standlist)
  {
    stocks=standstocks(stand);
    tot.carbon+=stocks.carbon*stand->frac;
    tot.nitrogen+=stocks.nitrogen*stand->frac;
  }
  if(cell->ml.dam)
  {
    tot.carbon+=cell->ml.resdata->pool.carbon;
    tot.nitrogen+=cell->ml.resdata->pool.nitrogen;
  }
  tot.carbon+=cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  tot.nitrogen+=cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;
  tot.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon;
  tot.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen;
  delta_tot.carbon=tot.carbon-cell->balance.tot.carbon;
  delta_tot.nitrogen=tot.nitrogen-cell->balance.tot.nitrogen;
  cell->balance.tot=tot;

  balance.carbon=cell->balance.anpp-cell->balance.arh-cell->balance.fire.carbon-cell->balance.flux_firewood.carbon+cell->balance.flux_estab.carbon-cell->balance.flux_harvest.carbon-cell->balance.biomass_yield.carbon-delta_tot.carbon-cell->output.neg_fluxes.carbon;
  balance.nitrogen=cell->balance.n_influx-cell->balance.fire.nitrogen-cell->balance.flux_firewood.nitrogen-cell->balance.n_outflux+cell->balance.flux_estab.nitrogen-
    cell->balance.biomass_yield.nitrogen-cell->balance.flux_harvest.nitrogen-delta_tot.nitrogen-cell->balance.neg_fluxes.nitrogen-
    cell->balance.deforest_emissions.nitrogen;//cell->output.timber_harvest.nitrogen;
  /* for IMAGE but can also be used without IMAGE */
#ifdef IMAGE
  balance.carbon-=cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon+cell->balance.trad_biofuel.carbon+cell->output.timber_harvest.carbon;
#else
  balance.carbon-=cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon+cell->balance.trad_biofuel.carbon; // +cell->output.timber_harvest.carbon;
  balance.nitrogen-=cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen;
#endif
  if(config->ischeckpoint)
    startyear=max(config->firstyear,config->checkpointyear)+1;
  else
    startyear=config->firstyear+1;

  if(year>startyear && fabs(balance.carbon)>1)
  {
#if defined IMAGE && defined COUPLED
    if(config->sim_id==LPJML_IMAGE)
      foreachstand(stand,s,cell->standlist)
      {
        printf("C-balance on stand %d LUT %s, standfrac %g litter %g timber_frac %g\n",
          s,stand->type->name,stand->frac,littercarbon(&stand->soil.litter),cell->ml.image_data->timber_frac);
        fflush(stdout);
        foreachpft(pft,p,&stand->pftlist)
          printf("pft %d, %s, vegc %g\n",
                 p,pft->par->name,vegc_sum(pft));
        fflush(stdout);
      } /* of 'foreachstand' */
#ifdef NO_FAIL_BALANCE
    fprintf(stderr,"ERROR004: "
#else
    fail(INVALID_CARBON_BALANCE_ERR,FALSE,
#endif
         "y: %d c: %d (%s) BALANCE_C-error %.10f nep: %.2f firec: %.2f flux_estab: %.2f flux_harvest: %.2f delta_totc: %.2f\ndeforest_emissions: %.2f product_turnover: %.2f trad_biofuel: %.2f product pools %.2f %.2f timber_harvest %.2f ftimber %.2f fburn %.2f\n",
         year,cellid+config->startgrid,sprintcoord(line,&cell->coord),balance.carbon,cell->balance.anpp-cell->balance.arh,
         cell->balance.fire.carbon,
         cell->balance.flux_estab.carbon,cell->balance.flux_harvest.carbon,delta_tot.carbon,
         cell->balance.deforest_emissions.carbon,cell->balance.prod_turnover,cell->balance.trad_biofuel.carbon,
         cell->ml.product.slow.carbon,cell->ml.product.fast.carbon,cell->output.timber_harvest.carbon,
         cell->ml.image_data->timber_f,cell->ml.image_data->fburnt);
#else
#ifdef NO_FAIL_BALANCE
    fprintf(stderr,"ERROR004: "
#else
    fail(INVALID_CARBON_BALANCE_ERR,FALSE,
#endif
         "y: %d c: %d (%s) BALANCE_C-error %.10f nep: %.2f\n"
         "                            firec: %.2f flux_estab: %.2f \n"
         "                            flux_harvest: %.2f delta_totc: %.2f biomass_yield: %.2f\n"
         "                            estab_storage_grass: %.2f %.2f estab_storage_tree %.2f %.2f\n"
         "                            deforest_emissions: %.2f product_turnover: %.2f\n",
         year,cellid+config->startgrid,sprintcoord(line,&cell->coord),balance.carbon,cell->balance.anpp-cell->balance.arh,cell->balance.fire.carbon,
         cell->balance.flux_estab.carbon,cell->balance.flux_harvest.carbon,delta_tot.carbon,cell->balance.biomass_yield.carbon,
         cell->balance.estab_storage_grass[0].carbon,cell->balance.estab_storage_grass[1].carbon,cell->balance.estab_storage_tree[0].carbon,cell->balance.estab_storage_tree[1].carbon,
         cell->balance.deforest_emissions.carbon,cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon);
#endif
  }
  if(config->with_nitrogen && year>startyear && fabs(balance.nitrogen)>.2)
  {
#ifdef NO_FAIL_BALANCE
    fprintf(stderr,"ERROR032: "
#else
    fail(INVALID_NITROGEN_BALANCE_ERR,FALSE,
#endif
      "N-balance on y %d c %d (%0.2f/%0.2f) BALANCE_N-error %.10f n_influx %g n_outflux %g n_harvest %g n_biomass_yield %g n_estab %g n_defor_emis %g n_product_turnover %g delta_tot=%g total nitrogen=%g estab_storage grass [0] = %g estab_storage grass [1] = %g  estab_storage tree [0] = %g estab_storage tree [1] = %g \n",
      year,cellid+config->startgrid,cell->coord.lon,cell->coord.lat,balance.nitrogen,
      cell->balance.n_influx,cell->balance.n_outflux,cell->balance.flux_harvest.nitrogen,cell->balance.biomass_yield.nitrogen,cell->balance.flux_estab.nitrogen,
      cell->balance.deforest_emissions.nitrogen,
      cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen,
      delta_tot.nitrogen,tot.nitrogen,
      cell->balance.estab_storage_grass[0].nitrogen,cell->balance.estab_storage_grass[1].nitrogen,cell->balance.estab_storage_tree[0].nitrogen,
      cell->balance.estab_storage_tree[1].nitrogen);
    foreachstand(stand,s,cell->standlist){
      foreachpft(pft,p,&stand->pftlist){
        fprintf(stderr,"PFT bm_inc nitrogen %g\n",pft->bm_inc.nitrogen);
      }
    }
  }

  /* water balance check */
#ifdef IMAGE
  totw=(cell->discharge.dmass_lake+cell->discharge.dmass_river+cell->discharge.dmass_gw)/cell->coord.area;
#else
  totw=(cell->discharge.dmass_lake+cell->discharge.dmass_river)/cell->coord.area;
#endif
  foreachstand(stand,s,cell->standlist)
  {
    totw+=soilwater(&stand->soil)*stand->frac;
  }
  if(cell->ml.dam)
  {
    totw+=cell->ml.resdata->dmass/cell->coord.area;/*+cell->resdata->dfout_irrigation/cell->coord.area; */
    for(i=0;i<NIRRIGDAYS;i++)
      totw+=cell->ml.resdata->dfout_irrigation_daily[i]/cell->coord.area;
  }
  cell->balance.awater_flux+=cell->balance.atransp+cell->balance.aevap+cell->balance.ainterc+cell->balance.aevap_lake+cell->balance.aevap_res-cell->balance.airrig;
  balanceW=totw-cell->balance.totw-cell->balance.aprec+cell->balance.awater_flux+cell->balance.excess_water;
  if(year>startyear && fabs(balanceW)>1.5)
  //if(year>1511 && fabs(balanceW)>1.5)
#ifdef NO_FAIL_BALANCE
    fprintf(stderr,"ERROR005: "
#else
    fail(INVALID_WATER_BALANCE_ERR,FALSE,
#endif
         "y: %d c: %d (%s) BALANCE_W-error %.2f cell->totw:%.2f totw:%.2f awater_flux:%.2f aprec:%.2f excess water:%.2f\n",
         year,cellid+config->startgrid,sprintcoord(line,&cell->coord),balanceW,cell->balance.totw,totw,
         cell->balance.awater_flux,cell->balance.aprec,cell->balance.excess_water);
  cell->balance.totw=totw;
} /* of 'check_fluxes' */

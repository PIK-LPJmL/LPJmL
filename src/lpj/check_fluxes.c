/**************************************************************************************/
/**                                                                                \n**/
/**             c  h  e  c  k  _  f  l  u  x  e  s  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function checks for water and carbon balance in a cell                     \n**/
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
  int s,i;
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
#ifdef IMAGE
  tot.carbon+=cell->ml.image_data->timber.slow+cell->ml.image_data->timber.fast;
#endif
  delta_tot.carbon=tot.carbon-cell->balance.tot.carbon;
  delta_tot.nitrogen=tot.nitrogen-cell->balance.tot.nitrogen;
  cell->balance.tot=tot;

  balance.carbon=cell->balance.nep-cell->output.fire.carbon-cell->output.flux_firewood.carbon+cell->output.flux_estab.carbon-cell->output.flux_harvest.carbon-cell->balance.biomass_yield.carbon-delta_tot.carbon-cell->output.neg_fluxes.carbon;
  /* for IMAGE but can also be used without IMAGE */
  balance.carbon-=cell->output.deforest_emissions.carbon+cell->output.prod_turnover+cell->output.trad_biofuel+cell->output.timber_harvest.carbon;
  balance.nitrogen=cell->balance.n_influx-cell->output.fire.nitrogen-cell->balance.n_outflux+cell->output.flux_estab.nitrogen-
    cell->balance.biomass_yield.nitrogen-cell->output.flux_harvest.nitrogen-delta_tot.nitrogen-cell->output.neg_fluxes.nitrogen-cell->output.deforest_emissions.nitrogen-cell->output.timber_harvest.nitrogen;

  if(year>config->firstyear+1 && fabs(balance.carbon)>1)
  {
#ifdef IMAGE
    foreachstand(stand,s,cell->standlist)
    {
      printf("C-balance on stand %d LUT %s, standfrac %g litter %g timber_frac %g\n",
        s,stand->type->name,stand->frac,littersum(&stand->soil.litter),cell->ml.image_data->timber_frac);
      fflush(stdout);
      foreachpft(pft,p,&stand->pftlist)
        printf("pft %d, %s, vegc %g\n",
               p,pft->par->name,vegc_sum(pft));
      fflush(stdout);
    } /* of 'foreachstand' */
    fail(INVALID_CARBON_BALANCE_ERR,TRUE,"y: %d c: %d (%s) BALANCE_C-error %.10f nep: %.2f firec: %.2f flux_estab: %.2f flux_harvest: %.2f delta_totc: %.2f\ndeforest_emissions: %.2f product_turnover: %.2f trad_biofuel: %.2f product pools %.2f %.2f timber_harvest %.2f ftimber %.2f fburn %.2f\n",
         year,cellid+config->startgrid,sprintcoord(line,&cell->coord),balance.carbon,cell->balance.nep,
         cell->output.fire.carbon,
         cell->output.flux_estab.carbon,cell->output.flux_harvest.carbon,delta_tot.carbon,
         cell->output.deforest_emissions.carbon,cell->output.prod_turnover,cell->output.trad_biofuel,
         cell->ml.image_data->timber.slow,cell->ml.image_data->timber.fast,cell->output.timber_harvest.carbon,
         cell->ml.image_data->timber_f,cell->ml.image_data->fburnt);
#else
    fail(INVALID_CARBON_BALANCE_ERR,TRUE,"y: %d c: %d (%s) BALANCE_C-error %.10f nep: %.2f\n"
         "                            firec: %.2f flux_estab: %.2f \n"
         "                            flux_harvest: %.2f delta_totc: %.2f biomass_yield: %.2f\n"
         "                            estab_storage_grass: %.2f %.2f estab_storage_tree %.2f %.2f\n"
         "                            deforest_emissions: %.2f product_turnover: %.2f\n",
         year,cellid+config->startgrid,sprintcoord(line,&cell->coord),balance.carbon,cell->balance.nep,cell->output.fire.carbon,
         cell->output.flux_estab.carbon,cell->output.flux_harvest.carbon,delta_tot.carbon,cell->balance.biomass_yield.carbon,
         cell->balance.estab_storage_grass[0].carbon,cell->balance.estab_storage_grass[1].carbon,cell->balance.estab_storage_tree[0].carbon,cell->balance.estab_storage_tree[1].carbon,
         cell->output.deforest_emissions.carbon,cell->output.prod_turnover);
#endif
  }
  if(config->with_nitrogen && year>config->firstyear+1 && fabs(balance.nitrogen)>.2)
  {
#ifdef FAIL
    fail(INVALID_NITROGEN_BALANCE_ERR,TRUE,
#else
    fprintf(stderr,"ERROR032: "
#endif
"N-balance on y %d c %d (%0.2f/%0.2f) BALANCE_N-error %.10f n_influx %g n_outflux %g n_harvest %g n_biomass_yield %g n_estab %g delta_tot=%g total nitrogen=%g estab_storage grass [0] = %g estab_storage grass [1] = %g  estab_storage tree [0] = %g estab_storage tree [1] = %g \n"
#ifndef FAIL
     ".\n"
#endif
      ,year,cellid+config->startgrid,cell->coord.lon,cell->coord.lat,balance.nitrogen,
      cell->balance.n_influx,cell->balance.n_outflux,cell->output.flux_harvest.nitrogen,cell->balance.biomass_yield.nitrogen,cell->output.flux_estab.nitrogen,delta_tot.nitrogen,tot.nitrogen,
      cell->balance.estab_storage_grass[0].nitrogen,cell->balance.estab_storage_grass[1].nitrogen,cell->balance.estab_storage_tree[0].nitrogen,
          cell->balance.estab_storage_tree[1].nitrogen);
    foreachstand(stand,s,cell->standlist){
      foreachpft(pft,p,&stand->pftlist){
        fprintf(stderr,"PFT bm_inc nitrogen %g\n",pft->bm_inc.nitrogen);
      }
    }
  }

  /* water balance check */
  totw=(cell->discharge.dmass_lake+cell->discharge.dmass_river)/cell->coord.area;
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
  balanceW=totw-cell->balance.totw-cell->balance.aprec+cell->balance.awater_flux;
  if(year>config->firstyear+1 && fabs(balanceW)>1.5)
   // fail(INVALID_WATER_BALANCE_ERR,TRUE,"y: %d c: %d (%s) BALANCE_W-error %.2f cell->totw:%.2f totw:%.2f awater_flux:%.2f aprec:%.2f\n",
    fprintf(stderr,"y: %d c: %d (%s) BALANCE_W-error %.2f cell->totw:%.2f totw:%.2f awater_flux:%.2f aprec:%.2f\n",
         year,cellid+config->startgrid,sprintcoord(line,&cell->coord),balanceW,cell->balance.totw,totw,
         cell->balance.awater_flux,cell->balance.aprec);
  cell->balance.totw=totw;
} /* of 'check_fluxes' */

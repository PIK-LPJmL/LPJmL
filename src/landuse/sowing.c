/**************************************************************************************/
/**                                                                                \n**/
/**                   s  o  w  i  n  g  .  c                                       \n**/
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

Stocks sowing(Cell *cell,          /**< cell pointer */
              Real prec,           /**< precipitation (mm) */
              int day,             /**< day (1..365) */
              int year,            /**< year (AD) */
              int npft,            /**< number of natural PFTs */
              int ncft,            /**< number of crop PFTs */
              const Config *config /**< LPJ configuration */
             )                     /** \return establishment flux (gC/m2,g/N/m2) */
{
  Stocks flux_estab={0,0};

#ifdef CHECK_BALANCE
  Stand *stand;
  Stocks start={0,0};
  Stocks fluxes_in,fluxes_out;
  Real end,CH4_fluxes;
  int s;

  fluxes_in.carbon=cell->balance.anpp+cell->balance.flux_estab.carbon+cell->balance.influx.carbon; //influxes
  fluxes_out.carbon=cell->balance.arh+cell->balance.fire.carbon+cell->balance.neg_fluxes.carbon+cell->balance.flux_harvest.carbon+cell->balance.biomass_yield.carbon; //outfluxes
  fluxes_in.nitrogen=cell->balance.flux_estab.nitrogen+cell->balance.influx.nitrogen; //influxes
  fluxes_out.nitrogen=cell->balance.fire.nitrogen+cell->balance.n_outflux+cell->balance.neg_fluxes.nitrogen
      +cell->balance.flux_harvest.nitrogen+cell->balance.biomass_yield.nitrogen+cell->balance.deforest_emissions.nitrogen; //outfluxes
  CH4_fluxes=(cell->balance.aCH4_em)*WC/WCH4;                                 //will be negative, because emissions at the end are higher, thus we have to substract
  foreachstand(stand,s,cell->standlist)
  {
     start.carbon+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
     start.nitrogen+=standstocks(stand).nitrogen*stand->frac;
  }
  start.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  start.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;
#endif
  if(config->sdate_option==NO_FIXED_SDATE ||
    (config->sdate_option==FIXED_SDATE && year<=config->sdate_fixyear))
  {
    update_fallowdays(cell->ml.cropdates,cell->coord.lat,day,ncft);
    /* calling reduced calc_cropdates for computing vern_date20 as needed for vernalization */
    calc_cropdates(config->pftpar+npft,&cell->climbuf,cell->ml.cropdates,
                   cell->coord.lat,day,ncft);

    flux_estab=sowing_season(cell,day,npft,ncft,prec,year,config);
  }
  else
    flux_estab=sowing_prescribe(cell,day,npft,ncft,year,config);
  getoutput(&cell->output,SEEDN_AGR,config)+=flux_estab.nitrogen;
#ifdef CHECK_BALANCE
  end=0;
  fluxes_out.carbon=(cell->balance.arh+cell->balance.fire.carbon+cell->balance.neg_fluxes.carbon
                   +cell->balance.flux_harvest.carbon+cell->balance.biomass_yield.carbon)-fluxes_out.carbon; //outfluxes
  fluxes_in.carbon=(cell->balance.anpp+cell->balance.flux_estab.carbon+cell->balance.influx.carbon)-fluxes_in.carbon;
  CH4_fluxes-=(cell->balance.aCH4_em)*WC/WCH4;                                 //will be negative, because emissions at the end are higher, thus we have to substract
  foreachstand(stand,s,cell->standlist)
  {
    end+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
  }
  end+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
       cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  if(fabs(end-start.carbon-CH4_fluxes+fluxes_out.carbon-fluxes_in.carbon-flux_estab.carbon)>0.001)
  {
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid carbon balance in %s: day: %d    %g start: %g end: %g flux_out.carbon: %g fluxes_in.carbon: %g flux_estab.carbon: %g CH4_fluxes: %g",
          __FUNCTION__,day,end-start.carbon-CH4_fluxes+fluxes_out.carbon-fluxes_in.carbon-flux_estab.carbon,start.carbon,end,fluxes_out.carbon,fluxes_in.carbon,flux_estab.carbon,CH4_fluxes);
  }
  fluxes_out.nitrogen=(cell->balance.fire.nitrogen+cell->balance.n_outflux+cell->balance.neg_fluxes.nitrogen
                      +cell->balance.flux_harvest.nitrogen+cell->balance.biomass_yield.nitrogen+cell->balance.deforest_emissions.nitrogen)-fluxes_out.nitrogen;
  fluxes_in.nitrogen=(cell->balance.flux_estab.nitrogen+cell->balance.influx.nitrogen)-fluxes_in.nitrogen;

  end=0;
  foreachstand(stand,s,cell->standlist)
      end+=standstocks(stand).nitrogen*stand->frac;
  end+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
       cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;
  if(fabs(end-start.nitrogen+fluxes_out.nitrogen-fluxes_in.nitrogen-flux_estab.nitrogen)>0.001)
     fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid nitrogen balance in %s: day: %d    %g start: %.3f  end: %.3f influx: %g outflux: %g flux_estab.nitrogen: %g",
          __FUNCTION__,day,end-start.nitrogen-fluxes_in.nitrogen+fluxes_out.nitrogen-flux_estab.nitrogen,start.nitrogen,end,fluxes_in.nitrogen,fluxes_out.nitrogen,flux_estab.nitrogen);
#endif
  getoutput(&cell->output,FLUX_ESTABN_MG,config)+=flux_estab.nitrogen;
  return flux_estab;
} /* of 'sowing' */

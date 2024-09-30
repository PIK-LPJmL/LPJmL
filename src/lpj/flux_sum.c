/**************************************************************************************/
/**                                                                                \n**/
/**                       f  l  u  x  _  s  u  m  .  c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates global carbon and water fluxes                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real flux_sum(Flux *flux_global,   /**< global carbon and water fluxes */
              Cell grid[],         /**< array of grid cells */
              const Config *config /**< LPJ configuration */
             )                     /** \return total flux (gC) */
{
  int cell;
  int s, p, l;
  Stand *stand;
  Pft *pft;
  //Flux flux={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  Flux flux={};   //TODO reicht das?
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(!grid[cell].skip)
    {
      flux.area+=grid[cell].coord.area;
      flux.temp+=grid[cell].balance.temp/NDAYYEAR*grid[cell].coord.area;
      flux.npp+=grid[cell].balance.anpp*grid[cell].coord.area;
      flux.gpp+=grid[cell].balance.agpp*grid[cell].coord.area;
      flux.rh+=grid[cell].balance.arh*grid[cell].coord.area;
      flux.fire.carbon+=(grid[cell].balance.fire.carbon+grid[cell].balance.deforest_emissions.carbon)*grid[cell].coord.area;
      flux.fire.nitrogen+=(grid[cell].balance.fire.nitrogen+grid[cell].balance.deforest_emissions.nitrogen)*grid[cell].coord.area;
      flux.estab.carbon+=grid[cell].balance.flux_estab.carbon*grid[cell].coord.area;
      flux.estab.nitrogen+=grid[cell].balance.flux_estab.nitrogen*grid[cell].coord.area;
      flux.harvest.carbon+=(grid[cell].balance.flux_harvest.carbon+grid[cell].balance.biomass_yield.carbon)*grid[cell].coord.area;
      flux.harvest.nitrogen+=(grid[cell].balance.flux_harvest.nitrogen+grid[cell].balance.biomass_yield.nitrogen)*grid[cell].coord.area;
      flux.transp+=grid[cell].balance.atransp*grid[cell].coord.area;
      flux.evap+=grid[cell].balance.aevap*grid[cell].coord.area;
      flux.nat_nbpflux+=grid[cell].balance.nat_fluxes*grid[cell].coord.area;
      flux.interc+=grid[cell].balance.ainterc*grid[cell].coord.area;
      flux.wd+=(grid[cell].balance.airrig+grid[cell].balance.aconv_loss_evap+grid[cell].balance.aconv_loss_drain)*grid[cell].coord.area;
#ifdef IMAGE
      flux.wd_unsustainable+=grid[cell].balance.awd_unsustainable*grid[cell].coord.area;
#else
      flux.wd_unsustainable+=grid[cell].balance.awd_unsustainable;
#endif
      flux.evap_lake+=grid[cell].balance.aevap_lake*grid[cell].coord.area;
      flux.evap_res+=grid[cell].balance.aevap_res*grid[cell].coord.area;
      flux.irrig+=grid[cell].balance.airrig*grid[cell].coord.area;
      flux.conv_loss_evap+=grid[cell].balance.aconv_loss_evap*grid[cell].coord.area; /* only evaporated conveyance losses */
      flux.prec+=grid[cell].balance.aprec*grid[cell].coord.area;
      flux.delta_soil_storage+=grid[cell].balance.soil_storage-grid[cell].balance.soil_storage_last;
      grid[cell].balance.soil_storage_last=grid[cell].balance.soil_storage;
      flux.total_reservoir_out+=grid[cell].balance.total_reservoir_out;
      flux.total_irrig_from_reservoir+=grid[cell].balance.total_irrig_from_reservoir;
      flux.wateruse+=grid[cell].balance.awateruse_hil;
      flux.n_demand+=grid[cell].balance.n_demand*grid[cell].coord.area;
      flux.n_uptake+=grid[cell].balance.n_uptake*grid[cell].coord.area;
      flux.influx.carbon+=grid[cell].balance.influx.carbon*grid[cell].coord.area;
      flux.influx.nitrogen+=grid[cell].balance.influx.nitrogen*grid[cell].coord.area;
      flux.n_outflux+=grid[cell].balance.n_outflux*grid[cell].coord.area;
      flux.excess_water+=grid[cell].balance.excess_water*grid[cell].coord.area;
      flux.CH4_emissions+=grid[cell].balance.aCH4_em*grid[cell].coord.area;
      flux.CH4_oxidation+=grid[cell].balance.aCH4_oxid*grid[cell].coord.area;
      flux.CH4_sink+=grid[cell].balance.aCH4_sink*grid[cell].coord.area;
      flux.CH4_rice+=grid[cell].balance.aCH4_rice*grid[cell].coord.area;
      flux.CH4_setaside+=grid[cell].balance.aCH4_setaside*grid[cell].coord.area;
      flux.CH4_fire+=grid[cell].balance.aCH4_fire*grid[cell].coord.area;
      flux.product.carbon+=(grid[cell].ml.product.fast.carbon+grid[cell].ml.product.slow.carbon)*grid[cell].coord.area;
      flux.product.nitrogen+=(grid[cell].ml.product.fast.nitrogen+grid[cell].ml.product.slow.nitrogen)*grid[cell].coord.area;
      flux.product_turnover.carbon+=(grid[cell].balance.prod_turnover.fast.carbon+grid[cell].balance.prod_turnover.slow.carbon)*grid[cell].coord.area;
      flux.product_turnover.nitrogen+=(grid[cell].balance.prod_turnover.fast.nitrogen+grid[cell].balance.prod_turnover.slow.nitrogen)*grid[cell].coord.area;
      flux.neg_fluxes.carbon+=grid[cell].balance.neg_fluxes.carbon*grid[cell].coord.area;
      flux.neg_fluxes.nitrogen+=grid[cell].balance.neg_fluxes.nitrogen*grid[cell].coord.area;
      flux.area_agr+=(grid[cell].ml.cropfrac_rf+grid[cell].ml.cropfrac_ir+grid[cell].ml.cropfrac_wl)*grid[cell].coord.area;
      flux.estab_storage.carbon+=(grid[cell].balance.estab_storage_tree[0].carbon +
                                 grid[cell].balance.estab_storage_tree[1].carbon +
                                 grid[cell].balance.estab_storage_grass[0].carbon +
                                 grid[cell].balance.estab_storage_grass[1].carbon)*grid[cell].coord.area;

      flux.estab_storage.nitrogen+=(grid[cell].balance.estab_storage_tree[0].nitrogen +
                                   grid[cell].balance.estab_storage_tree[1].nitrogen +
                                   grid[cell].balance.estab_storage_grass[0].nitrogen +
                                   grid[cell].balance.estab_storage_grass[1].nitrogen)*grid[cell].coord.area;
      foreachstand(stand,s,grid[cell].standlist)
      {
        flux.lit.carbon+=(litter_agtop_sum(&stand->soil.litter)+litter_agsub_sum(&stand->soil.litter))*stand->frac*grid[cell].coord.area;
        flux.lit.nitrogen+=(litter_agtop_sum_n(&stand->soil.litter)+litter_agsub_sum_n(&stand->soil.litter))*stand->frac*grid[cell].coord.area;
        for (p = 0; p<stand->soil.litter.n; p++)
        {
          flux.soil.carbon+=stand->soil.litter.item[p].bg.carbon*stand->frac*grid[cell].coord.area;
          flux.soil.nitrogen+=stand->soil.litter.item[p].bg.nitrogen*stand->frac*grid[cell].coord.area;
        }
        forrootsoillayer(l)
        {
          flux.soil.carbon+=(stand->soil.pool[l].fast.carbon+stand->soil.pool[l].slow.carbon)*stand->frac*grid[cell].coord.area;
          flux.soil.nitrogen+=(stand->soil.pool[l].fast.nitrogen+stand->soil.pool[l].slow.nitrogen)*stand->frac*grid[cell].coord.area;
          flux.soil_slow.carbon+=stand->soil.pool[l].slow.carbon*stand->frac*grid[cell].coord.area;
          flux.soil_slow.nitrogen+=stand->soil.pool[l].slow.nitrogen*stand->frac*grid[cell].coord.area;
          flux.soil_CH4+=stand->soil.CH4[l]*stand->frac*grid[cell].coord.area;
          flux.soil_NO3+=stand->soil.NO3[l]*stand->frac*grid[cell].coord.area;
          flux.soil_NH4+=stand->soil.NH4[l]*stand->frac*grid[cell].coord.area;
        }
        foreachpft(pft,p,&stand->pftlist)
        {
          flux.veg.carbon+=vegc_sum(pft)*stand->frac*grid[cell].coord.area;
          flux.veg.nitrogen+=(vegn_sum(pft)+pft->bm_inc.nitrogen)*stand->frac*grid[cell].coord.area;
        }
      }
      if(grid[cell].ml.dam)
      {
        flux.soil.carbon+=grid[cell].ml.resdata->pool.carbon*grid[cell].coord.area;
        flux.soil.nitrogen+=grid[cell].ml.resdata->pool.nitrogen*grid[cell].coord.area;
      }
    }
    flux.discharge+=grid[cell].balance.adischarge;
    flux.ext+=grid[cell].discharge.afin_ext;
    flux.delta_surface_storage+=grid[cell].balance.surface_storage-grid[cell].balance.surface_storage_last;
    grid[cell].balance.surface_storage_last=grid[cell].balance.surface_storage;

  } /* of 'for(cell=...)' */


#ifdef USE_MPI
  /*
   * Use global reduce operation in order to sum carbon and water fluxes of all
   * tasks
   */
  MPI_Reduce(&flux,flux_global,sizeof(Flux)/sizeof(Real),
             (sizeof(Real)==sizeof(double)) ? MPI_DOUBLE : MPI_FLOAT,
             MPI_SUM,0,config->comm);
#else
  *flux_global=flux;
#endif
  if(flux_global->area>epsilon)
    flux_global->temp/=flux_global->area;
  return flux_global->npp-flux_global->rh-flux_global->fire.carbon-flux_global->harvest.carbon+flux_global->estab.carbon-flux_global->product_turnover.carbon-flux_global->neg_fluxes.carbon-flux.CH4_emissions*WC/WCH4-flux.CH4_sink*WC/WCH4;
} /* of 'flux_sum' */

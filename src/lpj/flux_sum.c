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
  Flux flux={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(!grid[cell].skip)
    {
      flux.area+=grid[cell].coord.area;
      flux.nep+=(grid[cell].balance.anpp-grid[cell].balance.arh)*grid[cell].coord.area;
      flux.anpp+=grid[cell].balance.anpp*grid[cell].coord.area;
      flux.fire+=(grid[cell].balance.fire.carbon+grid[cell].balance.flux_firewood.carbon)*grid[cell].coord.area;
      flux.estab+=grid[cell].balance.flux_estab.carbon*grid[cell].coord.area;
      flux.harvest+=(grid[cell].balance.flux_harvest.carbon+grid[cell].balance.biomass_yield.carbon)*grid[cell].coord.area;
      flux.transp+=grid[cell].balance.atransp*grid[cell].coord.area;
      flux.evap+=grid[cell].balance.aevap*grid[cell].coord.area;
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
      flux.n_influx+=grid[cell].balance.n_influx*grid[cell].coord.area;
      flux.n_outflux+=grid[cell].balance.n_outflux*grid[cell].coord.area;
      flux.excess_water+=grid[cell].balance.excess_water*grid[cell].coord.area;
      flux.aCH4_emissions+=grid[cell].balance.aCH4_em*grid[cell].coord.area;
      flux.aCH4_sink+=grid[cell].balance.aCH4_sink*grid[cell].coord.area;
      flux.aCH4_fire+=grid[cell].balance.aCH4_fire*grid[cell].coord.area;
      foreachstand(stand,s,grid[cell].standlist)
      {
        for (p = 0; p<stand->soil.litter.n; p++)
          flux.soilc+=(float)(stand->soil.litter.item[p].bg.carbon*stand->frac)*grid[cell].coord.area;
        forrootsoillayer(l)
        {
          flux.soilc+=(float)((stand->soil.pool[l].fast.carbon+stand->soil.pool[l].slow.carbon)*stand->frac*grid[cell].coord.area);
          flux.soilc_slow+=(float)(stand->soil.pool[l].slow.carbon*stand->frac*grid[cell].coord.area);
        }
        foreachpft(pft,p,&stand->pftlist)
          flux.vegc+=(float)(vegc_sum(pft)*stand->frac)*grid[cell].coord.area;
      }
    }
    flux.discharge+=grid[cell].balance.adischarge;
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
  return flux_global->nep-flux_global->fire-flux_global->harvest+flux_global->estab;
} /* of 'flux_sum' */

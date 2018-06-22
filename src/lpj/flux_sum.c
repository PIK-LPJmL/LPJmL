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
  Flux flux={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(!grid[cell].skip)
    {
      flux.area+=grid[cell].coord.area;
      flux.nep+=grid[cell].balance.nep*grid[cell].coord.area;
      flux.fire+=(grid[cell].output.firec+grid[cell].output.flux_firewood)*grid[cell].coord.area;
      flux.estab+=grid[cell].output.flux_estab*grid[cell].coord.area;
      flux.harvest+=(grid[cell].output.flux_harvest+grid[cell].balance.biomass_yield)*grid[cell].coord.area;
      flux.transp+=grid[cell].output.atransp*grid[cell].coord.area;
      flux.evap+=grid[cell].output.aevap*grid[cell].coord.area;
      flux.interc+=grid[cell].output.ainterc*grid[cell].coord.area;
      flux.wd+=(grid[cell].output.airrig+grid[cell].output.aconv_loss_evap+grid[cell].output.aconv_loss_drain)*grid[cell].coord.area;
      flux.wd_unsustainable+=grid[cell].output.awd_unsustainable;
      flux.evap_lake+=grid[cell].output.aevap_lake*grid[cell].coord.area;
      flux.evap_res+=grid[cell].output.aevap_res*grid[cell].coord.area;
      flux.irrig+=grid[cell].output.airrig*grid[cell].coord.area;
      flux.conv_loss_evap+=grid[cell].output.aconv_loss_evap*grid[cell].coord.area; /* only evaporated conveyance losses */
      flux.prec+=grid[cell].balance.aprec*grid[cell].coord.area;
      flux.delta_soil_storage+=grid[cell].output.soil_storage-grid[cell].balance.soil_storage;
      grid[cell].balance.soil_storage=grid[cell].output.soil_storage;
      flux.total_reservoir_out+=grid[cell].balance.total_reservoir_out;
      flux.total_irrig_from_reservoir+=grid[cell].balance.total_irrig_from_reservoir;
      flux.wateruse+=grid[cell].output.awateruse_hil;
    }
    flux.discharge+=grid[cell].output.adischarge;
    flux.delta_surface_storage+=grid[cell].output.surface_storage-grid[cell].balance.surface_storage;
    grid[cell].balance.surface_storage=grid[cell].output.surface_storage;

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

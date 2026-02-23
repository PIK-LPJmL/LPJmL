/**************************************************************************************/
/**                                                                                \n**/
/**              u p d a t e d a i l y _ g r i d . c                               \n**/
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
#include "agriculture.h"  /* for Irrigation struct in debug output */

void updatedaily_grid(Outputfile *output,  /**< Output file data */
                      Cell grid[],         /**< cell array */
                      Extflow extflow,     /**< Pointer to extflow data */
                      int day,             /**< day (1..365) */
                      int month,           /**< month (0..11) */
                      int year,            /**< simulation year (AD) */
                      int npft,            /**< number of natural PFTs */
                      int ncft,            /**< number of crop PFTs */
                      const Config *config /**< LPJ configuration */
                     )
{
#ifdef USE_TIMING
  double tstart;
  timing_start(tstart);
#endif
  if(config->river_routing)
  {
    if(config->withlanduse)
      withdrawal_demand(grid,month,config);
    if(config->extflow)
    {
      if(getextflow(extflow,grid,day-1,year))
        fail(INVALID_EXTFLOW_ERR,FALSE,FALSE,"Cannot read external flow data");
    }
    drain(grid,month,config);

    if(config->withlanduse)
      wateruse(grid,npft,ncft,month,config);
  }

  if(config->withdailyoutput && day<NDAYYEAR && year>=config->outputyear)
    /* postpone last timestep until after annual processes */
  if(fwriteoutput(output,grid,year,day-1,DAILY,npft,ncft,config))
  {
    if(isroot(*config))
      printfailerr(WRITE_OUTPUT_ERR,TRUE,"Cannot write output");
    exit(WRITE_OUTPUT_ERR);
  }

#ifdef CHECK_BALANCE
  /* Daily water balance check - catches balance errors as they occur, works only for one task */
  if(config->river_routing && config->ntask==1)
  {
    int cell;
    Real cum_prec = 0, cum_evap = 0, cum_transp = 0, cum_interc = 0;
    Real cum_evap_lake = 0, cum_evap_res = 0, cum_discharge = 0;
    Real cum_conv_loss = 0, cum_wateruse = 0, cum_excess = 0, cum_MT_water = 0;
    Real total_surface = 0, total_soil = 0;
    Real surface_storage_last_sum = 0, soil_storage_last_sum = 0;
    Real total_area = 0;
    Stand *stand;
    int s;

    for(cell = 0; cell < config->ngridcell; cell++)
    {
      if(!grid[cell].skip)
      {
        total_area += grid[cell].coord.area;
        cum_prec += grid[cell].balance.aprec * grid[cell].coord.area;
        cum_evap += grid[cell].balance.aevap * grid[cell].coord.area;
        cum_transp += grid[cell].balance.atransp * grid[cell].coord.area;
        cum_interc += grid[cell].balance.ainterc * grid[cell].coord.area;
        cum_evap_lake += grid[cell].balance.aevap_lake * grid[cell].coord.area;
        cum_evap_res += grid[cell].balance.aevap_res * grid[cell].coord.area;
        cum_conv_loss += grid[cell].balance.aconv_loss_evap * grid[cell].coord.area;
        cum_wateruse += grid[cell].balance.awateruse_hil;
        cum_excess += grid[cell].balance.excess_water * grid[cell].coord.area;
        cum_MT_water += grid[cell].balance.aMT_water * grid[cell].coord.area;
        soil_storage_last_sum += grid[cell].balance.soil_storage_last;

        /* Calculate current soil water including irrig_stor + irrig_amount for agriculture stands */
        foreachstand(stand, s, grid[cell].standlist)
        {
          total_soil += soilwater(&stand->soil) * stand->frac * grid[cell].coord.area;
          if(stand->data != NULL && isagriculture(stand))
          {
            Irrigation *irrig_data = stand->data;
            total_soil += (irrig_data->irrig_stor + irrig_data->irrig_amount) * stand->frac * grid[cell].coord.area;
          }
        }
      }
      cum_discharge += grid[cell].balance.adischarge;

      /* Surface storage */
      total_surface += grid[cell].discharge.dmass_lake + grid[cell].discharge.dmass_river
                     + grid[cell].discharge.dmass_gw + grid[cell].lateral_water * grid[cell].coord.area;
      if(grid[cell].ml.dam)
        total_surface += grid[cell].ml.resdata->dmass;
      surface_storage_last_sum += grid[cell].balance.surface_storage_last;
    }

    /* Compute running balance */
    Real cum_inputs = cum_prec + cum_MT_water;
    Real cum_outputs = cum_evap + cum_transp + cum_interc + cum_evap_lake + cum_evap_res
                     + cum_discharge + cum_conv_loss + cum_wateruse + cum_excess;
    Real delta_surface = total_surface - surface_storage_last_sum;
    Real delta_soil = total_soil - soil_storage_last_sum;
    Real running_balance = cum_inputs - cum_outputs - delta_surface - delta_soil;
    Real balance_per_area = (total_area > 0) ? running_balance / total_area : 0.0;

    /* Fail if daily balance error exceeds threshold (same threshold as annual check) */
    if(fabs(balance_per_area) > param.error_limit.w_global)
    {
      fail(GLOBAL_WATER_BALANCE_ERR, FAIL_ON_BALANCE, FALSE,
           "Global water balance not closed in year %d day %d: diff=%.5g (%.5g mm), "
           "prec=%.5g, conv_loss=%.5g, delta_surface=%.5g, delta_soil=%.5g",
           year, day, running_balance, balance_per_area,
           cum_prec, cum_conv_loss, delta_surface, delta_soil);
    }
  }
#endif

#ifdef USE_TIMING
  timing_stop(UPDATEDAILY_GRID_FCN,tstart);
#endif
} /* of 'updatedaily_grid' */

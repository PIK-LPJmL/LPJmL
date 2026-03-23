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
#include "agriculture.h"  /* for Irrigation struct in water balance check */

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
  /* Daily global water balance check - works only for single-task runs.
   * Uses daily increments of annual accumulators and daily snapshots of storage.
   * Skipped on the very first day of the simulation because storage_prev is uninitialised. */
  if(config->river_routing && config->ntask==1)
  {
    int cell;
    Real d_prec = 0, d_evap = 0, d_transp = 0, d_interc = 0;
    Real d_evap_lake = 0, d_evap_res = 0, d_discharge = 0;
    Real d_conv_loss = 0, d_wateruse = 0, d_excess = 0, d_MT_water = 0;
    Real total_surface = 0, total_soil = 0;
    Real total_area = 0;
    Stand *stand;
    int s;

    for(cell = 0; cell < config->ngridcell; cell++)
    {
      if(!grid[cell].skip)
      {
        total_area += grid[cell].coord.area;
        /* daily increments = today's annual accumulator minus yesterday's snapshot */
        d_prec              += (grid[cell].balance.aprec           - grid[cell].balance.daily_prec_last)      * grid[cell].coord.area;
        d_evap              += (grid[cell].balance.aevap           - grid[cell].balance.daily_evap_last)      * grid[cell].coord.area;
        d_transp            += (grid[cell].balance.atransp         - grid[cell].balance.daily_transp_last)    * grid[cell].coord.area;
        d_interc            += (grid[cell].balance.ainterc         - grid[cell].balance.daily_interc_last)    * grid[cell].coord.area;
        d_evap_lake         += (grid[cell].balance.aevap_lake      - grid[cell].balance.daily_evap_lake_last) * grid[cell].coord.area;
        d_evap_res          += (grid[cell].balance.aevap_res       - grid[cell].balance.daily_evap_res_last)  * grid[cell].coord.area;
        d_conv_loss         += (grid[cell].balance.aconv_loss_evap - grid[cell].balance.daily_conv_loss_last) * grid[cell].coord.area;
        d_wateruse          += (grid[cell].balance.awateruse_hil   - grid[cell].balance.daily_wateruse_last);
        d_excess            += (grid[cell].balance.excess_water    - grid[cell].balance.daily_excess_last)    * grid[cell].coord.area;
        d_MT_water          += (grid[cell].balance.aMT_water       - grid[cell].balance.daily_MT_water_last)  * grid[cell].coord.area;

        /* update daily snapshots of annual accumulators */
        grid[cell].balance.daily_prec_last           = grid[cell].balance.aprec;
        grid[cell].balance.daily_evap_last           = grid[cell].balance.aevap;
        grid[cell].balance.daily_transp_last         = grid[cell].balance.atransp;
        grid[cell].balance.daily_interc_last         = grid[cell].balance.ainterc;
        grid[cell].balance.daily_evap_lake_last      = grid[cell].balance.aevap_lake;
        grid[cell].balance.daily_evap_res_last       = grid[cell].balance.aevap_res;
        grid[cell].balance.daily_conv_loss_last      = grid[cell].balance.aconv_loss_evap;
        grid[cell].balance.daily_wateruse_last       = grid[cell].balance.awateruse_hil;
        grid[cell].balance.daily_excess_last         = grid[cell].balance.excess_water;
        grid[cell].balance.daily_MT_water_last       = grid[cell].balance.aMT_water;

        /* current soil water storage */
        foreachstand(stand, s, grid[cell].standlist)
          total_soil += soilwater(&stand->soil) * stand->frac * grid[cell].coord.area;
      }
      d_discharge += grid[cell].balance.adischarge - grid[cell].balance.daily_discharge_last;
      grid[cell].balance.daily_discharge_last = grid[cell].balance.adischarge;

      /* current surface storage: includes irrigation buffers (withdrawn from river, not yet in soil) */
      total_surface += grid[cell].discharge.dmass_lake + grid[cell].discharge.dmass_river
                     + grid[cell].discharge.dmass_gw + grid[cell].lateral_water * grid[cell].coord.area;
      if(grid[cell].ml.dam)
      {
        int k;
        total_surface += grid[cell].ml.resdata->dmass;
        for(k=0; k<NIRRIGDAYS; k++)
          total_surface += grid[cell].ml.resdata->dfout_irrigation_daily[k];
      }
      if(!grid[cell].skip)
      {
        foreachstand(stand, s, grid[cell].standlist)
        {
          if(stand->data != NULL && isagriculture(stand))
          {
            Irrigation *irrig_data = stand->data;
            total_surface += (irrig_data->irrig_stor + irrig_data->irrig_amount) * stand->frac * grid[cell].coord.area;
          }
        }
      }
    }

    /* On the very first day storage_prev fields are zero: skip the check but store current storage */
    Bool first_day = (day==1 && year==config->firstyear-config->nspinup);
    Real delta_surface = total_surface - grid[0].balance.daily_surface_prev;
    Real delta_soil    = total_soil    - grid[0].balance.daily_soil_prev;
    /* Store global totals in grid[0] balance fields for next day (single-task only) */
    grid[0].balance.daily_surface_prev = total_surface;
    grid[0].balance.daily_soil_prev    = total_soil;

    if(!first_day)
    {
      Real running_balance = d_prec + d_MT_water - d_evap - d_transp - d_interc - d_evap_lake
                           - d_evap_res - d_discharge - d_conv_loss - d_wateruse - d_excess
                           - delta_surface - delta_soil;
      Real balance_per_area = (total_area > 0) ? running_balance / total_area : 0.0;
      if(fabs(balance_per_area) > param.error_limit.w_global)
        fail(GLOBAL_WATER_BALANCE_ERR, config->fail_on_balance, FALSE,
             "Global water balance not closed in year %d day %d: diff=%.5g (%.5g mm)\n"
             "  prec=%.5g evap=%.5g transp=%.5g interc=%.5g evap_lake=%.5g evap_res=%.5g\n"
             "  discharge=%.5g conv_loss=%.5g wateruse=%.5g excess=%.5g MT_water=%.5g\n"
             "  delta_surface=%.5g delta_soil=%.5g",
             year, day, running_balance, balance_per_area,
             d_prec, d_evap, d_transp, d_interc, d_evap_lake, d_evap_res,
             d_discharge, d_conv_loss, d_wateruse, d_excess, d_MT_water,
             delta_surface, delta_soil);
    }
  }
#endif

#ifdef USE_TIMING
  timing_stop(UPDATEDAILY_GRID_FCN,tstart);
#endif
} /* of 'updatedaily_grid' */

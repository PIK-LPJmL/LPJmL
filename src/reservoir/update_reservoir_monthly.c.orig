/**************************************************************************************/
/**                                                                                \n**/
/**         u p d a t e _ r e s e r v o i r _ m o n t h l y . c                    \n**/
/**                                                                                \n**/
/**     Historic inflow and irrigation water demands are calculated and            \n**/
/**     saved during a defined period HIST_YEARS (defined in reservoir.h)          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void update_reservoir_monthly(Cell *cell, /**< pointer to cell */
                              int month
                              )           /** \return void */
{
  int i,j;

  /* if water left in the 'storage for irrigation', put it back
   * in the reservoir at the end of the month
   */
  /* update history */
  for(i=1;i<HIST_YEARS;i++)
  {
    cell->ml.resdata->inflow_hist[i-1][month]=cell->ml.resdata->inflow_hist[i][month];
    cell->ml.resdata->demand_hist[i-1][month]=cell->ml.resdata->demand_hist[i][month];
    cell->ml.resdata->level_hist[i-1][month]=cell->ml.resdata->level_hist[i][month];
  }
  cell->ml.resdata->inflow_hist[HIST_YEARS-1][month]=cell->discharge.mfin+cell->output.mprec_res*cell->coord.area; /*added the monthly prec to the reservoir as input, should mevap be subtracted?  */
#ifdef DEBUG
  if(cell->discharge.mfin<0 || cell->output.mprec_res<0)
    printf("%g %g\n",cell->discharge.mfin,cell->output.mprec_res);
#endif
  cell->ml.resdata->demand_hist[HIST_YEARS-1][month]=cell->ml.resdata->mdemand; /* changed because the mdemand is now updated every month */
  cell->output.mres_demand=cell->ml.resdata->mdemand/1e9; /*fill output with mdemand in million liters*/
  cell->output.mtarget_release=cell->ml.resdata->target_release_month[month]*1e-9; /*fill output with target release month in 1.000.000 m3/day */
  cell->output.mres_cap=cell->ml.resdata->reservoir.capacity*1e-9; /*fill output with the reservoir capacity in 1.000.000 m3 */
  cell->ml.resdata->mdemand=0.0; /* Reset mdemand to zero at the end of each month*/
  cell->ml.resdata->level_hist[HIST_YEARS-1][month]=cell->ml.resdata->dmass; /* at end of every month store the reservoir mass  */

  /* set the target release if it is the start of the operational year */
  if(month==(cell->ml.resdata->start_operational_year-1)||
     (month==11 && cell->ml.resdata->start_operational_year==0))
  {
     /* NON IRRIGATION RESERVOIRS */
    if(cell->ml.resdata->reservoir.purpose[0]!=2)
    {
      cell->ml.resdata->k_rls=cell->ml.resdata->dmass/(ALPHA*cell->ml.resdata->reservoir.capacity);
      cell->ml.resdata->target_release_year=365*cell->ml.resdata->k_rls*cell->ml.resdata->mean_inflow; /* in liters/yr */
      for(j=0;j<NMONTH;j++)
        cell->ml.resdata->target_release_month[j]=cell->ml.resdata->mean_inflow; /* in liters/d */
    }
    /* IRRIGATION RESERVOIRS */
    if(cell->ml.resdata->reservoir.purpose[0]==2 )/*||cell->ml.resdata->reservoir.purpose[1]==1) */
    {
      cell->ml.resdata->k_rls=cell->ml.resdata->dmass/(ALPHA*cell->ml.resdata->reservoir.capacity);
      cell->ml.resdata->target_release_year=365*cell->ml.resdata->k_rls*cell->ml.resdata->mean_inflow; /* in liters/yr */
      for(j=0;j<NMONTH;j++)
      {
        if (cell->ml.resdata->mean_demand>0.5*cell->ml.resdata->mean_inflow)
          /*cell->ml.resdata->target_release_month[j]=cell->ml.resdata->mean_inflow/10 + cell->ml.resdata->mean_inflow*cell->ml.resdata->mean_demand_month[j]/(ndaymonth[j]*cell->ml.resdata->mean_demand)*9/10;*/ /* in liters/d */
		  cell->ml.resdata->target_release_month[j]=ENV_FLOW*cell->ml.resdata->mean_inflow_month[j]/ndaymonth[j] + ((1-ENV_FLOW)*cell->ml.resdata->mean_inflow/cell->ml.resdata->mean_demand)*(cell->ml.resdata->mean_demand_month[j]/ndaymonth[j]); /* in liters/d */
        else
          cell->ml.resdata->target_release_month[j]=cell->ml.resdata->mean_inflow+cell->ml.resdata->mean_demand_month[j]/ndaymonth[j]-cell->ml.resdata->mean_demand;
      }
    }
  }
} /* of 'update_reservoir_monthly' */

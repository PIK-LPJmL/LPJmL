/**************************************************************************************/
/**                                                                                \n**/
/**          u p d a t e _ r e s e r v o i r _ a n n u a l . c                     \n**/
/**                                                                                \n**/
/**     Every year the start of the operational year is found and the              \n**/
/**     mean annual inflow is calculated based on the reservoirs history.          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void update_reservoir_annual(Cell *cell /**< pointer to cell */
                             )          /** \return void */
{
  int i,j,count,longest,month;
  Bool wetmonth[NMONTH];
  /*
   *  Find the mean annual inflow and the mean monthly inflow and demand over
   *  the past 20 years
   */
  for(i=0;i<NMONTH;i++)
    cell->ml.resdata->mean_inflow_month[i]=cell->ml.resdata->mean_demand_month[i]=cell->ml.resdata->mean_level_month[i]=0;

  count=0;
  cell->ml.resdata->mean_inflow=cell->ml.resdata->mean_demand=cell->ml.resdata->mean_volume=0;

  for(i=0;i<HIST_YEARS;i++)
    if(cell->ml.resdata->inflow_hist[i][0]>=0)
    {
      count++;
      for(j=0;j<NMONTH;j++)
      {
        cell->ml.resdata->mean_inflow+=cell->ml.resdata->inflow_hist[i][j];
        cell->ml.resdata->mean_inflow_month[j]+=cell->ml.resdata->inflow_hist[i][j];
        cell->ml.resdata->mean_demand+=cell->ml.resdata->demand_hist[i][j];
        cell->ml.resdata->mean_demand_month[j]+=cell->ml.resdata->demand_hist[i][j];
        cell->ml.resdata->mean_level_month[j]+=cell->ml.resdata->level_hist[i][j];
        cell->ml.resdata->mean_volume+=cell->ml.resdata->level_hist[i][j];
      }
    }
  cell->ml.resdata->mean_inflow/=(count*365); /* mean inflow in liters/day */
  cell->ml.resdata->mean_demand/=(count*365); /* mean demand in liters/day */
  cell->ml.resdata->mean_volume/=(count*12); /* mean volume in liters */

  for(j=0;j<NMONTH;j++)
  {
    cell->ml.resdata->mean_inflow_month[j]/=count;  /*mean inflow in liters/month */
    cell->ml.resdata->mean_demand_month[j]/=count; /*mean monthly demand in liters/month */
    cell->ml.resdata->mean_level_month[j]/=count; /*mean monthly level in liters */
  }

  /* update the capacity to inflow ratio */
  cell->ml.resdata->c=cell->ml.resdata->reservoir.capacity/(cell->ml.resdata->mean_inflow*365);

  /*
   * Find the start of the operational year based on average inflow over the last 20 years
   * assign 'wet (recharge) or 'dry'(release) to all months (Haddeland)
   */

  /* Find the longest period of consequtive wet months (Haddeland) */
  month=0; /*initialize the start of the operational year with 0. */
  count=0;
  longest=0;
  for (j=0;j<NMONTH;j++)
  {
    wetmonth[j]=(cell->ml.resdata->mean_inflow_month[j]/ndaymonth[j]>=cell->ml.resdata->mean_inflow);
    if(wetmonth[j])
      count++;
    else
    {
      if(count>longest)
      {
        longest=count;
        month=j;
      }
      count=0;
    }
  }
  /* Once more to take care of the new year's (Haddeland) */
  for (j=0;j<NMONTH;j++)
  {
    if(wetmonth[j])
      count++;
    else
    {
      if(count>longest)
      {
        longest=count;
        month=j;
      }
      count=0;
    }
  }
  if(month>11)
    month=11;

  cell->ml.resdata->start_operational_year=month;

} /* of 'update_reservoir_annual' */

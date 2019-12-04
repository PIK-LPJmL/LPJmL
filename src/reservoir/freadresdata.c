/**************************************************************************************/
/**                                                                                \n**/
/**       f  r  e  a  d  r  e  s  d  a  t  a  .  c                                 \n**/
/**                                                                                \n**/
/**     Function reads state variables for reservoirs                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool freadresdata(FILE *file,  /**< pointer to restart file */
                  Cell *cell,  /**< pointer to cell */
                  Bool swap    /**< need to swap data read from restart file */
                  )            /**\return TRUE on error */
{
  cell->ml.resdata=new(Resdata);
  if(cell->ml.resdata==NULL)
    return TRUE;
  /*initialize other characteristics of this reservoir*/
  initresdata(cell);
  /* read from restart file */
  freadreal1(&cell->ml.reservoirfrac,swap,file);
  freadreal((Real *)&cell->ml.resdata->pool,sizeof(Stocks)/sizeof(Real),swap,file);
  freadreal1(&cell->ml.resdata->dmass,swap,file);
  freadreal1(&cell->ml.resdata->k_rls,swap,file);
  freadreal1(&cell->ml.resdata->target_release_year,swap,file);
  freadreal1(&cell->ml.resdata->reservoir.capacity,swap,file); /* reservoir input is only loaded in initreservoir but capacity used in update_reservoir_annual below */
  freadreal(cell->ml.resdata->dfout_irrigation_daily, NIRRIGDAYS, swap, file);
  freadreal(cell->ml.resdata->target_release_month, NMONTH, swap, file);
  freadreal(cell->ml.resdata->demand_hist[0],NMONTH*HIST_YEARS,swap,file);
  freadreal(cell->ml.resdata->inflow_hist[0],NMONTH*HIST_YEARS,swap,file);
  freadreal(cell->ml.resdata->level_hist[0],NMONTH*HIST_YEARS,swap,file);
  /* restore additional reservoir characteristics based on reservoir history */
  update_reservoir_annual(cell);

  return FALSE;
} /* of 'freadresdata' */

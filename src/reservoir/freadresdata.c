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
  cell->ml.resdata->dfout_res=0.0;
  cell->ml.resdata->dfout_irrigation=0.0;
  cell->ml.resdata->c=1.0;
  cell->ml.resdata->mdemand=0.0;
  cell->ml.resdata->ddemand=0.0;
  cell->ml.resdata->demand_fraction=0.0;
  cell->ml.resdata->mean_volume=0.0;
  cell->ml.reservoirfrac=((Real)cell->ml.resdata->reservoir.area)*1000*1000/cell->coord.area; /*area in km2 to m2 */
  /* read from restart file */
  freadreal1(&cell->ml.reservoirfrac,swap,file);
  freadreal1(&cell->ml.resdata->c_pool,swap,file);
  freadreal1(&cell->ml.resdata->dmass,swap,file);
  freadreal1(&cell->ml.resdata->k_rls,swap,file);
  freadreal1(&cell->ml.resdata->target_release_year,swap,file);
  freadreal(cell->ml.resdata->dfout_irrigation_daily, NIRRIGDAYS, swap, file);
  freadreal(cell->ml.resdata->target_release_month, NMONTH, swap, file);
  freadreal(cell->ml.resdata->demand_hist[0],NMONTH*HIST_YEARS,swap,file);
  freadreal(cell->ml.resdata->inflow_hist[0],NMONTH*HIST_YEARS,swap,file);
  freadreal(cell->ml.resdata->level_hist[0],NMONTH*HIST_YEARS,swap,file);
  /* restore additional reservoir characteristics based on reservoir history */
  update_reservoir_annual(cell);

  return FALSE;
} /* of 'freadresdata' */

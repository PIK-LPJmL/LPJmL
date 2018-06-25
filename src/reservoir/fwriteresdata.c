/**************************************************************************************/
/**                                                                                \n**/
/**          f  w  r  i  t  e  r  e  s  d  a  t  a  .  c                           \n**/
/**                                                                                \n**/
/**     Function writes state variables for reservoirs                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fwriteresdata(FILE *file,       /**< pointer to restart file */
                   const Cell *cell  /**< pointer to cell */
                   )                 /** \return TRUE on error */
{
  fwrite(&cell->ml.reservoirfrac,sizeof(Real),1,file);
  fwrite(&cell->ml.resdata->c_pool,sizeof(Real),1,file);
  fwrite(&cell->ml.resdata->dmass,sizeof(Real),1,file);
  fwrite(&cell->ml.resdata->k_rls,sizeof(Real),1,file);
  fwrite(&cell->ml.resdata->target_release_year,sizeof(Real), 1,file);
  fwrite(cell->ml.resdata->dfout_irrigation_daily,sizeof(Real),NIRRIGDAYS,file);
  fwrite(cell->ml.resdata->target_release_month,sizeof(Real),NMONTH,file);
  fwrite(cell->ml.resdata->demand_hist,sizeof(Real),NMONTH*HIST_YEARS,file);
  fwrite(cell->ml.resdata->inflow_hist,sizeof(Real),NMONTH*HIST_YEARS,file);
  return fwrite(cell->ml.resdata->level_hist,sizeof(Real),NMONTH*HIST_YEARS,file)!=NMONTH*HIST_YEARS;
} /* of 'fwriteresdata' */

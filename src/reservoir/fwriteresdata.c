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

static Bool fwritehist(Bstruct file,const char *name,Real hist[HIST_YEARS][NMONTH])
{
  int i;
  bstruct_writearray(file,name,HIST_YEARS);
  for(i=0;i<HIST_YEARS;i++)
    bstruct_writerealarray(file,NULL,hist[i],NMONTH);
  return bstruct_writeendarray(file);
} /* of 'fwritehist' */

Bool fwriteresdata(Bstruct file,     /**< pointer to restart file */
                   const char *name, /**< name of object */
                   const Cell *cell  /**< pointer to cell */
                  )                  /** \return TRUE on error */
{
  bstruct_writestruct(file,name);
  bstruct_writereal(file,"reservoirfrac",cell->ml.reservoirfrac);
  fwritestocks(file,"pool",&cell->ml.resdata->pool);
  bstruct_writereal(file,"dmass",cell->ml.resdata->dmass);
  bstruct_writereal(file,"k_rls",cell->ml.resdata->k_rls);
  bstruct_writereal(file,"target_release_year",cell->ml.resdata->target_release_year);
  bstruct_writefloat(file,"reservoir_capacity",cell->ml.resdata->reservoir.capacity); /* reservoir input is only loaded in initreservoir but capacity used in update_reservoir_annual called from freadresdata */
  bstruct_writerealarray(file,"dfout_irrigation_daily",cell->ml.resdata->dfout_irrigation_daily,NIRRIGDAYS);
  bstruct_writerealarray(file,"target_release_month",cell->ml.resdata->target_release_month,NMONTH);
  fwritehist(file,"demand_hist",cell->ml.resdata->demand_hist);
  fwritehist(file,"inflow_hist",cell->ml.resdata->inflow_hist);
  fwritehist(file,"level_hist",cell->ml.resdata->level_hist);
  return bstruct_writeendstruct(file);
} /* of 'fwriteresdata' */

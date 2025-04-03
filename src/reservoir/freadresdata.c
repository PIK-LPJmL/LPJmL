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

#define readreal2(file,name,val,skip) if(readreal(file,name,val,skip)) return TRUE
#define readfloat2(file,name,val,skip) if(readfloat(file,name,val,skip)) return TRUE
#define readrealarray2(file,name,val,size,skip) if(readrealarray(file,name,val,size,skip)) return TRUE

static Bool freadhist(FILE *file,const char *name,Real hist[HIST_YEARS][NMONTH],Bool swap)
{
  int size,i;
  if(readarray(file,name,&size,swap))
    return TRUE;
  if(size!=HIST_YEARS)
  {
    fprintf(stderr,"ERROR227: Size of %s=%d is not %d.\n",
            name,size,HIST_YEARS);
    return TRUE;
  }
  for(i=0;i<HIST_YEARS;i++)
  {
    if(readrealarray(file,NULL,hist[i],NMONTH,swap))
      return TRUE;
  }
  return readendarray(file);
} /* of 'freadhist' */

Bool freadresdata(FILE *file,  /**< pointer to restart file */
                  const char *name, /**< name of object */
                  Cell *cell,  /**< pointer to cell */
                  Bool swap    /**< need to swap data read from restart file */
                  )            /**\return TRUE on error */
{
  if(readstruct(file,name,swap))
    return TRUE;
  cell->ml.resdata=new(Resdata);
  if(cell->ml.resdata==NULL)
    return TRUE;
  /*initialize other characteristics of this reservoir*/
  cell->ml.resdata->reservoir.area=0; /* to avoid uninitialized variable */
  initresdata(cell);
  /* read from restart file */
  readreal2(file,"reservoirfrac",&cell->ml.reservoirfrac,swap);
  if(readstocks(file,"pool",&cell->ml.resdata->pool,swap))
    return TRUE;
  readreal2(file,"dmass",&cell->ml.resdata->dmass,swap);
  readreal2(file,"k_rls",&cell->ml.resdata->k_rls,swap);
  readreal2(file,"target_release_year",&cell->ml.resdata->target_release_year,swap);
  readfloat2(file,"reservoir_capacity",&cell->ml.resdata->reservoir.capacity,swap); /* reservoir input is only loaded in initreservoir but capacity used in update_reservoir_annual below */
  readrealarray2(file,"dfout_irrigation_daily",cell->ml.resdata->dfout_irrigation_daily, NIRRIGDAYS, swap);
  readrealarray2(file,"target_release_month",cell->ml.resdata->target_release_month, NMONTH, swap);
  if(freadhist(file,"demand_hist",cell->ml.resdata->demand_hist,swap))
    return TRUE;
  if(freadhist(file,"inflow_hist",cell->ml.resdata->inflow_hist,swap))
    return TRUE;
  if(freadhist(file,"level_hist",cell->ml.resdata->level_hist,swap))
    return TRUE;
  /* restore additional reservoir characteristics based on reservoir history */
  update_reservoir_annual(cell);

  return readendstruct(file);
} /* of 'freadresdata' */

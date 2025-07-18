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

#define readreal2(file,name,val) if(bstruct_readreal(file,name,val)) return TRUE
#define readfloat2(file,name,val) if(bstruct_readfloat(file,name,val)) return TRUE
#define readrealarray2(file,name,val,size) if(bstruct_readrealarray(file,name,val,size)) return TRUE

static Bool freadhist(Bstruct file,const char *name,Real hist[HIST_YEARS][NMONTH])
{
  int size,i;
  if(bstruct_readbeginarray(file,name,&size))
    return TRUE;
  if(size!=HIST_YEARS)
  {
    fprintf(stderr,"ERROR227: Size of %s=%d is not %d.\n",
            name,size,HIST_YEARS);
    return TRUE;
  }
  for(i=0;i<HIST_YEARS;i++)
  {
    if(bstruct_readrealarray(file,NULL,hist[i],NMONTH))
      return TRUE;
  }
  return bstruct_readendarray(file,name);
} /* of 'freadhist' */

Bool freadresdata(Bstruct file,     /**< pointer to restart file */
                  const char *name, /**< name of object */
                  Cell *cell        /**< pointer to cell */
                 )                  /**\return TRUE on error */
{
  if(bstruct_readbeginstruct(file,name))
    return TRUE;
  cell->ml.resdata=new(Resdata);
  if(cell->ml.resdata==NULL)
    return TRUE;
  /*initialize other characteristics of this reservoir*/
  cell->ml.resdata->reservoir.area=0; /* to avoid uninitialized variable */
  cell->ml.resdata->fraction=NULL;
  initresdata(cell);
  /* read from restart file */
  readreal2(file,"reservoirfrac",&cell->ml.reservoirfrac);
  if(freadstocks(file,"pool",&cell->ml.resdata->pool))
    return TRUE;
  readreal2(file,"dmass",&cell->ml.resdata->dmass);
  readreal2(file,"k_rls",&cell->ml.resdata->k_rls);
  readreal2(file,"target_release_year",&cell->ml.resdata->target_release_year);
  readfloat2(file,"reservoir_capacity",&cell->ml.resdata->reservoir.capacity); /* reservoir input is only loaded in initreservoir but capacity used in update_reservoir_annual below */
  readrealarray2(file,"dfout_irrigation_daily",cell->ml.resdata->dfout_irrigation_daily, NIRRIGDAYS);
  readrealarray2(file,"target_release_month",cell->ml.resdata->target_release_month, NMONTH);
  if(freadhist(file,"demand_hist",cell->ml.resdata->demand_hist))
    return TRUE;
  if(freadhist(file,"inflow_hist",cell->ml.resdata->inflow_hist))
    return TRUE;
  if(freadhist(file,"level_hist",cell->ml.resdata->level_hist))
    return TRUE;
  /* restore additional reservoir characteristics based on reservoir history */
  update_reservoir_annual(cell);

  return bstruct_readendstruct(file,name);
} /* of 'freadresdata' */

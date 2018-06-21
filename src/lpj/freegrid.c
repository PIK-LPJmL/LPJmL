/**************************************************************************************/
/**                                                                                \n**/
/**                    f  r  e  e  g  r  i  d  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function frees all memory of cell grid                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void freegrid(Cell grid[],         /**< cell grid */
              int npft,            /**< number of natural PFTs */
              const Config *config /**< LPJmL configuration */
             )
{
  int cell;
  /* iterate over all cells */
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(config->river_routing)
    {
      freequeue(grid[cell].discharge.queue);
      free(grid[cell].discharge.tfunct);
    }
    if(grid[cell].ml.irrig_system!=NULL)
    {
      free(grid[cell].ml.irrig_system->crop);
      free(grid[cell].ml.irrig_system);
    }
    if(config->reservoir)
    {
      if(grid[cell].ml.resdata!=NULL)
      {
        free(grid[cell].ml.resdata->fraction);
        free(grid[cell].ml.resdata);
      }
      free(grid[cell].ml.fraction);
    }
    if(grid[cell].ml.landfrac!=NULL)
    {
      freelandfrac(grid[cell].ml.landfrac);
      free(grid[cell].ml.landfrac);
    }
    freeoutput(&grid[cell].output);
    if(!grid[cell].skip)
    {
      if(config->withlanduse!=NO_LANDUSE)
        freemanage(&grid[cell].ml.manage,npft);
      /* free all stands */
      while(!isempty(grid[cell].standlist))
        delstand(grid[cell].standlist,0);

      freelist(grid[cell].standlist);
      freeclimbuf(&grid[cell].climbuf);
      freecropdates(grid[cell].ml.cropdates);
      free(grid[cell].gdd);
      free(grid[cell].ml.sowing_month);
      free(grid[cell].ml.gs);
      if(config->sdate_option>NO_FIXED_SDATE)
        free(grid[cell].ml.sdate_fixed);

#ifdef IMAGE
      free(grid[cell].ml.image_data);
#endif
    }
  } /* of 'for(cell=...)' */
  free(grid);
} /* of 'freegrid' */

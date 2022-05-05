/**************************************************************************************/
/**                                                                                \n**/
/**             r  e  a  d  c  o  t  t  o  n  d  a  y  s  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads sowing and harvest dates for cotton                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool readcottondays(Cell grid[],         /**< LPJ grid */
                    const Config *config /**< LPJ configuration */
                   )                     /** \return TRUE on error */
{
  int cell;
  Bool missing;
  Infile input;
  if(openinputdata(&input,&config->sowing_cotton_rf_filename,"sowing cotton rf",NULL,LPJ_SHORT,1,config))
    return TRUE;
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(readintinputdata(&input,grid[cell].ml.sowing_day_cotton,&missing,&grid[cell].coord,cell+config->startgrid,&config->sowing_cotton_rf_filename))
    {
      closeinput(&input);
      return TRUE;
    }
    if(missing)
      grid[cell].ml.sowing_day_cotton[0]=-1;
  }
  closeinput(&input);
  if(openinputdata(&input,&config->harvest_cotton_rf_filename,"harvest cotton rf",NULL,LPJ_SHORT,1,config))
    return TRUE;
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(readintinputdata(&input,grid[cell].ml.growing_season_cotton,&missing,&grid[cell].coord,cell+config->startgrid,&config->harvest_cotton_rf_filename))
    {
      closeinput(&input);
      return TRUE;
    }
    if(missing)
      grid[cell].ml.growing_season_cotton[0]=-1;
  }
  closeinput(&input);
  if(openinputdata(&input,&config->sowing_cotton_ir_filename,"sowing cotton ir",NULL,LPJ_SHORT,1,config))
    return TRUE;
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(readintinputdata(&input,grid[cell].ml.sowing_day_cotton+1,&missing,&grid[cell].coord,cell+config->startgrid,&config->sowing_cotton_ir_filename))
    {
      closeinput(&input);
      return TRUE;
    }
    if(missing)
      grid[cell].ml.sowing_day_cotton[1]=-1;
  }
  closeinput(&input);
  if(openinputdata(&input,&config->harvest_cotton_ir_filename,"harvest cotton ir",NULL,LPJ_SHORT,1,config))
    return TRUE;
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(readintinputdata(&input,grid[cell].ml.growing_season_cotton+1,&missing,&grid[cell].coord,cell+config->startgrid,&config->harvest_cotton_ir_filename))
    {
      closeinput(&input);
      return TRUE;
    }
    if(missing)
      grid[cell].ml.growing_season_cotton[1]=-1;
  }
  closeinput(&input);

    //if(config->withlanduse==ALL_CROPS)
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(grid[cell].ml.sowing_day_cotton[0]==-1)
      grid[cell].ml.sowing_day_cotton[0]=(grid[cell].ml.sowing_day_cotton[1]==-1) ? 1 : grid[cell].ml.sowing_day_cotton[1];
    if(grid[cell].ml.sowing_day_cotton[1]==-1)
      grid[cell].ml.sowing_day_cotton[1]=(grid[cell].ml.sowing_day_cotton[0]==-1) ? 1 : grid[cell].ml.sowing_day_cotton[0];
    if(grid[cell].ml.growing_season_cotton[0]==-1)
      grid[cell].ml.growing_season_cotton[0]=(grid[cell].ml.growing_season_cotton[1]==-1) ? 364 : grid[cell].ml.growing_season_cotton[1];
    if(grid[cell].ml.growing_season_cotton[1]==-1)
      grid[cell].ml.growing_season_cotton[1]=(grid[cell].ml.growing_season_cotton[0]==-1) ? 364 : grid[cell].ml.growing_season_cotton[0];
  }
  return FALSE;
} /* of 'readcottondays' */

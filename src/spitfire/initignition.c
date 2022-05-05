/**************************************************************************************/
/**                                                                                \n**/
/**            i  n  i  t  i  g  n  i  t  i  o  n  .  c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJ                                                    \n**/
/**                                                                                \n**/
/**     Function reads human ignitions from file                                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool initignition(Cell grid[],         /* LPJ grid */
                  const Config *config /* LPJ configuration */
                 )                     /* returns TRUE on error */
{
  int cell;
  Infile input;
  if(openinputdata(&input,&config->human_ignition_filename,"human ignition","yr-1",LPJ_SHORT,0.001,config))
    return TRUE;
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(readinputdata(&input,&grid[cell].ignition.human,&grid[cell].coord,cell+config->startgrid,&config->human_ignition_filename))
    {
      closeinput(&input);
      return TRUE;
    }
    if(grid[cell].ignition.human<0)
      grid[cell].ignition.human=0;
    grid[cell].ignition.human/=365;
  }
  closeinput(&input);
  return FALSE;
} /* of 'initignition' */

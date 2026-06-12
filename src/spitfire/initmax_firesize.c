/**************************************************************************************/
/**                                                                                \n**/
/**            i  n  i  t  m  a  x  _  f  i  r  e  s  i  z  e  .  c                \n**/
/**                                                                                \n**/
/**     C implementation of LPJ                                                    \n**/
/**                                                                                \n**/
/**     Function reads maximum firesize from file                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool initmax_firesize(Cell grid[],         /* LPJ grid */
                      const Config *config /* LPJ configuration */
                     )                     /* returns TRUE on error */
{
  int cell;
  Infile input;
  if(openinputdata(&input,&config->max_firesize_filename,"maximum firesize","hectare",LPJ_SHORT,0.001,0,config))
    return TRUE;
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(readinputdata(&input,&grid[cell].max_firesize,&grid[cell].coord,cell+config->startgrid,&config->max_firesize_filename))
    {
      closeinput(&input);
      return TRUE;
    }
  }
  closeinput(&input);
  return FALSE;
} /* of 'initmax_firesize' */

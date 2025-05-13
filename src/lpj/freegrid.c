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
    freecell(grid+cell,npft,config);
  } /* of 'for(cell=...)' */
  free(grid);
} /* of 'freegrid' */

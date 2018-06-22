/**************************************************************************************/
/**                                                                                \n**/
/**                    f  r  e  e  c  e  l  l  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void freecell(Cell *cell,        /**< Pointer to cell */
              int npft,          /**< number of natural PFTs */
              Bool river_routing /**< river routing enabled (TRUE/FALSE) */
             )
{
  freemanage(&cell->ml.manage,npft);
  if(river_routing)
    freequeue(cell->discharge.queue);
  if(!cell->skip)
  {
    while(!isempty(cell->standlist))
      delstand(cell->standlist,0);

    freelist(cell->standlist);
    freeclimbuf(&cell->climbuf);
    freecropdates(cell->ml.cropdates);
    freeoutput(&cell->output);
    free(cell->gdd);
    free(cell->ml.sowing_month);
    free(cell->ml.gs);
  }
} /* of 'freecell' */

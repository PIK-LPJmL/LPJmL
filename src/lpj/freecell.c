/**************************************************************************************/
/**                                                                                \n**/
/**                    f  r  e  e  c  e  l  l  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function frees all memory of a cell                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void freecell(Cell *cell,          /**< cell */
              int npft,            /**< number of natural PFTs */
              const Config *config /**< LPJmL configuration */
             )
{
  if(config->river_routing)
  {
    freequeue(cell->discharge.queue);
    free(cell->discharge.tfunct);
  }
  freelandfrac(cell->ml.fertilizer_nr);
  freelandfrac(cell->ml.manure_nr);
  freelandfrac(cell->ml.residue_on_field);
  if(cell->ml.irrig_system!=NULL)
  {
    free(cell->ml.irrig_system->crop);
    free(cell->ml.irrig_system->ag_tree);
    free(cell->ml.irrig_system);
  }
  if(config->reservoir)
  {
    if(cell->ml.resdata!=NULL)
    {
      free(cell->ml.resdata->fraction);
      free(cell->ml.resdata);
    }
    free(cell->ml.fraction);
  }
  freelandfrac(cell->ml.landfrac);
  freeoutput(&cell->output);
  if(!cell->skip)
  {
    if(config->withlanduse!=NO_LANDUSE)
      freemanage(&cell->ml.manage,npft);
    /* free all stands */
    while(!isempty(cell->standlist))
      delstand(cell->standlist,0);

    freelist(cell->standlist);
    freeclimbuf(&cell->climbuf);
    freecropdates(cell->ml.cropdates);
    free(cell->gdd);
    free(cell->ml.sowing_month);
    free(cell->ml.gs);
    if(config->sdate_option>NO_FIXED_SDATE)
      free(cell->ml.sdate_fixed);
    if(config->crop_phu_option>=PRESCRIBED_CROP_PHU)
      free(cell->ml.crop_phu_fixed);

#if defined IMAGE && defined COUPLED
    free(cell->ml.image_data);
    free(cell->pft_harvest);
#endif
  }
} /* of 'freecell' */

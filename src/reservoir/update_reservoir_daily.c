/**************************************************************************************/
/**                                                                                \n**/
/**       u p d a t e _ r e s e r v o i r _ d a i l y . c                          \n**/
/**                                                                                \n**/
/**     Function updates daily reservoir water balance                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void update_reservoir_daily(Cell *cell, /**< pointer to cell */
                            Real prec,  /**< precipitation (mm) */
                            Real eeq    /**< equilibrium evapotranspiration (mm) */
                           )
{
  cell->ml.resdata->dmass+=prec*cell->coord.area*cell->ml.reservoirfrac;
  cell->output.mprec_res+=prec*cell->ml.reservoirfrac;
  if(cell->ml.resdata->dmass>cell->ml.resdata->reservoir.capacity)
  {
    cell->discharge.dmass_lake+=cell->ml.resdata->dmass-cell->ml.resdata->reservoir.capacity;
    cell->ml.resdata->dmass=cell->ml.resdata->reservoir.capacity;
  }
  cell->output.mevap_res+=min(cell->ml.resdata->dmass/cell->coord.area,eeq*PRIESTLEY_TAYLOR*cell->ml.reservoirfrac);
  cell->ml.resdata->dmass=max(cell->ml.resdata->dmass-eeq*PRIESTLEY_TAYLOR*cell->coord.area*cell->ml.reservoirfrac,0.0);
  cell->output.mres_storage+=cell->ml.resdata->dmass;
} /* of 'update_reservoir_daily' */

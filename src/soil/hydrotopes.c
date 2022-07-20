/**************************************************************************************/
/**                                                                                \n**/
/**                           h y d r o t o p e s . c                              \n**/
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

void hydrotopes(Cell *cell /**< Pointer to cell */
               )
{
  Real wtable,wtable_h;
  Stand *stand;
  Bool iswetland;
  int s,w=0;
  iswetland=FALSE;

  wtable=wtable_h=0;
  // 	determine mean water table position

  // Latest formulation: Follow Habets & Saulnier (2001) and use mean wetness as indicator of water table depth.
  foreachstand(stand,s,cell->standlist)
  {
    wtable += stand->soil.wtable*stand->frac*(1.0 / (1 - stand->cell->lakefrac));
    if(stand->type->landusetype==WETLAND || stand->type->landusetype==SETASIDE_WETLAND)
    {
      stand->soil.iswetland=TRUE;
  // 	determine hydrotope water level
      if (!cell->hydrotopes.skip_cell)
      {
        wtable_h+= (iswetland) ? stand->soil.wtable/-1000 : -99;
        w++;
      }
    }
  }
  if(w>0) cell->hydrotopes.wetland_wtable_current=wtable_h/w;
  wtable/=-1000;                            // transform from mm to m, negative values = under surface
  cell->hydrotopes.meanwater=wtable;
} /* of 'hydrotopes' */

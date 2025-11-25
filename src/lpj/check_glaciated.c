/**************************************************************************************/
/**                                                                                \n**/
/**          c  h  e  c  k  _  g  l  a  c  i  a  t  e  d  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     check whether grid cell was glaciated previously                           \n**/
/**     if not: Kill all pfts, add biomass to litter pool                          \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void check_glaciated(Cell *cell,const Config *config)
{
  int s, p;
  Pft *pft;
  Stand *stand;
  if (!cell->was_glaciated)
  {
    foreachstand(stand, s, cell->standlist)
    {
      foreachpft(pft, p, &stand->pftlist)
      {
        litter_update(&stand->soil.litter, pft, pft->nind,config);
        delpft(&stand->pftlist, p);
        p--;
      }
    }
  }
} /* of 'check_glaciated */

// check_glaciated.c
// 
// check whether grid cell was glaciated previously
// if not: Kill all pfts, add biomass to litter pool
// 
// Thomas Kleinen, thomas.kleinen@zmaw.de
// 24/05/2011

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
}

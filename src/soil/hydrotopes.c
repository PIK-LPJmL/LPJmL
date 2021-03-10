/*----------------------------------------------------------*/
/*                                                          */
/*                  hydrotopes.c                            */
/*  hydrotope determination for climLPJ                     */
/*                                                          */
/*  Thomas Kleinen (kleinen@pik-potsdam.de)                 */
/*  18/08/2008                                              */
/*                                                          */
/*                                                          */
/*----------------------------------------------------------*/

#include "lpj.h"

void hydrotopes(Cell *cell /**< Pointer to cell */
               )
{
  Real meanwater;
  Real mean_wet, tmp, wtable;
  Stand *stand;
  Stand *natstand, *wetstand;
  Real efold_current;
  Bool iswetland;
  int s, h, l;
  iswetland = FALSE;
  s = findlandusetype(cell->standlist, WETLAND);            /*COULD BE AGRICULTURE AS WELL BUT NOT YET*/
  if (s != NOT_FOUND)
  {
    wetstand = getstand(cell->standlist, s);
    iswetland = TRUE;
  }

  meanwater = -(soildepth[0] + soildepth[1]);
  h = 0;
  mean_wet = wtable = 0;
  // 	determine mean water table position, use modified Stieglitz et al (1997)
  // 	formulation

  // 	mean_wet = (stand->soil.w[1]*soildepth[1] + stand->soil.w[0]*soildepth[0]) / (soildepth[0] + soildepth[1]);
  // 	meanwater = (-1. + mean_wet) * (soildepth[0] + soildepth[1]) ;

  // Latest formulation: Follow Habets & Saulnier (2001) and use mean wetness as indicator of water table depth.
  foreachstand(stand, s, cell->standlist)
  {
    if (TOPMODEL)
    {
      if (s == 0)
      {
        mean_wet = (stand->soil.w[1] * stand->soil.whcs[1] + stand->soil.w_fw[1] + stand->soil.wpwps[1] + stand->soil.w[0] *
          stand->soil.whcs[0] + stand->soil.w_fw[0] + stand->soil.wpwps[0]) / (stand->soil.wsats[0] + stand->soil.wsats[1]);
        if (mean_wet > hydropar.wtab_thres)
        {
          tmp = (mean_wet - hydropar.wtab_thres) / (1 - hydropar.wtab_thres);
          meanwater = (-1. + tmp) * (soildepth[0] + soildepth[1]);
        }
      }
    }
    else
    {
      h++;
      mean_wet = wtable = 0;
      forrootsoillayer(l)
        mean_wet += (stand->soil.w[l] * stand->soil.whcs[l] + stand->soil.w_fw[l] + stand->soil.wpwps[l] +
          stand->soil.ice_depth[l] + stand->soil.ice_fw[l]) / stand->soil.wsats[l];
      wtable += stand->soil.wtable*stand->frac*(1.0 / (1 - stand->cell->lakefrac));
      if (mean_wet > hydropar.wtab_thres)
      {
        mean_wet /= BOTTOMLAYER;
        tmp = (mean_wet - hydropar.wtab_thres) / (1 - hydropar.wtab_thres);
        meanwater = (-1. + tmp) * (layerbound[BOTTOMLAYER - 1]);
      }
    }
  }

  if (TOPMODEL)
  {
    if (meanwater <= (-(soildepth[0] + soildepth[1])))
      meanwater *= 100000000.;
  }
  else
  {
    wtable /= -1000;         // transform from mm to m, negative values = under surface
  }

  meanwater /= 1000.;		// transform from mm to m

  cell->hydrotopes.meanwater = wtable;

  // 	determine hydrotope water level
  if (!cell->hydrotopes.skip_cell)
  {
    if (TOPMODEL)
    {
      if (cell->hydrotopes.wetland_area <= 0) {
        cell->hydrotopes.wetland_wtable_current = -99.;
      }
      else
      {
        cell->hydrotopes.wetland_wtable_current = meanwater +
          ((cell->hydrotopes.wetland_cti -
            cell->hydrotopes.cti_mean) / stand->soil.par->efold);
      }
    }
    else
    {
      if (iswetland)
        cell->hydrotopes.wetland_wtable_current = wetstand->soil.wtable / -1000;
      else
        cell->hydrotopes.wetland_wtable_current = -99;
    }
  }
}

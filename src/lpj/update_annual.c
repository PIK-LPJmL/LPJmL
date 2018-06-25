/**************************************************************************************/
/**                                                                                \n**/
/**               u  p  d  a  t  e  _  a  n  n  u  a  l  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function performs necessary updates after iteration over one               \n**/
/**     year                                                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void update_annual(Cell *cell,           /**< Pointer to cell */
                   int npft,             /**< number of natural pfts */
                   int ncft,             /**< number of crop pfts */
                   Real popdens,         /**< population density (capita/km2) */
                   int year,               /**< simulation year (AD) */
                   const Real landcover[], /**< array with observed PFT fractions */
                   Bool isdaily,           /**< daily temperature data (TRUE/FALSE) */
                   Bool intercrop,         /**< intercropping (TRUE/FALSE) */
                   const Config *config    /**< LPJ configuration */
                  )
{
  int s;
  Stand *stand;
  if(cell->ml.dam)
    update_reservoir_annual(cell);
  annual_climbuf(&cell->climbuf);
  if(config->sdate_option==NO_FIXED_SDATE ||
    (config->sdate_option==FIXED_SDATE && year<=config->sdate_fixyear)||
    (config->sdate_option==PRESCRIBED_SDATE && year<=config->sdate_fixyear))
    cell->climbuf.atemp_mean20_fix=cell->climbuf.atemp_mean20;

  /* count number of years without harvest
   * and set 20-year mean to zero if no harvest
   * occurred within the last 10 years
   */

    if((year<config->firstyear && config->sdate_option!=PRESCRIBED_SDATE) ||
       config->sdate_option==NO_FIXED_SDATE)
      update_cropdates(cell->ml.cropdates,ncft);

  foreachstand(stand,s,cell->standlist)
  {
    stand->prescribe_landcover = config->prescribe_landcover;
    if (config->prescribe_landcover != NO_LANDCOVER && stand->type->landusetype==NATURAL)
      stand->landcover = landcover;

    stand->soil.mean_maxthaw=(stand->soil.mean_maxthaw-stand->soil.mean_maxthaw/CLIMBUFSIZE)+stand->soil.maxthaw_depth/CLIMBUFSIZE;
    if(annual_stand(stand,npft,ncft,popdens,year,isdaily,intercrop,config))
    {
      /* stand has to be deleted */
      delstand(cell->standlist,s);
      s--; /* adjust stand index */
    }
  } /* of foreachstand */
  foreachstand(stand,s,cell->standlist)
    stand->cell->output.soil_storage+=soilwater(&stand->soil)*stand->frac*stand->cell->coord.area;
  cell->output.fpc[0] = 1-cell->ml.cropfrac_rf-cell->ml.cropfrac_ir-cell->lakefrac-cell->ml.reservoirfrac;
#ifdef IMAGE
  cell->output.prod_turnover=product_turnover(cell->ml.image_data);
  cell->output.product_pool_fast=cell->ml.image_data->timber.fast;
  cell->output.product_pool_slow=cell->ml.image_data->timber.slow;
#endif
} /* of 'update_annual' */

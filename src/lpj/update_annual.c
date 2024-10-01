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

#define N 5

void update_annual(Cell *cell,          /**< Pointer to cell */
                   int npft,            /**< number of natural pfts */
                   int ncft,            /**< number of crop pfts */
                   int year,            /**< simulation year (AD) */
                   Bool isdaily,        /**< daily temperature data (TRUE/FALSE) */
                   Bool intercrop,      /**< intercropping (TRUE/FALSE) */
                   const Config *config /**< LPJ configuration */
                  )
{
  int s,m,cft;
  Stand *stand;
  Pftcroppar *croppar;
  Real mintemp[N];
  Stocks litter_neg;
  if(cell->ml.dam)
    update_reservoir_annual(cell);

  /* Vernalization requirements in case not STATIC_PHU */
  if(config->crop_phu_option!=OLD_CROP_PHU && (config->sdate_option==NO_FIXED_SDATE || year<=config->sdate_fixyear)) /* update only until sdate_fixyear, included NEW_CROP_PHU option for World Modelers */
  {
    getmintemp20_n(&cell->climbuf,mintemp,N);
    for (m=0;m<N;m++)
    {
     for (cft=0;cft<ncft;cft++)
     {
       croppar=config->pftpar[npft+cft].data;
       if (mintemp[m]<=croppar->tv_opt.low && mintemp[m]> -9999)
         cell->climbuf.V_req_a[cft]+=croppar->pvd_max/N; /* maximum number of vernalization days per months */
       else if (mintemp[m]>croppar->tv_opt.low && mintemp[m]<croppar->tv_opt.high)
         cell->climbuf.V_req_a[cft]+=croppar->pvd_max/N*(1-(mintemp[m]-croppar->tv_opt.low)/(croppar->tv_opt.high-croppar->tv_opt.low));
     }
    }
  }

  annual_climbuf(&cell->climbuf,cell->balance.aevap+cell->balance.atransp,ncft,config->crop_phu_option!=OLD_CROP_PHU && (config->sdate_option==NO_FIXED_SDATE || year<=config->sdate_fixyear));
  if(config->sdate_option==NO_FIXED_SDATE ||
    (config->sdate_option==FIXED_SDATE && year<=config->sdate_fixyear)||
    (config->sdate_option>=PRESCRIBED_SDATE && year<=config->sdate_fixyear))
    cell->climbuf.atemp_mean20_fix=cell->climbuf.atemp_mean20;

  /* count number of years without harvest
   * and set 20-year mean to zero if no harvest
   * occurred within the last 10 years
   */

   if((year<config->firstyear && config->sdate_option<PRESCRIBED_SDATE) || config->sdate_option==NO_FIXED_SDATE)
     update_cropdates(cell->ml.cropdates,ncft);

  foreachstand(stand,s,cell->standlist)
  {
    stand->prescribe_landcover = config->prescribe_landcover;

    stand->soil.mean_maxthaw=(stand->soil.mean_maxthaw-stand->soil.mean_maxthaw/CLIMBUFSIZE)+stand->soil.maxthaw_depth/CLIMBUFSIZE;
    if(annual_stand(stand,npft,ncft,year,isdaily,intercrop,config))
    {
      /* stand has to be deleted */
      delstand(cell->standlist,s);
      s--; /* adjust stand index */
    }
  } /* of foreachstand */
  foreachstand(stand,s,cell->standlist)
  {
    litter_neg=checklitter(&stand->soil.litter);
    getoutput(&cell->output,NEGC_FLUXES,config)+=litter_neg.carbon*stand->frac;
    getoutput(&cell->output,NEGN_FLUXES,config)+=litter_neg.nitrogen*stand->frac;
    cell->balance.neg_fluxes.carbon+=litter_neg.carbon*stand->frac;
    cell->balance.neg_fluxes.nitrogen+=litter_neg.nitrogen*stand->frac;
    stand->cell->balance.soil_storage+=soilwater(&stand->soil)*stand->frac*stand->cell->coord.area;
  }
  getoutput(&cell->output,LAND_AREA,config)+=cell->coord.area*(1-cell->lakefrac-cell->ml.reservoirfrac);
  //cell->output.soil_storage+=cell->balance.excess_water*cell->coord.area; /* now tracked in separate flux */
  getoutputindex(&cell->output,FPC,0,config) += 1-cell->ml.cropfrac_rf-cell->ml.cropfrac_ir-cell->lakefrac-cell->ml.reservoirfrac;
#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
    product_turnover(cell,config);
#else
  /* reset product pools after first year with land use to avoid large peak */
  if(year==config->firstyear-config->nspinup&&!config->landuse_restart)
  {
    cell->ml.product.fast.carbon=cell->ml.product.fast.nitrogen=0;
    cell->ml.product.slow.carbon=cell->ml.product.slow.nitrogen=0;
    cell->balance.deforest_emissions.carbon=cell->balance.deforest_emissions.nitrogen=0;
  }
  product_turnover(cell,config);
#endif
} /* of 'update_annual' */

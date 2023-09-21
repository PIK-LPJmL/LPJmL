/**************************************************************************************/
/**                                                                                \n**/
/**                   s  o  w  i  n  g  .  c                                       \n**/
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

Stocks sowing(Cell *cell,          /**< cell pointer */
              Real prec,           /**< precipitation (mm) */
              int day,             /**< day (1..365) */
              int year,            /**< year (AD) */
              int npft,            /**< number of natural PFTs */
              int ncft,            /**< number of crop PFTs */
              const Config *config /**< LPJ configuration */
             )                     /** \return establishment flux (gC/m2,g/N/m2) */
{
  Stocks flux_estab={0,0};
#ifdef CHECK_BALANCE
  Stand *stand;
  Stocks flux;
  Real end, start;
  flux.carbon=cell->balance.anpp+cell->balance.influx.carbon+cell->balance.flux_estab.carbon+flux_estab.carbon;   //is added in cultivate as manure
  int s;
  start=end=0;
  foreachstand(stand,s,cell->standlist)
  {
    start+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
  }
#endif
  if(config->sdate_option==NO_FIXED_SDATE ||
    (config->sdate_option==FIXED_SDATE && year<=config->sdate_fixyear))
  {
    update_fallowdays(cell->ml.cropdates,cell->coord.lat,day,ncft);
    /* calling reduced calc_cropdates for computing vern_date20 as needed for vernalization */
    calc_cropdates(config->pftpar+npft,&cell->climbuf,cell->ml.cropdates,
                   cell->coord.lat,day,ncft);

    flux_estab=sowing_season(cell,day,npft,ncft,prec,year,config);
  }
  else
    flux_estab=sowing_prescribe(cell,day,npft,ncft,year,config);
  getoutput(&cell->output,SEEDN_AGR,config)+=flux_estab.nitrogen;
#ifdef CHECK_BALANCE
  flux.carbon=(cell->balance.anpp+cell->balance.influx.carbon+cell->balance.flux_estab.carbon+flux_estab.carbon)-flux.carbon;
  foreachstand(stand,s,cell->standlist)
  {
    end+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
  }
  if (fabs(end-start-flux.carbon)>0.001)
         fprintf(stdout, "C_ERROR-in sowing: day: %d    %g start: %.3f  end: %.3f flux_estab.carbon: %g flux_carbon: %g \n",
           day,end-start-flux.carbon,start, end,flux_estab.carbon,flux.carbon);

#endif
  return flux_estab;
} /* of 'sowing' */

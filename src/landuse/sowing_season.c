/**************************************************************************************/
/**                                                                                \n**/
/**             s  o  w  i  n  g  _  s  e  a  s  o  n  .  c                        \n**/
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
#include "agriculture.h"
#include "crop.h"

Stocks sowing_season(Cell *cell,          /**< pointer to cell */
                     int day,             /**< day (1..365) */
                     int npft,            /**< number of natural PFTs  */
                     int ncft,            /**< number of crop PFTs */
                     Real dprec,          /**< today's precipitation (mm) */
                     int year,            /**< simulation year (AD) */
                     const Config *config /**< LPJ settings */
                    )                     /** \return establishment flux (gC/m2,gN/m2) */
{
  Bool alloc_today[2]={FALSE,FALSE};
  int i,cft,m,mm,dayofmonth,month,cft_other;
  Stocks flux_estab={0,0};
  const Pftcroppar *croppar;
#ifdef CHECK_BALANCE
  Stand *stand;
  Stocks start={0,0};
  Stocks fluxes_in,fluxes_out;
  Real end,CH4_fluxes;
  int s;

  fluxes_in.carbon=cell->balance.anpp+cell->balance.flux_estab.carbon+cell->balance.influx.carbon; //influxes
  fluxes_out.carbon=cell->balance.arh+cell->balance.fire.carbon+cell->balance.flux_firewood.carbon+cell->balance.neg_fluxes.carbon+cell->balance.flux_harvest.carbon+cell->balance.biomass_yield.carbon; //outfluxes
  fluxes_in.nitrogen=cell->balance.flux_estab.nitrogen+cell->balance.influx.nitrogen; //influxes
  fluxes_out.nitrogen=cell->balance.fire.nitrogen+cell->balance.flux_firewood.nitrogen+cell->balance.n_outflux+cell->balance.neg_fluxes.nitrogen
      +cell->balance.flux_harvest.nitrogen+cell->balance.biomass_yield.nitrogen+cell->balance.deforest_emissions.nitrogen; //outfluxes
  CH4_fluxes=(cell->balance.aCH4_em+cell->balance.aCH4_sink)*WC/WCH4;                                 //will be negative, because emissions at the end are higher, thus we have to substract
  foreachstand(stand,s,cell->standlist)
  {
     start.carbon+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
     start.nitrogen+=standstocks(stand).nitrogen*stand->frac;
  }
  start.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  start.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;
#endif

  if(config->sdate_option==FIXED_SDATE || 
     findlandusetype(cell->standlist,SETASIDE_RF)!=NOT_FOUND ||
     findlandusetype(cell->standlist,SETASIDE_IR)!=NOT_FOUND ||
    findlandusetype(cell->standlist,SETASIDE_WETLAND)!=NOT_FOUND)
 {
    if(config->others_to_crop)
       cft_other=(fabs(cell->coord.lat)>30) ? config->cft_temp : config->cft_tropic;
    else
       cft_other=-1;
    for(cft=0; cft<ncft; cft++)
    {
      croppar=config->pftpar[npft+cft].data;
      cvrtdaymonth(&dayofmonth,&month,day);
      //if(year==1880)printf("sowingseason: CFT: cft_other: %d %d seasonality: %d sowing_month: %d %d\n",cft,cft_other,cell->ml.seasonality_type,cell->ml.sowing_month[cft],cell->ml.sowing_month[cft+ncft]);
      switch(cell->ml.seasonality_type)
      {
        case NO_SEASONALITY:
          if(dayofmonth==1)
          {
            if(month==cell->ml.sowing_month[cft])
              sowingcft(&flux_estab,alloc_today,cell,FALSE,FALSE,FALSE,npft,ncft,cft,year,day,FALSE,config);
            if(month==cell->ml.sowing_month[cft+ncft])
              sowingcft(&flux_estab,alloc_today+1,cell,TRUE,FALSE,FALSE,npft,ncft,cft,year,day,FALSE,config);
            if(cft==cft_other)
            {
              if(month==cell->ml.sowing_month[cft])
                sowingcft(&flux_estab,alloc_today,cell,FALSE,FALSE,FALSE,npft,ncft,cft,year,day,TRUE,config);
              if(month==cell->ml.sowing_month[cft+ncft])
                sowingcft(&flux_estab,alloc_today+1,cell,TRUE,FALSE,FALSE,npft,ncft,cft,year,day,TRUE,config);
            }
          } /*of no seasonality*/
          break;
        case PRECIP: case PRECIPTEMP:
          if(dprec>MIN_PREC || dayofmonth==ndaymonth[month-1])
          {
            if(month==cell->ml.sowing_month[cft]) /*no irrigation, first wet day*/
              sowingcft(&flux_estab,alloc_today,cell,FALSE,FALSE,FALSE,npft,ncft,cft,year,day,FALSE,config);
            if(month==cell->ml.sowing_month[cft+ncft]) /* irrigation, first wet day*/
              sowingcft(&flux_estab,alloc_today+1,cell,TRUE,FALSE,FALSE,npft,ncft,cft,year,day,FALSE,config);
            if(cft==cft_other)
            {
              if(month==cell->ml.sowing_month[cft]) /*no irrigation, first wet day*/
                sowingcft(&flux_estab,alloc_today,cell,FALSE,FALSE,FALSE,npft,ncft,cft,year,day,TRUE,config);
              if(month==cell->ml.sowing_month[cft+ncft]) /* irrigation, first wet day*/
                sowingcft(&flux_estab,alloc_today+1,cell,TRUE,FALSE,FALSE,npft,ncft,cft,year,day,TRUE,config);
            }
          } /*of precipitation seasonality*/
          break;
        case TEMPERATURE: case TEMPPRECIP:
          for(i=0;i<2;i++)
            if(month==cell->ml.sowing_month[cft+i*ncft])
            {
              m=month-1; /*m runs from 0 to 11*/
              mm=(m-1<0) ? NMONTH-1 : m-1; /*mm is the month before*/
              if(cell->climbuf.mtemp20[mm]>cell->climbuf.mtemp20[m]&&croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE)
              {
                /*calculate day when temperature exceeds or falls below a crop-specific temperature threshold - from former function calc_cropdates*/
                if(((cell->climbuf.temp[NDAYS-1]<croppar->temp_fall)
                   &&(cell->climbuf.temp[NDAYS-2]>=croppar->temp_fall||dayofmonth==1))||dayofmonth==ndaymonth[m]) /*sow winter variety*/
                {
                  sowingcft(&flux_estab,alloc_today+i,cell,i,TRUE,FALSE,npft,ncft,cft,year,day,FALSE,config);
                  if(cft==cft_other)
                    sowingcft(&flux_estab,alloc_today+i,cell,i,TRUE,FALSE,npft,ncft,cft,year,day,TRUE,config);
                }
              }
              else if(((cell->climbuf.temp[NDAYS-1]>croppar->temp_spring)
                      &&(cell->climbuf.temp[NDAYS-2]<=croppar->temp_spring||dayofmonth==1))||dayofmonth==ndaymonth[m]) /*sow summer variety */
              {
                sowingcft(&flux_estab,alloc_today+i,cell,i,FALSE,FALSE,npft,ncft,cft,year,day,FALSE,config);
                if(cft==cft_other)
                  sowingcft(&flux_estab,alloc_today+i,cell,i,FALSE,FALSE,npft,ncft,cft,year,day,TRUE,config);
              } /*of cultivating summer variety*/
            } /*of if month==ml.sowing_month[cft]*/
          break; /* of temperature-dependent rule */
      } /* of switch() */
    }  /* for(cft=...) */
  }

#ifdef CHECK_BALANCE
  end=0;
  fluxes_out.carbon=(cell->balance.arh+cell->balance.fire.carbon+cell->balance.flux_firewood.carbon+cell->balance.neg_fluxes.carbon
                   +cell->balance.flux_harvest.carbon+cell->balance.biomass_yield.carbon)-fluxes_out.carbon; //outfluxes
  fluxes_in.carbon=(cell->balance.anpp+cell->balance.flux_estab.carbon+cell->balance.influx.carbon)-fluxes_in.carbon;
  CH4_fluxes-=(cell->balance.aCH4_em+cell->balance.aCH4_sink)*WC/WCH4;                                 //will be negative, because emissions at the end are higher, thus we have to substract
  foreachstand(stand,s,cell->standlist)
  {
    end+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
  }
  end+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
       cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  if(fabs(end-start.carbon-CH4_fluxes+fluxes_out.carbon-fluxes_in.carbon-flux_estab.carbon)>0.1)
  {
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid carbon balance in %s: day: %d    %g start: %g end: %g flux_out.carbon: %g fluxes_in.carbon: %g flux_estab.carbon: %g CH4_fluxes: %g",
         __FUNCTION__,day,end-start.carbon-CH4_fluxes+fluxes_out.carbon-fluxes_in.carbon-flux_estab.carbon,start.carbon,end,fluxes_out.carbon,fluxes_in.carbon,flux_estab.carbon,CH4_fluxes);
  }
  fluxes_out.nitrogen=(cell->balance.fire.nitrogen+cell->balance.flux_firewood.nitrogen+cell->balance.n_outflux+cell->balance.neg_fluxes.nitrogen
                      +cell->balance.flux_harvest.nitrogen+cell->balance.biomass_yield.nitrogen+cell->balance.deforest_emissions.nitrogen)-fluxes_out.nitrogen;
  fluxes_in.nitrogen=(cell->balance.flux_estab.nitrogen+cell->balance.influx.nitrogen)-fluxes_in.nitrogen;

  end=0;
  foreachstand(stand,s,cell->standlist)
    end+=standstocks(stand).nitrogen*stand->frac;
  end+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
     cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;
  if(fabs(end-start.nitrogen+fluxes_out.nitrogen-fluxes_in.nitrogen-flux_estab.nitrogen)>0.001)
      fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid nitrogen balance in %s: day: %d    %g start: %.3f  end: %.3f influx: %g outflux: %g flux_estab.nitrogen: %g",
           __FUNCTION__,day,end-start.nitrogen-fluxes_in.nitrogen+fluxes_out.nitrogen-flux_estab.nitrogen,start.nitrogen,end,fluxes_in.nitrogen,fluxes_out.nitrogen,flux_estab.nitrogen);
#endif
  return flux_estab;
} /* of 'sowing_season' */

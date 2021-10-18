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
  int cft,m,mm,dayofmonth,month,s,s2;
  Stocks flux_estab={0,0};
  const Pftcroppar *croppar;

  s=findlandusetype(cell->standlist,SETASIDE_RF);
  s2=findlandusetype(cell->standlist,SETASIDE_IR);
  if(s!=NOT_FOUND || s2!=NOT_FOUND)
  {
    for(cft=0; cft<ncft; cft++)
    {
      croppar=config->pftpar[npft+cft].data;
      cvrtdaymonth(&dayofmonth,&month,day);

      if(cell->ml.seasonality_type==NO_SEASONALITY && dayofmonth==1) /*no seasonality*/
      {
        if(month==cell->ml.sowing_month[cft])
          sowingcft(&flux_estab,alloc_today,cell,FALSE,FALSE,npft,ncft,cft,year,day,config);
        if(month==cell->ml.sowing_month[cft+ncft])
          sowingcft(&flux_estab,alloc_today+1,cell,TRUE,FALSE,npft,ncft,cft,year,day,config);
      } /*of no seasonality*/

      if (cell->ml.seasonality_type==PRECIP || cell->ml.seasonality_type==PRECIPTEMP) /*precipitation-dependent rules*/
      {
        if(dprec>MIN_PREC || dayofmonth==ndaymonth[month-1])
        {
          if(month==cell->ml.sowing_month[cft]) /*no irrigation, first wet day*/
            sowingcft(&flux_estab,alloc_today,cell,FALSE,FALSE,npft,ncft,cft,year,day,config);
          if(month==cell->ml.sowing_month[cft+ncft]) /* irrigation, first wet day*/
            sowingcft(&flux_estab,alloc_today+1,cell,TRUE,FALSE,npft,ncft,cft,year,day,config);
        }
      } /*of precipitation seasonality*/

      if (cell->ml.seasonality_type==TEMPERATURE || cell->ml.seasonality_type==TEMPPRECIP) /*temperature-dependent rule*/
      {
        if(month==cell->ml.sowing_month[cft]) /* no irrigation */
        {
          m=month-1; /*m runs from 0 to 11*/
          mm=(m-1<0) ? NMONTH-1 : m-1; /*mm is the month before*/
          if(cell->climbuf.mtemp20[mm]>cell->climbuf.mtemp20[m]&&croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE)
          {
            /*calculate day when temperature exceeds or falls below a crop-specific temperature threshold - from former function calc_cropdates*/
            if(((cell->climbuf.temp[NDAYS-1]<croppar->temp_fall)
               &&(cell->climbuf.temp[NDAYS-2]>=croppar->temp_fall||dayofmonth==1))||dayofmonth==ndaymonth[m]) /*sow winter variety*/
            {
              sowingcft(&flux_estab,alloc_today,cell,FALSE,FALSE,npft,ncft,cft,year,day,config);
            }
          }
          else if(((cell->climbuf.temp[NDAYS-1]>croppar->temp_spring)
                  &&(cell->climbuf.temp[NDAYS-2]<=croppar->temp_spring||dayofmonth==1))||dayofmonth==ndaymonth[m]) /*sow summer variety */
          {
            sowingcft(&flux_estab,alloc_today,cell,FALSE,FALSE,npft,ncft,cft,year,day,config);
          } /*of cultivating summer variety*/
        } /*of if month==ml.sowing_month[cft]*/
        if(month==cell->ml.sowing_month[cft+ncft]) /* irrigation */
        {
          m=month-1; /*m runs from 0 to 11*/
          mm=(m-1<0) ? NMONTH-1 : m-1; /*mm is the month before*/
          if(cell->climbuf.mtemp20[mm]>cell->climbuf.mtemp20[m]&&croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE)
          {
            /*calculate day when temperature exceeds or falls below a crop-specific temperature threshold - from former function calc_cropdates*/
            if(((cell->climbuf.temp[NDAYS-1]<croppar->temp_fall)
               &&(cell->climbuf.temp[NDAYS-2]>=croppar->temp_fall||dayofmonth==1))||dayofmonth==ndaymonth[m]) /*sow winter variety*/
            {
              sowingcft(&flux_estab,alloc_today+1,cell,TRUE,FALSE,npft,ncft,cft,year,day,config);
            }
          }
          else if(((cell->climbuf.temp[NDAYS-1]>croppar->temp_spring)
                  &&(cell->climbuf.temp[NDAYS-2]<=croppar->temp_spring||dayofmonth==1))||dayofmonth==ndaymonth[m]) /*sow summer variety */
          {
            sowingcft(&flux_estab,alloc_today+1,cell,TRUE,FALSE,npft,ncft,cft,year,day,config);
          } /*of cultivating summer variety*/
        } /*of if month==ml.sowing_month[cft+npft]*/
      } /* of temperature-dependent rule */
    }  /* for(cft=...) */
  }
  return flux_estab;
} /* of 'sowing_season' */

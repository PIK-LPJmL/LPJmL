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

static void cultcftstand(Stocks *flux_estab, /**< establishment flux */
                         Bool *alloc_today,
                         Cell *cell,          /**< pointer to cell */
                         Stand *setasidestand,/** pointer to setaside stand */
                         Bool irrig,          /** stand irrigated? (TRUE/FALSE) */
                         int npft,            /**< number of natural PFTs  */
                         int ncft,            /**< number of crop PFTs */
                         int cft,
                         int day,             /**< day (1..365) */
                         int year,            /**< simulation year (AD) */
                         const Config *config /**< LPJmL settings */
                        )
{
  Stocks stocks;
  if(cell->ml.cropdates[cft].fallow[irrig]<=0 &&
     check_lu(cell->standlist,cell->ml.landfrac[irrig].crop[cft],npft+cft,irrig))
  {
    if(!(*alloc_today))
    {
      allocation_today(setasidestand,config);
      *alloc_today=TRUE;
    }
    stocks=cultivate(cell,irrig,day,FALSE,setasidestand,
                     npft,ncft,cft,year,config);
    flux_estab->carbon+=stocks.carbon;
    flux_estab->nitrogen+=stocks.nitrogen;
    if(irrig)
      cft+=ncft;
    if(!config->double_harvest)
      getoutputindex(&cell->output,SDATE,cft,config)=day;
    if(config->sdate_option==FIXED_SDATE)
      cell->ml.sdate_fixed[cft]=day;
  }//if check lu
} /* of 'cultcftstand' */

static void sowingcft(Stocks *flux_estab, /**< establishment flux */
                     Bool *alloc_today,
                     Cell *cell,          /**< pointer to cell */
                     Bool irrig,          /** stand is irrigated? */
                     int npft,            /**< number of natural PFTs  */
                     int ncft,            /**< number of crop PFTs */
                     int cft,
                     int year,            /**< simulation year (AD) */
                     int day,             /**< day (1..365) */
                     const Config *config /**< LPJmL settings */
                    )
{
  Stand *stand,*cropstand,*setasidestand;
  Pft *pft;
  Real difffrac;
  int s,p,cft_id,pos;
  Irrigation *irrigation,*data;
  s=findlandusetype(cell->standlist,(irrig) ? SETASIDE_IR : SETASIDE_RF);

  if(s!=NOT_FOUND)
  {
    setasidestand=getstand(cell->standlist,s);
    cultcftstand(flux_estab,alloc_today,cell,setasidestand,irrig,npft,ncft,cft,year,day,config);
  }
  else
  {
    difffrac=0;
    foreachstand(stand,s,cell->standlist)
    {
      if(stand->type->landusetype==AGRICULTURE)
      {
        irrigation=stand->data;
        // determine PFT-ID of crop grown here (use last as there is only one crop per cropstand)
        foreachpft(pft,p,&stand->pftlist)
          cft_id=pft->par->id-npft;
        if(irrigation->irrigation==irrig && stand->frac > (2*tinyfrac+epsilon) && stand->frac > (cell->ml.landfrac[irrig].crop[cft_id]+epsilon))
        {
          difffrac=min(stand->frac-tinyfrac,stand->frac-cell->ml.landfrac[irrig].crop[cft_id]);
          pos=addstand(&agriculture_stand,cell);
          cropstand=getstand(cell->standlist,pos-1);
          data=cropstand->data;
          cropstand->frac=difffrac;
          stand->frac-=difffrac;
          //printf("taking %g from %g of %s\n",difffrac,stand->frac+difffrac,pft->par->name);
          data->irrigation=irrig;
          reclaim_land(stand,cropstand,cell,config->istimber,npft+ncft,config);
          set_irrigsystem(cropstand,cft,npft,ncft,config);
          setaside(cell,cropstand,cell->ml.with_tillage,config->intercrop,npft,irrig,year,config);
          setasidestand=getstand(cell->standlist,pos-1);
          cultcftstand(flux_estab,alloc_today,cell,setasidestand,irrig,npft,ncft,cft,year,day,config);
        }//if too large stand->frac
      } // if AGRICULTURE
      if(difffrac>epsilon)
        break;
    } // foreachstand
  }
} /* of 'sowingcft' */

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
  if(s!=NOT_FOUND||s2!=NOT_FOUND)
  {
    for(cft=0; cft<ncft; cft++)
    {
      croppar=config->pftpar[npft+cft].data;
      cvrtdaymonth(&dayofmonth,&month,day);

      if(cell->ml.seasonality_type==NO_SEASONALITY && dayofmonth==1) /*no seasonality*/
      {
        if(month==cell->ml.sowing_month[cft])
          sowingcft(&flux_estab,alloc_today,cell,FALSE,npft,ncft,cft,year,day,config);
        if(month==cell->ml.sowing_month[cft+ncft])
          sowingcft(&flux_estab,alloc_today+1,cell,TRUE,npft,ncft,cft,year,day,config);
      } /*of no seasonality*/

      if (cell->ml.seasonality_type==PRECIP || cell->ml.seasonality_type==PRECIPTEMP) /*precipitation-dependent rules*/
      {
        if(dprec>MIN_PREC || dayofmonth==ndaymonth[month-1])
        {
          if(month==cell->ml.sowing_month[cft]) /*no irrigation, first wet day*/
            sowingcft(&flux_estab,alloc_today,cell,FALSE,npft,ncft,cft,year,day,config);
          if(month==cell->ml.sowing_month[cft+ncft]) /* irrigation, first wet day*/
            sowingcft(&flux_estab,alloc_today+1,cell,TRUE,npft,ncft,cft,year,day,config);
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
              sowingcft(&flux_estab,alloc_today,cell,FALSE,npft,ncft,cft,year,day,config);
            }
          }
          else if(((cell->climbuf.temp[NDAYS-1]>croppar->temp_spring)
                  &&(cell->climbuf.temp[NDAYS-2]<=croppar->temp_spring||dayofmonth==1))||dayofmonth==ndaymonth[m]) /*sow summer variety */
          {
            sowingcft(&flux_estab,alloc_today,cell,FALSE,npft,ncft,cft,year,day,config);
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
              sowingcft(&flux_estab,alloc_today+1,cell,TRUE,npft,ncft,cft,year,day,config);
            }
          }
          else if(((cell->climbuf.temp[NDAYS-1]>croppar->temp_spring)
                  &&(cell->climbuf.temp[NDAYS-2]<=croppar->temp_spring||dayofmonth==1))||dayofmonth==ndaymonth[m]) /*sow summer variety */
          {
            sowingcft(&flux_estab,alloc_today+1,cell,TRUE,npft,ncft,cft,year,day,config);
          } /*of cultivating summer variety*/
        } /*of if month==ml.sowing_month[cft+npft]*/
      } /* of temperature-dependent rule */
    }  /* for(cft=...) */
  }
  return flux_estab;
} /* of 'sowing_season' */

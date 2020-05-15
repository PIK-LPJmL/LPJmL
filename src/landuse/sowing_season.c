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
#include "crop.h"
#include "agriculture.h"

Stocks sowing_season(Cell *cell,            /**< pointer to cell */
  int day,               /**< day (1..365) */
  int npft,              /**< number of natural PFTs  */
  int ncft,              /**< number of crop PFTs */
  Real dprec,            /**< today's precipitation (mm) */
  int year,              /**< simulation year (AD) */
  const Config *config   /**< LPJ settings */
)                       /** \return establish flux (gC/m2,gN/m2) */
{
  Bool alloc_today_rf=FALSE,alloc_today_ir=FALSE,istimber;
  int cft,m,mm,dayofmonth,month,s,s2;
  Stocks flux_estab={0,0},stocks;
  const Pftcroppar *croppar;
  Stand *setasidestand;
  Pft *pft;
  int p,pos,cft_id;
  Real difffrac=0;
  Stand *stand, *cropstand;
  Irrigation *irrigation, *data;

#ifdef IMAGE
  istimber=(config->start_imagecoupling!=INT_MAX);
#else
  istimber=FALSE;
#endif
  s=findlandusetype(cell->standlist,SETASIDE_RF);
  s2=findlandusetype(cell->standlist,SETASIDE_IR);
  if(s!=NOT_FOUND||s2!=NOT_FOUND)
  {

    for(cft=0; cft<ncft; cft++)
    {
      croppar=config->pftpar[npft+cft].data;
      cvrtdaymonth(&dayofmonth,&month,day);

      if(cell->ml.seasonality_type==NO_SEASONALITY) /*no seasonality*/
      {
        if(month==cell->ml.sowing_month[cft]&&dayofmonth==1)
        {
          /*rainfed CFTs*/
          s=findlandusetype(cell->standlist,SETASIDE_RF);

          if(s!=NOT_FOUND)
          {
            setasidestand=getstand(cell->standlist,s);
            if(cell->ml.cropdates[cft].fallow<=0&&
              check_lu(cell->standlist,cell->ml.landfrac[0].crop[cft],npft+cft,FALSE))
            {
              if(!alloc_today_rf)
              {
                allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                alloc_today_rf=TRUE;
              }
              stocks=cultivate(cell,config->pftpar+npft+cft,
                cell->ml.cropdates[cft].vern_date20,
                cell->ml.landfrac[0].crop[cft],FALSE,day,FALSE,
                setasidestand,cell->ml.with_tillage,istimber,config,
                npft,ncft,cft,year);
              flux_estab.carbon+=stocks.carbon;
              flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
              cell->output.sdate[cft]=day;
#endif
              if(config->sdate_option==FIXED_SDATE)
                cell->ml.sdate_fixed[cft]=day;
            }
          }
          else
          {
            difffrac=0;
            foreachstand(stand,s,cell->standlist){
              if(stand->type->landusetype==AGRICULTURE){
                irrigation=stand->data;
                // determine PFT-ID of crop grown here (use last as there is only one crop per cropstand)
                foreachpft(pft,p,&stand->pftlist)
                  cft_id=pft->par->id-npft;
                if(irrigation->irrigation==FALSE && (stand->frac>cell->ml.landfrac[irrigation->irrigation].crop[cft_id]+epsilon))
                {
                  difffrac=min(stand->frac-tinyfrac,stand->frac-cell->ml.landfrac[irrigation->irrigation].crop[cft_id]);
                  pos=addstand(&agriculture_stand,cell);
                  cropstand=getstand(cell->standlist,pos-1);
                  data=cropstand->data;
                  cropstand->frac=difffrac;
                  stand->frac-=difffrac;
                  //printf("taking %g from %g of %s\n",difffrac,stand->frac+difffrac,pft->par->name);
                  data->irrigation=irrigation->irrigation;
                  reclaim_land(stand,cropstand,cell,istimber,npft+ncft);
                  set_irrigsystem(cropstand,cft,ncft,config->pft_output_scaled);
                  setaside(cell,cropstand,config->pftpar,cell->ml.with_tillage,config->intercrop,npft,irrigation->irrigation,year,config->with_nitrogen);
                  setasidestand=getstand(cell->standlist,pos-1);
                  if(check_lu(cell->standlist,cell->ml.landfrac[irrigation->irrigation].crop[cft],npft+cft,irrigation->irrigation))
                  {
                    if(!alloc_today_rf)
                    {
                      allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                      alloc_today_rf=TRUE;
                    }
                    stocks=cultivate(cell,config->pftpar+npft+cft,
                      cell->ml.cropdates[cft].vern_date20,
                      cell->ml.landfrac[irrigation->irrigation].crop[cft],irrigation->irrigation,day,FALSE,
                      setasidestand,cell->ml.with_tillage,istimber,config,
                      npft,ncft,cft,year);
                    flux_estab.carbon+=stocks.carbon;
                    flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
                    cell->output.sdate[cft]=day;
#endif
                    if(config->sdate_option==FIXED_SDATE)
                      cell->ml.sdate_fixed[cft]=day;
                  }//if check lu
                }//if too large stand->frac
              } // if AGRICULTURE
              if(difffrac>epsilon)
                break;
            } // foreachstand
          }/*of rainfed CFTs*/
          /*irrigated CFTs*/
          s=findlandusetype(cell->standlist,SETASIDE_IR);
          if(s!=NOT_FOUND)
          {
            setasidestand=getstand(cell->standlist,s);
            if(cell->ml.cropdates[cft].fallow_irrig<=0&&
              check_lu(cell->standlist,cell->ml.landfrac[1].crop[cft],npft+cft,TRUE))
            {
              if(!alloc_today_ir)
              {
                allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                alloc_today_ir=TRUE;
              }
              stocks=cultivate(cell,config->pftpar+npft+cft,
                cell->ml.cropdates[cft].vern_date20,
                cell->ml.landfrac[1].crop[cft],TRUE,day,FALSE,
                setasidestand,cell->ml.with_tillage,istimber,config,
                npft,ncft,cft,year);
              flux_estab.carbon+=stocks.carbon;
              flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
              cell->output.sdate[cft+ncft]=day;
#endif
              if(config->sdate_option==FIXED_SDATE)
                cell->ml.sdate_fixed[cft+ncft]=day;
            }
          }
          else
          {
            difffrac=0;
            //printf("forcing part of too large cropstand to give up fraction needed\n");
            foreachstand(stand,s,cell->standlist){
              if(stand->type->landusetype==AGRICULTURE){
                irrigation=stand->data;
                // determine PFT-ID of crop grown here (use last as there is only one crop per cropstand)
                foreachpft(pft,p,&stand->pftlist)
                  cft_id=pft->par->id-npft;
                if(irrigation->irrigation==TRUE && (stand->frac>cell->ml.landfrac[irrigation->irrigation].crop[cft_id]+epsilon))
                {
                  difffrac=min(stand->frac-tinyfrac,stand->frac-cell->ml.landfrac[irrigation->irrigation].crop[cft_id]);
                  pos=addstand(&agriculture_stand,cell);
                  cropstand=getstand(cell->standlist,pos-1);
                  data=cropstand->data;
                  cropstand->frac=difffrac;
                  stand->frac-=difffrac;
                  //printf("taking %g from %g of %s\n",difffrac,stand->frac+difffrac,pft->par->name);
                  data->irrigation=irrigation->irrigation;
                  reclaim_land(stand,cropstand,cell,istimber,npft+ncft);
                  set_irrigsystem(cropstand,cft,ncft,config->pft_output_scaled);
                  setaside(cell,cropstand,config->pftpar,cell->ml.with_tillage,config->intercrop,npft,FALSE,year,config->with_nitrogen);
                  setasidestand=getstand(cell->standlist,pos-1);
                  if(cell->ml.cropdates[cft].fallow_irrig<=0&&
                    check_lu(cell->standlist,cell->ml.landfrac[irrigation->irrigation].crop[cft],npft+cft,irrigation->irrigation))
                  {
                    if(!alloc_today_ir)
                    {
                      allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                      alloc_today_ir=TRUE;
                    }
                    stocks=cultivate(cell,config->pftpar+npft+cft,
                      cell->ml.cropdates[cft].vern_date20,
                      cell->ml.landfrac[irrigation->irrigation].crop[cft],irrigation->irrigation,day,FALSE,
                      setasidestand,cell->ml.with_tillage,istimber,config,
                      npft,ncft,cft,year);
                    flux_estab.carbon+=stocks.carbon;
                    flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
                    cell->output.sdate[cft+ncft]=day;
#endif
                    if(config->sdate_option==FIXED_SDATE)
                      cell->ml.sdate_fixed[cft+ncft]=day;
                  }//if check lu
                }//if too large stand->frac
              } // if AGRICULTURE
              if(difffrac>epsilon)
                break;
            } // foreachstand
          }/*of irrigated CFTs*/
        }
      } /*of no seasonality*/

      if(cell->ml.seasonality_type==PREC||cell->ml.seasonality_type==PRECTEMP) /*precipitation-dependent rules*/
      {
        s=findlandusetype(cell->standlist,SETASIDE_RF);

        if(s!=NOT_FOUND)
        {
          setasidestand=getstand(cell->standlist,s);
          if(month==cell->ml.sowing_month[cft]&&(dprec>MIN_PREC||dayofmonth==ndaymonth[month-1])) /*no irrigation, first wet day*/
          {
            if(cell->ml.cropdates[cft].fallow<=0&&
              check_lu(cell->standlist,cell->ml.landfrac[0].crop[cft],npft+cft,FALSE))
            {
              if(!alloc_today_rf)
              {
                allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                alloc_today_rf=TRUE;
              }
              stocks=cultivate(cell,config->pftpar+npft+cft,
                cell->ml.cropdates[cft].vern_date20,
                cell->ml.landfrac[0].crop[cft],FALSE,day,FALSE,
                setasidestand,cell->ml.with_tillage,istimber,config,
                npft,ncft,cft,year);
              flux_estab.carbon+=stocks.carbon;
              flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
              cell->output.sdate[cft]=day;
#endif
              if(config->sdate_option==FIXED_SDATE)
                cell->ml.sdate_fixed[cft]=day;
            }
          }
        }
        else
        {
          if(month==cell->ml.sowing_month[cft]&&(dprec>MIN_PREC||dayofmonth==ndaymonth[month-1])) /*no irrigation, first wet day*/
          {
            difffrac=0;
            foreachstand(stand,s,cell->standlist){
              if(stand->type->landusetype==AGRICULTURE){
                irrigation=stand->data;
                // determine PFT-ID of crop grown here (use last as there is only one crop per cropstand)
                foreachpft(pft,p,&stand->pftlist)
                  cft_id=pft->par->id-npft;
                if(irrigation->irrigation==FALSE && (stand->frac>cell->ml.landfrac[irrigation->irrigation].crop[cft_id]+epsilon))
                {
                  difffrac=min(stand->frac-tinyfrac,stand->frac-cell->ml.landfrac[irrigation->irrigation].crop[cft_id]);
                  pos=addstand(&agriculture_stand,cell);
                  cropstand=getstand(cell->standlist,pos-1);
                  data=cropstand->data;
                  cropstand->frac=difffrac;
                  stand->frac-=difffrac;
                  //printf("taking %g from %g of %s\n",difffrac,stand->frac+difffrac,pft->par->name);
                  data->irrigation=irrigation->irrigation;
                  reclaim_land(stand,cropstand,cell,istimber,npft+ncft);
                  set_irrigsystem(cropstand,cft,ncft,config->pft_output_scaled);
                  setaside(cell,cropstand,config->pftpar,cell->ml.with_tillage,config->intercrop,npft,irrigation->irrigation,year,config->with_nitrogen);
                  setasidestand=getstand(cell->standlist,pos-1);
                  if(cell->ml.cropdates[cft].fallow<=0&&
                    check_lu(cell->standlist,cell->ml.landfrac[irrigation->irrigation].crop[cft],npft+cft,irrigation->irrigation))
                  {
                    if(!alloc_today_rf)
                    {
                      allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                      alloc_today_rf=TRUE;
                    }
                    stocks=cultivate(cell,config->pftpar+npft+cft,
                      cell->ml.cropdates[cft].vern_date20,
                      cell->ml.landfrac[irrigation->irrigation].crop[cft],irrigation->irrigation,day,FALSE,
                      setasidestand,cell->ml.with_tillage,istimber,config,
                      npft,ncft,cft,year);
                    flux_estab.carbon+=stocks.carbon;
                    flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
                    cell->output.sdate[cft]=day;
#endif
                    if(config->sdate_option==FIXED_SDATE)
                      cell->ml.sdate_fixed[cft]=day;
                  }//if check lu
                }//if too large stand->frac
              } // if AGRICULTURE
              if(difffrac>epsilon)
                break;
            } // foreachstand
          }// if sowing but no setaside
        }/*of rainfed CFTs*/

        s=findlandusetype(cell->standlist,SETASIDE_IR);
        if(s!=NOT_FOUND)
        {
          setasidestand=getstand(cell->standlist,s);
          if(month==cell->ml.sowing_month[cft+ncft]&&(dprec>MIN_PREC||dayofmonth==ndaymonth[month-1])) /*irrigation, first wet day*/
          {
            if(cell->ml.cropdates[cft].fallow_irrig<=0&&
              check_lu(cell->standlist,cell->ml.landfrac[1].crop[cft],npft+cft,TRUE))
            {
              if(!alloc_today_ir)
              {
                allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                alloc_today_ir=TRUE;
              }
              stocks=cultivate(cell,config->pftpar+npft+cft,
                cell->ml.cropdates[cft].vern_date20,
                cell->ml.landfrac[1].crop[cft],TRUE,day,FALSE,
                setasidestand,cell->ml.with_tillage,istimber,config,
                npft,ncft,cft,year);
              flux_estab.carbon+=stocks.carbon;
              flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
              cell->output.sdate[cft+ncft]=day;
#endif
              if(config->sdate_option==FIXED_SDATE)
                cell->ml.sdate_fixed[cft+ncft]=day;
            }
          }
        }
        else
        {
          difffrac=0;
          if(month==cell->ml.sowing_month[cft+ncft]&&(dprec>MIN_PREC||dayofmonth==ndaymonth[month-1])) /*irrigation, first wet day*/
          {
            //printf("forcing part of too large cropstand to give up fraction needed\n");
            foreachstand(stand,s,cell->standlist){
              if(stand->type->landusetype==AGRICULTURE){
                irrigation=stand->data;
                // determine PFT-ID of crop grown here (use last as there is only one crop per cropstand)
                foreachpft(pft,p,&stand->pftlist)
                  cft_id=pft->par->id-npft;
                if(irrigation->irrigation==TRUE && (stand->frac>cell->ml.landfrac[irrigation->irrigation].crop[cft_id]+epsilon))
                {
                  difffrac=min(stand->frac-tinyfrac,stand->frac-cell->ml.landfrac[irrigation->irrigation].crop[cft_id]);
                  pos=addstand(&agriculture_stand,cell);
                  cropstand=getstand(cell->standlist,pos-1);
                  data=cropstand->data;
                  cropstand->frac=difffrac;
                  stand->frac-=difffrac;
                  //printf("taking %g from %g of %s\n",difffrac,stand->frac+difffrac,pft->par->name);
                  data->irrigation=irrigation->irrigation;
                  reclaim_land(stand,cropstand,cell,istimber,npft+ncft);
                  set_irrigsystem(cropstand,cft,ncft,config->pft_output_scaled);
                  setaside(cell,cropstand,config->pftpar,cell->ml.with_tillage,config->intercrop,npft,irrigation->irrigation,year,config->with_nitrogen);
                  setasidestand=getstand(cell->standlist,pos-1);
                  if(cell->ml.cropdates[cft].fallow_irrig<=0&&
                    check_lu(cell->standlist,cell->ml.landfrac[irrigation->irrigation].crop[cft],npft+cft,irrigation->irrigation))
                  {
                    if(!alloc_today_ir)
                    {
                      allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                      alloc_today_ir=TRUE;
                    }
                    stocks=cultivate(cell,config->pftpar+npft+cft,
                      cell->ml.cropdates[cft].vern_date20,
                      cell->ml.landfrac[irrigation->irrigation].crop[cft],irrigation->irrigation,day,FALSE,
                      setasidestand,cell->ml.with_tillage,istimber,config,
                      npft,ncft,cft,year);
                    flux_estab.carbon+=stocks.carbon;
                    flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
                    cell->output.sdate[cft+ncft]=day;
#endif
                    if(config->sdate_option==FIXED_SDATE)
                      cell->ml.sdate_fixed[cft+ncft]=day;
                  }//if check lu
                }//if too large stand->frac
              } // if AGRICULTURE
              if(difffrac>epsilon)
                break;
            } // foreachstand
          }// if sowing but no setaside
        }/*of irrigated CFTs*/
      } /*of precipitation seasonality*/

      if(cell->ml.seasonality_type==TEMP||cell->ml.seasonality_type==TEMPPREC) /*temperature-dependent rule*/
      {
        s=findlandusetype(cell->standlist,SETASIDE_RF);

        if(s!=NOT_FOUND)
        {
          setasidestand=getstand(cell->standlist,s);
          if(month==cell->ml.sowing_month[cft]) /*no irrigation*/
          {
            m=month-1; /*m runs from 0 to 11*/
            mm=(m-1<0) ? NMONTH-1 : m-1; /*mm is the month before*/
            if(cell->climbuf.mtemp20[mm]>cell->climbuf.mtemp20[m]&&croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE)
            {
              /*calculate day when temperature exceeds or falls below a crop-specific temperature threshold - from former function calc_cropdates*/
              if(((cell->climbuf.temp[NDAYS-1]<croppar->temp_fall)
                &&(cell->climbuf.temp[NDAYS-2]>=croppar->temp_fall||dayofmonth==1))||dayofmonth==ndaymonth[m]) /*sow winter variety*/
              {
                if(cell->ml.cropdates[cft].fallow<=0&&
                  check_lu(cell->standlist,cell->ml.landfrac[0].crop[cft],npft+cft,FALSE))
                {
                  if(!alloc_today_rf)
                  {
                    allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                    alloc_today_rf=TRUE;
                  }
                  stocks=cultivate(cell,config->pftpar+npft+cft,
                    cell->ml.cropdates[cft].vern_date20,
                    cell->ml.landfrac[0].crop[cft],FALSE,day,TRUE,
                    setasidestand,cell->ml.with_tillage,istimber,config,
                    npft,ncft,cft,year);
                  flux_estab.carbon+=stocks.carbon;
                  flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
                  cell->output.sdate[cft]=day;
#endif
                  if(config->sdate_option==FIXED_SDATE)
                    cell->ml.sdate_fixed[cft]=day;
                }
              }
            }
            else if(((cell->climbuf.temp[NDAYS-1]>croppar->temp_spring)
              &&(cell->climbuf.temp[NDAYS-2]<=croppar->temp_spring||dayofmonth==1))||dayofmonth==ndaymonth[m]) /*sow summer variety */
            {
              if(cell->ml.cropdates[cft].fallow<=0&&
                check_lu(cell->standlist,cell->ml.landfrac[0].crop[cft],npft+cft,FALSE))
              {
                if(!alloc_today_rf)
                {
                  allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                  alloc_today_rf=TRUE;
                }
                stocks=cultivate(cell,config->pftpar+npft+cft,
                  cell->ml.cropdates[cft].vern_date20,
                  cell->ml.landfrac[0].crop[cft],FALSE,day,FALSE,
                  setasidestand,cell->ml.with_tillage,istimber,config,
                  npft,ncft,cft,year);
                flux_estab.carbon+=stocks.carbon;
                flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
                cell->output.sdate[cft]=day;
#endif
                if(config->sdate_option==FIXED_SDATE)
                  cell->ml.sdate_fixed[cft]=day;
              }
            } /*of cultivating summer variety*/
          } /*of if month==ml.sowing_month[cft]*/
        }
        else
        {
          if(month==cell->ml.sowing_month[cft]) /*no irrigation*/
          {
            m=month-1; /*m runs from 0 to 11*/
            mm=(m-1<0) ? NMONTH-1 : m-1; /*mm is the month before*/
            if(cell->climbuf.mtemp20[mm]>cell->climbuf.mtemp20[m]&&croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE)
            {
              /*calculate day when temperature exceeds or falls below a crop-specific temperature threshold - from former function calc_cropdates*/
              if(((cell->climbuf.temp[NDAYS-1]<croppar->temp_fall)
                &&(cell->climbuf.temp[NDAYS-2]>=croppar->temp_fall||dayofmonth==1))||dayofmonth==ndaymonth[m]) /*sow winter variety*/
              {
                difffrac=0;
                foreachstand(stand,s,cell->standlist){
                  if(stand->type->landusetype==AGRICULTURE){
                    irrigation=stand->data;
                    // determine PFT-ID of crop grown here (use last as there is only one crop per cropstand)
                    foreachpft(pft,p,&stand->pftlist)
                      cft_id=pft->par->id-npft;
                    if(irrigation->irrigation==FALSE && (stand->frac>cell->ml.landfrac[irrigation->irrigation].crop[cft_id]+epsilon))
                    {
                      difffrac=min(stand->frac-tinyfrac,stand->frac-cell->ml.landfrac[irrigation->irrigation].crop[cft_id]);
                      pos=addstand(&agriculture_stand,cell);
                      cropstand=getstand(cell->standlist,pos-1);
                      data=cropstand->data;
                      cropstand->frac=difffrac;
                      stand->frac-=difffrac;
                      data->irrigation=irrigation->irrigation;
                      reclaim_land(stand,cropstand,cell,istimber,npft+ncft);
                      set_irrigsystem(cropstand,cft,ncft,config->pft_output_scaled);
                      setaside(cell,cropstand,config->pftpar,cell->ml.with_tillage,config->intercrop,npft,irrigation->irrigation,year,config->with_nitrogen);
                      setasidestand=getstand(cell->standlist,pos-1);
                      if(cell->ml.cropdates[cft].fallow<=0&&
                        check_lu(cell->standlist,cell->ml.landfrac[irrigation->irrigation].crop[cft],npft+cft,irrigation->irrigation))
                      {
                        if(!alloc_today_rf)
                        {
                          allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                          alloc_today_rf=TRUE;
                        }
                        stocks=cultivate(cell,config->pftpar+npft+cft,
                          cell->ml.cropdates[cft].vern_date20,
                          cell->ml.landfrac[irrigation->irrigation].crop[cft],irrigation->irrigation,day,TRUE,
                          setasidestand,cell->ml.with_tillage,istimber,config,
                          npft,ncft,cft,year);
                        flux_estab.carbon+=stocks.carbon;
                        flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
                        cell->output.sdate[cft]=day;
#endif
                      }//if check lu
                    }//if too large stand->frac
                  } // if AGRICULTURE
                  if(difffrac>epsilon)
                    break;
                } // foreachstand
              }
            }
            else if(((cell->climbuf.temp[NDAYS-1]>croppar->temp_spring)
              &&(cell->climbuf.temp[NDAYS-2]<=croppar->temp_spring||dayofmonth==1))||dayofmonth==ndaymonth[m]) /*sow summer variety */
            {
              difffrac=0;
              foreachstand(stand,s,cell->standlist){
                if(stand->type->landusetype==AGRICULTURE){
                  irrigation=stand->data;
                  // determine PFT-ID of crop grown here (use last as there is only one crop per cropstand)
                  foreachpft(pft,p,&stand->pftlist)
                    cft_id=pft->par->id-npft;
                  if(irrigation->irrigation==FALSE && (stand->frac>cell->ml.landfrac[irrigation->irrigation].crop[cft_id]+epsilon))
                  {
                    difffrac=min(stand->frac-tinyfrac,stand->frac-cell->ml.landfrac[irrigation->irrigation].crop[cft_id]);
                    pos=addstand(&agriculture_stand,cell);
                    cropstand=getstand(cell->standlist,pos-1);
                    data=cropstand->data;
                    cropstand->frac=difffrac;
                    stand->frac-=difffrac;
                    //printf("taking %g from %g of %s\n",difffrac,stand->frac+difffrac,pft->par->name);
                    data->irrigation=irrigation->irrigation;
                    reclaim_land(stand,cropstand,cell,istimber,npft+ncft);
                    set_irrigsystem(cropstand,cft,ncft,config->pft_output_scaled);
                    setaside(cell,cropstand,config->pftpar,cell->ml.with_tillage,config->intercrop,npft,irrigation->irrigation,year,config->with_nitrogen);
                    setasidestand=getstand(cell->standlist,pos-1);
                    if(cell->ml.cropdates[cft].fallow<=0&&
                      check_lu(cell->standlist,cell->ml.landfrac[irrigation->irrigation].crop[cft],npft+cft,irrigation->irrigation))
                    {
                      if(!alloc_today_rf)
                      {
                        allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                        alloc_today_rf=TRUE;
                      }
                      stocks=cultivate(cell,config->pftpar+npft+cft,
                        cell->ml.cropdates[cft].vern_date20,
                        cell->ml.landfrac[irrigation->irrigation].crop[cft],irrigation->irrigation,day,FALSE,
                        setasidestand,cell->ml.with_tillage,istimber,config,
                        npft,ncft,cft,year);
                      flux_estab.carbon+=stocks.carbon;
                      flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
                      cell->output.sdate[cft]=day;
#endif
                    }//if check lu
                  }//if too large stand->frac
                } // if AGRICULTURE
                if(difffrac>epsilon)
                  break;
              } // foreachstand
            }
          }

        }/*of rainfed CFTs*/
        s=findlandusetype(cell->standlist,SETASIDE_IR);

        if(s!=NOT_FOUND)
        {
          setasidestand=getstand(cell->standlist,s);

          if(month==cell->ml.sowing_month[cft+ncft]) /*irrigation*/
          {
            m=month-1; /*m runs from 0 to 11*/
            mm=(m-1<0) ? NMONTH-1 : m-1; /*mm is the month before*/
            if(cell->climbuf.mtemp20[mm]>cell->climbuf.mtemp20[m]&&croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE)
            {
              /*calculate day when temperature exceeds or falls below a crop-specific temperature threshold - from former function calc_cropdates*/
              if(((cell->climbuf.temp[NDAYS-1]<croppar->temp_fall)
                &&(cell->climbuf.temp[NDAYS-2]>=croppar->temp_fall||dayofmonth==1))||dayofmonth==ndaymonth[m]) /*sow winter variety*/
              {
                if(cell->ml.cropdates[cft].fallow_irrig<=0&&
                  check_lu(cell->standlist,cell->ml.landfrac[1].crop[cft],npft+cft,TRUE))
                {
                  if(!alloc_today_ir)
                  {
                    allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                    alloc_today_ir=TRUE;
                  }
                  stocks=cultivate(cell,config->pftpar+npft+cft,
                    cell->ml.cropdates[cft].vern_date20,
                    cell->ml.landfrac[1].crop[cft],TRUE,day,TRUE,
                    setasidestand,cell->ml.with_tillage,istimber,config,
                    npft,ncft,cft,year);
                  flux_estab.carbon+=stocks.carbon;
                  flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
                  cell->output.sdate[cft+ncft]=day;
#endif
                  if(config->sdate_option==FIXED_SDATE)
                    cell->ml.sdate_fixed[cft+ncft]=day;
                }
              }
            }
            else if(((cell->climbuf.temp[NDAYS-1]>croppar->temp_spring)
              &&(cell->climbuf.temp[NDAYS-2]<=croppar->temp_spring||dayofmonth==1))||dayofmonth==ndaymonth[m]) /*sow summer variety */
            {
              if(cell->ml.cropdates[cft].fallow_irrig<=0&&
                check_lu(cell->standlist,cell->ml.landfrac[1].crop[cft],npft+cft,TRUE))
              {
                if(!alloc_today_ir)
                {
                  allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                  alloc_today_ir=TRUE;
                }
                stocks=cultivate(cell,config->pftpar+npft+cft,
                  cell->ml.cropdates[cft].vern_date20,
                  cell->ml.landfrac[1].crop[cft],TRUE,day,FALSE,
                  setasidestand,cell->ml.with_tillage,istimber,config,
                  npft,ncft,cft,year);
                flux_estab.carbon+=stocks.carbon;
                flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
                cell->output.sdate[cft+ncft]=day;
#endif
                if(config->sdate_option==FIXED_SDATE)
                  cell->ml.sdate_fixed[cft+ncft]=day;
              }
            } /*of cultivating summer variety*/
          } /*of if month==ml.sowing_month[cft+ncft]*/
        }
        else
        {
          if(month==cell->ml.sowing_month[cft+ncft]) /*no irrigation*/
          {
            m=month-1; /*m runs from 0 to 11*/
            mm=(m-1<0) ? NMONTH-1 : m-1; /*mm is the month before*/
            if(cell->climbuf.mtemp20[mm]>cell->climbuf.mtemp20[m]&&croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE)
            {
              /*calculate day when temperature exceeds or falls below a crop-specific temperature threshold - from former function calc_cropdates*/
              if(((cell->climbuf.temp[NDAYS-1]<croppar->temp_fall)
                &&(cell->climbuf.temp[NDAYS-2]>=croppar->temp_fall||dayofmonth==1))||dayofmonth==ndaymonth[m]) /*sow winter variety*/
              {
                difffrac=0;
                foreachstand(stand,s,cell->standlist){
                  if(stand->type->landusetype==AGRICULTURE){
                    irrigation=stand->data;
                    // determine PFT-ID of crop grown here (use last as there is only one crop per cropstand)
                    foreachpft(pft,p,&stand->pftlist)
                      cft_id=pft->par->id-npft;
                    if(irrigation->irrigation==TRUE && (stand->frac>cell->ml.landfrac[irrigation->irrigation].crop[cft_id]+epsilon))
                    {
                      difffrac=min(stand->frac-tinyfrac,stand->frac-cell->ml.landfrac[irrigation->irrigation].crop[cft_id]);
                      pos=addstand(&agriculture_stand,cell);
                      cropstand=getstand(cell->standlist,pos-1);
                      data=cropstand->data;
                      cropstand->frac=difffrac;
                      stand->frac-=difffrac;
                      data->irrigation=irrigation->irrigation;
                      reclaim_land(stand,cropstand,cell,istimber,npft+ncft);
                      set_irrigsystem(cropstand,cft,ncft,config->pft_output_scaled);
                      setaside(cell,cropstand,config->pftpar,cell->ml.with_tillage,config->intercrop,npft,irrigation->irrigation,year,config->with_nitrogen);
                      setasidestand=getstand(cell->standlist,pos-1);
                      if(cell->ml.cropdates[cft].fallow_irrig<=0&&
                        check_lu(cell->standlist,cell->ml.landfrac[irrigation->irrigation].crop[cft],npft+cft,irrigation->irrigation))
                      {
                        if(!alloc_today_ir)
                        {
                          allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                          alloc_today_ir=TRUE;
                        }
                        stocks=cultivate(cell,config->pftpar+npft+cft,
                          cell->ml.cropdates[cft].vern_date20,
                          cell->ml.landfrac[irrigation->irrigation].crop[cft],irrigation->irrigation,day,TRUE,
                          setasidestand,cell->ml.with_tillage,istimber,config,
                          npft,ncft,cft,year);
                        flux_estab.carbon+=stocks.carbon;
                        flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
                        cell->output.sdate[cft+ncft]=day;
#endif
                      }//if check lu
                    }//if too large stand->frac
                  } // if AGRICULTURE
                  if(difffrac>epsilon)
                    break;
                } // foreachstand
              }
            }
            else if(((cell->climbuf.temp[NDAYS-1]>croppar->temp_spring)
              &&(cell->climbuf.temp[NDAYS-2]<=croppar->temp_spring||dayofmonth==1))||dayofmonth==ndaymonth[m]) /*sow summer variety */
            {
              difffrac=0;
              foreachstand(stand,s,cell->standlist){
                if(stand->type->landusetype==AGRICULTURE){
                  irrigation=stand->data;
                  // determine PFT-ID of crop grown here (use last as there is only one crop per cropstand)
                  foreachpft(pft,p,&stand->pftlist)
                    cft_id=pft->par->id-npft;
                  if(irrigation->irrigation==TRUE && (stand->frac>cell->ml.landfrac[irrigation->irrigation].crop[cft_id]+epsilon))
                  {
                    difffrac=min(stand->frac-tinyfrac,stand->frac-cell->ml.landfrac[irrigation->irrigation].crop[cft_id]);
                    pos=addstand(&agriculture_stand,cell);
                    cropstand=getstand(cell->standlist,pos-1);
                    data=cropstand->data;
                    cropstand->frac=difffrac;
                    stand->frac-=difffrac;
                    //printf("taking %g from %g of %s\n",difffrac,stand->frac+difffrac,pft->par->name);
                    data->irrigation=irrigation->irrigation;
                    reclaim_land(stand,cropstand,cell,istimber,npft+ncft);
                    set_irrigsystem(cropstand,cft,ncft,config->pft_output_scaled);
                    setaside(cell,cropstand,config->pftpar,cell->ml.with_tillage,config->intercrop,npft,irrigation->irrigation,year,config->with_nitrogen);
                    setasidestand=getstand(cell->standlist,pos-1);
                    if(cell->ml.cropdates[cft].fallow_irrig<=0&&
                      check_lu(cell->standlist,cell->ml.landfrac[irrigation->irrigation].crop[cft],npft+cft,irrigation->irrigation))
                    {
                      if(!alloc_today_ir)
                      {
                        allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                        alloc_today_ir=TRUE;
                      }
                      stocks=cultivate(cell,config->pftpar+npft+cft,
                        cell->ml.cropdates[cft].vern_date20,
                        cell->ml.landfrac[irrigation->irrigation].crop[cft],irrigation->irrigation,day,FALSE,
                        setasidestand,cell->ml.with_tillage,istimber,config,
                        npft,ncft,cft,year);
                      flux_estab.carbon+=stocks.carbon;
                      flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
                      cell->output.sdate[cft+ncft]=day;
#endif
                    }//if check lu
                  }//if too large stand->frac
                } // if AGRICULTURE
                if(difffrac>epsilon)
                  break;
              } // foreachstand
            }
          }

        }/*of rainfed CFTs*/
      }  /*for(cft=...) */
    }
  }
  return flux_estab;
} /* of 'sowing_season' */

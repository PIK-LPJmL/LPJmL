/**************************************************************************************/
/**                                                                                \n**/
/**            s  o  w  i  n  g  _  p  r  e  s  c  r  i  b  e  .  c                \n**/
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

Stocks sowing_prescribe(Cell* cell,          /**< pointer to cell */
    int day,             /**< day of year (1..365) */
    int npft,            /**< number of natural PFTs */
    int ncft,            /**< number of crop PFTs */
    int year,            /**< simulation year (AD) */
    const Config* config /**< LPJmL configuration */
)                     /** \return establishment flux (gC/m2) */
{
    Stocks flux_estab = { 0,0 }, stocks;
    Stand *setasidestand;
    Bool alloc_today_rf = FALSE, alloc_today_ir = FALSE, istimber;
    const Pftcroppar* croppar;
    int cft, s, s2;
    int earliest_sdate;
    Bool wtype = FALSE;
    Pft *pft;
    int p,pos,cft_id;
    Real difffrac=0;
    Stand *stand, *cropstand;
    Irrigation *irrigation, *data;

#ifdef IMAGE
    istimber = (config->start_imagecoupling != INT_MAX);
#else
    istimber = FALSE;
#endif
    s = findlandusetype(cell->standlist, SETASIDE_RF);
    s2 = findlandusetype(cell->standlist, SETASIDE_IR);
    //if (day == cell->ml.sdate_fixed[1]) {
    //    printf("sowing_prescribe(L42): day==%d, s==%d, s2==%d\n", day, s, s2);
    //}
  if (s!=NOT_FOUND||s2!=NOT_FOUND)
  {
    for (cft=0; cft<ncft; cft++)
    {
      //if(day==1)  printf("sowing_prescribe(L49): cft %d, sowingday=%d\n", cft,cell->ml.sdate_fixed[cft]);
      croppar=config->pftpar[npft+cft].data;
      earliest_sdate=(cell->coord.lat>=0)?croppar->initdate.sdatenh:croppar->initdate.sdatesh;

      /*rainfed crops*/
      s=findlandusetype(cell->standlist, SETASIDE_RF);

      if(s!=NOT_FOUND)
      {
        setasidestand=getstand(cell->standlist,s);

        if(day==cell->ml.sdate_fixed[cft])
        {
          //printf("sowing_prescribe(L61): for cft=%d, lon=%.2f, lat=%.2f, sdate=%d, earliest_sdate=%d frac %g\n",cft,cell->coord.lon,cell->coord.lat,day,earliest_sdate,cell->ml.landfrac[0].crop[cft]);
          wtype=(croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE&&day>earliest_sdate) ? TRUE : FALSE;
          if(check_lu(cell->standlist,cell->ml.landfrac[0].crop[cft],npft+cft,FALSE))
          {
            if(!alloc_today_rf)
            {
              allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
              alloc_today_rf=TRUE;
            }
            stocks=cultivate(cell,config->pftpar+npft+cft,
              cell->ml.cropdates[cft].vern_date20,
              cell->ml.landfrac[0].crop[cft],FALSE,day,wtype,
              setasidestand,cell->ml.with_tillage,istimber,config,
              npft,ncft,cft,year);
            flux_estab.carbon+=stocks.carbon;
            flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
            cell->output.sdate[cft]=day;
#endif
          }
        }/*of rainfed CFTs*/
      }
      else
      {
        /* need to add similar thing for irrigated case */
        if(day==cell->ml.sdate_fixed[cft]){
          /*printf("sowing_prescribe else: crop %d landfrac %g\n",cft,cell->ml.landfrac[0].crop[cft]);
          foreachstand(stand,s,cell->standlist){
            printf("stand %d type %d frac %g\n",s,stand->type->landusetype,stand->frac);
            if(stand->type->landusetype==AGRICULTURE){
              irrigation=stand->data;
              foreachpft(pft,p,&stand->pftlist)
                printf("stand %d type %d irrig %d lufrac %g %g %s\n",s,stand->type->landusetype,irrigation->irrigation,cell->ml.landfrac[irrigation->irrigation].crop[pft->par->id-npft],stand->frac,pft->par->name);
            }
          }*/
          //printf("forcing part of too large cropstand to give up fraction needed\n");
          difffrac=0;
          foreachstand(stand,s,cell->standlist){
            if(stand->type->landusetype==AGRICULTURE){
              irrigation=stand->data;
              // determine PFT-ID of crop grown here (use last as there is only one crop per cropstand)
              foreachpft(pft,p,&stand->pftlist)
                cft_id=pft->par->id-npft;
              if(irrigation->irrigation==FALSE && stand->frac>cell->ml.landfrac[irrigation->irrigation].crop[cft_id])
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
                set_irrigsystem(cropstand,cft,0,config->pft_output_scaled);
                setaside(cell,cropstand,config->pftpar,cell->ml.with_tillage,config->intercrop,npft,FALSE,year,config->with_nitrogen);
                setasidestand=getstand(cell->standlist,pos-1);
                wtype=(croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE&&day>earliest_sdate) ? TRUE : FALSE;
                if(check_lu(cell->standlist,cell->ml.landfrac[0].crop[cft],npft+cft,FALSE))
                {
                  if(!alloc_today_rf)
                  {
                    allocation_today(setasidestand,config->ntypes,config->with_nitrogen);
                    alloc_today_rf=TRUE;
                  }
                  stocks=cultivate(cell,config->pftpar+npft+cft,
                    cell->ml.cropdates[cft].vern_date20,
                    cell->ml.landfrac[0].crop[cft],FALSE,day,wtype,
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
        }// if sowing but no setaside
      }// else
      s=findlandusetype(cell->standlist, SETASIDE_IR);
      if (s!=NOT_FOUND)
      {
        setasidestand=getstand(cell->standlist, s);


        /*irrigated crops*/
        if (day==cell->ml.sdate_fixed[cft+ncft])
        {
          wtype=(croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE&&day>earliest_sdate)?TRUE:FALSE;
          if (check_lu(cell->standlist, cell->ml.landfrac[1].crop[cft], npft+cft, TRUE))
          {
            if (!alloc_today_ir)
            {
              allocation_today(setasidestand, config->ntypes, config->with_nitrogen);
              alloc_today_ir=TRUE;
            }
            stocks=cultivate(cell, config->pftpar+npft+cft,
              cell->ml.cropdates[cft].vern_date20,
              cell->ml.landfrac[1].crop[cft], TRUE, day, wtype,
              setasidestand, cell->ml.with_tillage, istimber, config,
              npft, ncft, cft, year);
            flux_estab.carbon+=stocks.carbon;
            flux_estab.nitrogen+=stocks.nitrogen;
#ifndef DOUBLE_HARVEST
            cell->output.sdate[cft+ncft]=day;
#endif
          }
        }/*of irrigated CFTs*/
      }
      else
      {
        difffrac=0;
        if (day==cell->ml.sdate_fixed[cft+ncft]){
          //printf("forcing part of too large cropstand to give up fraction needed\n");
          foreachstand(stand,s,cell->standlist){
            if(stand->type->landusetype==AGRICULTURE){
              irrigation=stand->data;
              // determine PFT-ID of crop grown here (use last as there is only one crop per cropstand)
              foreachpft(pft,p,&stand->pftlist)
                cft_id=pft->par->id-npft;
              if(irrigation->irrigation == TRUE && stand->frac > cell->ml.landfrac[irrigation->irrigation].crop[cft_id])
              {
                difffrac=min(stand->frac-tinyfrac,stand->frac-cell->ml.landfrac[irrigation->irrigation].crop[cft_id]);
                pos=addstand(&agriculture_stand,cell);
                cropstand=getstand(cell->standlist,pos-1);
                data=cropstand->data;
                cropstand->frac=difffrac;
                stand->frac-=difffrac;
                //printf("taking %g from %g of %s\n",difffrac,stand->frac+difffrac,pft->par->name);
                data->irrigation= irrigation->irrigation;
                reclaim_land(stand,cropstand,cell,istimber,npft+ncft);
                set_irrigsystem(cropstand,cft,ncft,config->pft_output_scaled);
                setaside(cell,cropstand,config->pftpar,cell->ml.with_tillage,config->intercrop,npft,irrigation->irrigation,year,config->with_nitrogen);
                setasidestand=getstand(cell->standlist, pos-1);
                wtype=(croppar->calcmethod_sdate==TEMP_WTYP_CALC_SDATE&&day>earliest_sdate)?TRUE:FALSE;
                if (check_lu(cell->standlist, cell->ml.landfrac[irrigation->irrigation].crop[cft], npft + cft, irrigation->irrigation))
                {
                  if (!alloc_today_ir)
                  {
                    allocation_today(setasidestand, config->ntypes, config->with_nitrogen);
                    alloc_today_ir=TRUE;
                  }
                  stocks=cultivate(cell, config->pftpar+npft+cft,
                    cell->ml.cropdates[cft].vern_date20,
                    cell->ml.landfrac[irrigation->irrigation].crop[cft], irrigation->irrigation, day, wtype,
                    setasidestand, cell->ml.with_tillage, istimber, config,
                    npft, ncft, cft, year);
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
        }// if sowing but no setaside
      }// else
    }
  }
  return flux_estab;
} /* of 'sowing_prescribe' */

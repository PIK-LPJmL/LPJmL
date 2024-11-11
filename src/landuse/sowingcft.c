/**************************************************************************************/
/**                                                                                \n**/
/**                  s  o  w  i  n  g  c  f  t  .  c                               \n**/
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
#include "grassland.h"
#include "agriculture.h"
#include "crop.h"
#include "grass.h"

static void cultcftstand(Stocks *flux_estab,  /**< establishment flux */
                         Bool *alloc_today,   /**< first allocation? (TRUE/FALSE) */
                         Cell *cell,          /**< pointer to cell */
                         Stand *setasidestand,/**< pointer to setaside stand */
                         Bool irrig,          /**< stand irrigated? (TRUE/FALSE) */
                         Bool wtype,          /**< winter type (TRUE/FALSE) */
                         Bool nofallow,       /**< fallow will not be checked */
                         int npft,            /**< number of natural PFTs  */
                         int ncft,            /**< number of crop PFTs */
                         int cft,             /**< index of CFT to establish */
                         int year,            /**< simulation year (AD) */
                         int day,             /**< day (1..365) */
                         Bool isother,        /**< other stand (TRUE/FALSE) */
                         const Config *config /**< LPJmL settings */
                        )
{
  Pft *pft;
  int p;
  Stocks stocks;
#ifdef CHECK_BALANCE
  Stand *stand;
  Stocks flux_in={0,0};
  Stocks start={0,0};
  int s;
  flux_in.carbon=cell->balance.anpp+cell->balance.flux_estab.carbon+cell->balance.influx.carbon+flux_estab->carbon;
  flux_in.nitrogen=cell->balance.flux_estab.nitrogen+cell->balance.influx.nitrogen+flux_estab->nitrogen;
  foreachstand(stand,s,cell->standlist)
  {
    start.carbon+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
    start.nitrogen+=standstocks(stand).nitrogen*stand->frac;
  }
  Real end=0;
#endif

  if((nofallow || cell->ml.cropdates[cft].fallow[irrig]<=0)  &&
      check_lu(cell->standlist,(isother) ? cell->ml.landfrac[irrig].grass[0] : cell->ml.landfrac[irrig].crop[cft],npft+cft,(isother) ? OTHERS : AGRICULTURE,irrig))
  {
//    if(year==2010 && day==59) fprintf(stdout,"cultcftstand2 on day: %d allocation: %d type: %s standfrac: %g\n",day,*alloc_today,setasidestand->type->name,setasidestand->frac);
    if(!(*alloc_today) && setasidestand->type->landusetype!=AGRICULTURE)
    {
      foreachpft(pft, p, &setasidestand->pftlist)
        turnover_grass(&setasidestand->soil.litter, pft,1.0/NDAYYEAR,config);
      allocation_today(setasidestand,config);
      *alloc_today=TRUE;
    }
    stocks=cultivate(cell,irrig,day,wtype,setasidestand,
                     npft,ncft,cft,year,isother,config);
    flux_estab->carbon+=stocks.carbon;
    flux_estab->nitrogen+=stocks.nitrogen;
    if(irrig)
      cft+=ncft;
    if(!config->separate_harvests)
      getoutputindex(&cell->output,SDATE,cft,config)=day;
    if(config->sdate_option==FIXED_SDATE)
      cell->ml.sdate_fixed[cft]=day;
  }//if check lu
#ifdef CHECK_BALANCE
  foreachstand(stand,s,cell->standlist)
    end+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
  end=0;
  flux_in.carbon=(cell->balance.anpp+cell->balance.flux_estab.carbon+cell->balance.influx.carbon+flux_estab->carbon)-flux_in.carbon;
  flux_in.nitrogen=(cell->balance.flux_estab.nitrogen+cell->balance.influx.nitrogen+flux_estab->nitrogen)-flux_in.nitrogen;

  foreachstand(stand,s,cell->standlist)
    end+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;

  if (fabs(end-start.carbon-flux_in.carbon)>0.01)
  {
    fprintf(stderr, "C_ERROR-cultcftstand: day: %d  CFT: %d  %g start: %.3f  end: %.3f  "
        "flux_estab.carbon: %g balance.flux_estab: %g flux_in.carbon: %g \n",
        day,cft,end-start.carbon-flux_in.carbon,start.carbon, end,flux_estab->carbon,cell->balance.flux_estab.carbon,
        flux_in.carbon);
   }
  end=0;
  foreachstand(stand,s,cell->standlist)
    end+=standstocks(stand).nitrogen*stand->frac;

  if (fabs(end-start.nitrogen-flux_in.nitrogen)>0.01)
  {
    fprintf(stderr, "N_ERROR-cultcftstand: day: %d  CFT: %d  %g start: %.3f  end: %.3f  "
        "flux_estab.nitrogen: %g balance.flux_estab: %g flux_in.nitrogen: %g \n",
        day,cft,end-start.nitrogen-flux_in.nitrogen,start.nitrogen, end,flux_estab->nitrogen,cell->balance.flux_estab.nitrogen,
        flux_in.nitrogen);
   }

#endif

} /* of 'cultcftstand' */

void sowingcft(Stocks *flux_estab,  /**< establishment flux */
               Bool *alloc_today,   /**< first allocation? (TRUE/FALSE) */
               Cell *cell,          /**< pointer to cell */
               Bool irrig,          /**< stand is irrigated? */
               Bool wtype,          /**< winter type (TRUE/FALSE) */
               Bool nofallow,       /**< fallow will not be checked */
               int npft,            /**< number of natural PFTs  */
               int ncft,            /**< number of crop PFTs */
               int cft,             /**< index of CFT to establish */
               int year,            /**< simulation year (AD) */
               int day,             /**< day (1..365) */
               Bool isother,        /**< other stand(TRUE/FALSE) */
               const Config *config /**< LPJmL configuration */
              )
{
  Stand *stand,*cropstand,*setasidestand;
  Pft *pft;
  Real difffrac=0;
  Real landfrac;
  int s,p,cft_id=0,pos;
  Irrigation *irrigation,*data;
#ifdef CHECK_BALANCE
  Stocks flux_in={0,0};
  Stocks start={0,0};
  flux_in.carbon=cell->balance.anpp+cell->balance.flux_estab.carbon+cell->balance.influx.carbon+flux_estab->carbon;
  flux_in.nitrogen=cell->balance.flux_estab.nitrogen+cell->balance.influx.nitrogen+flux_estab->nitrogen;
  foreachstand(stand,s,cell->standlist)
  {
    start.carbon+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
    start.nitrogen+=standstocks(stand).nitrogen*stand->frac;
  }
#endif
  s=NOT_FOUND;

  /* set sowing date for all CFTs not in the land-use data set */
  if(config->sdate_option==FIXED_SDATE)
    cell->ml.sdate_fixed[cft+irrig*ncft]=day;

  if(cft==RICE)
    s=findlandusetype(cell->standlist,SETASIDE_WETLAND);
  if(s==NOT_FOUND)
    s=findlandusetype(cell->standlist,(irrig) ? SETASIDE_IR : SETASIDE_RF);
  if(s==NOT_FOUND)
    s=findlandusetype(cell->standlist,(irrig) ? SETASIDE_RF : SETASIDE_IR);
  if(s==NOT_FOUND)
    s=findlandusetype(cell->standlist,SETASIDE_WETLAND);

//  if(s!=NOT_FOUND)
//    {
//      setasidestand=getstand(cell->standlist,s);
//      if(setasidestand->frac<(param.tinyfrac)) s=NOT_FOUND;
//    }
//
  if(s!=NOT_FOUND)
  {
    setasidestand=getstand(cell->standlist,s);
    foreachpft(pft,p,&setasidestand->pftlist)
     if(pft->bm_inc.carbon>0) *alloc_today=FALSE;
     cultcftstand(flux_estab,alloc_today,cell,setasidestand,irrig,wtype,nofallow,npft,ncft,cft,year,day,isother,config);
  }
  else
  {
    difffrac=0;
    foreachstand(stand,s,cell->standlist)
    {
      if(stand->type->landusetype==((isother) ? OTHERS : AGRICULTURE))
      {
        irrigation=stand->data;
        // determine PFT-ID of crop grown here (use last or first as there is only one crop per cropstand)
        foreachpft(pft,p,&stand->pftlist)
          cft_id=pft->par->id-npft;
        landfrac=(isother) ? cell->ml.landfrac[irrig].grass[0] : cell->ml.landfrac[irrig].crop[cft_id];
        if(irrigation->irrigation==irrig && stand->frac > (2*param.tinyfrac+epsilon) && stand->frac > landfrac+epsilon && cell->ml.landfrac[irrig].crop[cft]>0)
        {
          difffrac=min(stand->frac-param.tinyfrac,stand->frac-landfrac);
          pos=addstand((isother) ? &others_stand : &agriculture_stand,cell);
          cropstand=getstand(cell->standlist,pos-1);
          data=cropstand->data;
          cropstand->frac=difffrac;
//          if(year==1880) printf("\n\n taking %g from %g of %s on irrig: %d landfrac: %g standname: %s isother: %d landfrac_others: %g landfrac_cfts: %.8f landfrac_cft_id: %g cft: %d cft_id: %d\n",
//              difffrac,stand->frac,pft->par->name,irrig,landfrac,stand->type->name,isother,
//              cell->ml.landfrac[irrig].grass[0],cell->ml.landfrac[irrig].crop[cft],cell->ml.landfrac[irrig].crop[cft_id],cft,cft_id);
          data->irrigation=irrig;
          stand->frac-=difffrac;
          reclaim_land(stand,cropstand,cell,config->luc_timber,npft+ncft,config);
          set_irrigsystem(cropstand,cft,npft,ncft,config);
          setaside(cell,cropstand,cell->ml.with_tillage,config->intercrop,npft,ncft,irrig,cropstand->soil.iswetland,year,config);
          cultcftstand(flux_estab,alloc_today,cell,cropstand,irrig,wtype,nofallow,npft,ncft,cft,year,day,isother,config);
        }//if too large stand->frac
      } // if AGRICULTURE
      if(difffrac>epsilon)
        break;
    } // foreachstand
  }

#ifdef CHECK_BALANCE
  Real end=0;
  flux_in.carbon=(cell->balance.anpp+cell->balance.flux_estab.carbon+cell->balance.influx.carbon+flux_estab->carbon)-flux_in.carbon;
  flux_in.nitrogen=(cell->balance.flux_estab.nitrogen+cell->balance.influx.nitrogen+flux_estab->nitrogen)-flux_in.nitrogen;

  foreachstand(stand,s,cell->standlist)
    end+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;

  if (fabs(end-start.carbon-flux_in.carbon)>0.01)
  {
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid carbon balance in %s: day: %d  CFT: %d  %g start: %.3f  end: %.3f  "
        "flux_estab.carbon: %g balance.flux_estab: %g flux_in.carbon: %g "
        "diffrac: %g",
        __FUNCTION__,day,cft,end-start.carbon-flux_in.carbon,start.carbon, end,flux_estab->carbon,cell->balance.flux_estab.carbon,
        flux_in.carbon,difffrac);
   }
  end=0;
  foreachstand(stand,s,cell->standlist)
    end+=standstocks(stand).nitrogen*stand->frac;

  if (fabs(end-start.nitrogen-flux_in.nitrogen)>0.01)
  {
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid nitrogen balance in %s: day: %d  CFT: %d  %g start: %.3f  end: %.3f  "
        "flux_estab.nitrogen: %g balance.flux_estab: %g flux_in.nitrogen: %g "
        "diffrac: %g",
        __FUNCTION__,day,cft,end-start.nitrogen-flux_in.nitrogen,start.nitrogen, end,flux_estab->nitrogen,cell->balance.flux_estab.nitrogen,
        flux_in.nitrogen,difffrac);
   }
#endif
} /* of 'sowingcft' */

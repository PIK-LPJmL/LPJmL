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
#include "agriculture.h"
#include "crop.h"

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
                         const Config *config /**< LPJmL settings */
                        )
{
  Stocks stocks;
  if((nofallow || cell->ml.cropdates[cft].fallow[irrig]<=0) &&
     check_lu(cell->standlist,cell->ml.landfrac[irrig].crop[cft],npft+cft,irrig))
  {
    if(!(*alloc_today))
    {
      allocation_today(setasidestand,config);
      *alloc_today=TRUE;
    }
    stocks=cultivate(cell,irrig,day,wtype,setasidestand,
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
               const Config *config /**< LPJmL configuration */
              )
{
  Stand *stand,*cropstand,*setasidestand;
  Pft *pft;
  Real difffrac;
  int s,p,cft_id,pos;
  Irrigation *irrigation,*data;
  /* set sowing date for all CFTs not in the land-use data set */
  if(config->sdate_option==FIXED_SDATE && cell->ml.landfrac[irrig].crop[cft]==0)
    cell->ml.sdate_fixed[cft+irrig*ncft]=day;
  s=findlandusetype(cell->standlist,(irrig) ? SETASIDE_IR : SETASIDE_RF);

  if(s!=NOT_FOUND)
  {
    setasidestand=getstand(cell->standlist,s);
    cultcftstand(flux_estab,alloc_today,cell,setasidestand,irrig,wtype,nofallow,npft,ncft,cft,year,day,config);
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
          cultcftstand(flux_estab,alloc_today,cell,setasidestand,irrig,wtype,nofallow,npft,ncft,cft,year,day,config);
        }//if too large stand->frac
      } // if AGRICULTURE
      if(difffrac>epsilon)
        break;
    } // foreachstand
  }
} /* of 'sowingcft' */

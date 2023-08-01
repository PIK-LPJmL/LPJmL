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
  Bool CULT =FALSE;
  if((nofallow || cell->ml.cropdates[cft].fallow[irrig]<=0)  &&
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
  Pft *pft,*croppft;
  Real difffrac=0;
  int s,p,cft_id,pos,st,l;
  Irrigation *irrigation,*data;
#ifdef CHECK_BALANCE
  Real flux_carbon=cell->balance.flux_estab.carbon+flux_estab->carbon;
  Real balance_carbon=cell->balance.flux_estab.carbon;
  Real sflux_estab=flux_estab->carbon;
  Real start=0;
  Real manure=0;   //TODO is applied in cultive.c and needs to be taking into account
  foreachstand(stand,s,cell->standlist)
  {
    start+=(standstocks(stand).carbon + soilmethane(&stand->soil))*stand->frac;
  }
#endif
  s=NOT_FOUND;

  /* set sowing date for all CFTs not in the land-use data set */
  if(config->sdate_option==FIXED_SDATE)
    cell->ml.sdate_fixed[cft+irrig*ncft]=day;
  if(cell->ml.landfrac[irrig].crop[cft]==0)
    return;

  if(cft==RICE)
    s=findlandusetype(cell->standlist,SETASIDE_WETLAND);
  if(s==NOT_FOUND)
    s=findlandusetype(cell->standlist,(irrig) ? SETASIDE_IR : SETASIDE_RF);
  if(s==NOT_FOUND)
    s=findlandusetype(cell->standlist,SETASIDE_WETLAND);
  if(s!=NOT_FOUND)
  {
    setasidestand=getstand(cell->standlist,s);
    foreachpft(pft,p,&setasidestand->pftlist)
     if(pft->bm_inc.carbon>0) *alloc_today=FALSE;
    if(cft!=RICE)
      setasidestand->soil.iswetland=FALSE;
    cultcftstand(flux_estab,alloc_today,cell,setasidestand,irrig,wtype,nofallow,npft,ncft,cft,year,day,config);
  }

#ifdef CHECK_BALANCE
  Real end=0;
  flux_carbon=cell->balance.flux_estab.carbon+flux_estab->carbon-flux_carbon;
  foreachstand(stand,s,cell->standlist)
  {
    end+=(standstocks(stand).carbon + soilmethane(&stand->soil))*stand->frac;
  }
  if (fabs(end-start-flux_carbon)>0.01)
         fprintf(stderr, "C_ERROR-SOWINGCFT: day: %d    %g start: %g  end: %g  sflux_estab.carbon: %g sbalance.flux_estab: %g flux_estab.carbon: %g balance.flux_estab: %g flux_carbon: %g \n",
           day,end-start-flux_carbon,start, end,sflux_estab,balance_carbon,flux_estab->carbon,cell->balance.flux_estab.carbon, flux_carbon);

#endif
} /* of 'sowingcft' */

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
  Real manure=0;
  foreachstand(stand,s,cell->standlist)
  {
    start+=(standstocks(stand).carbon + soilmethane(&stand->soil))*stand->frac;
  }
#endif
  s=NOT_FOUND;

  /* set sowing date for all CFTs not in the land-use data set */
  if(config->sdate_option==FIXED_SDATE)
    cell->ml.sdate_fixed[cft+irrig*ncft]=day;
  //fprintf(stderr,"Start sowingcft year: %d day: %d  datafrac: %g irrig: %d cft: %d\n", year,day,cell->ml.landfrac[irrig].crop[cft],irrig,cft);
  if(cell->ml.landfrac[irrig].crop[cft]==0)
    return;

  if(cft==RICE)
    s=findlandusetype(cell->standlist,SETASIDE_WETLAND);
  if(s==NOT_FOUND)
    s=findlandusetype(cell->standlist,(irrig) ? SETASIDE_IR : SETASIDE_RF);
  if(s==NOT_FOUND)
    s=findlandusetype(cell->standlist,SETASIDE_WETLAND);
//  if(year>=1511)
//  {
//    fprintf(stderr,"sowingcft: s: %d cft: %d\n ",s,cft);
//    foreachstand(stand,st,cell->standlist)
//        foreachpft(pft,p,&stand->pftlist)
//         fprintf(stderr,"sowingcft year: %d day: %d frac[%s]= %g standNR: %d PFT: %s bm_in= %g landusetype: %d\n",
//             year,day,stand->type->name,stand->frac,st,pft->par->name,pft->bm_inc,stand->type->landusetype);
//  }
  if(s!=NOT_FOUND)
  {
    setasidestand=getstand(cell->standlist,s);
    foreachpft(pft,p,&setasidestand->pftlist)
     if(pft->bm_inc.carbon>0) *alloc_today=FALSE;
    if(cft!=RICE)
      setasidestand->soil.iswetland=FALSE;
      //if(year==1546) fprintf(stderr,"1:setaside: %s landusetype: %d standfrac: %g s: %d day: %d CFT: %d flux_estab.carbon: %g frac: %g\n",
      //setasidestand->type->name,setasidestand->type->landusetype,setasidestand->frac,s,day,cft,flux_estab->carbon,cell->ml.landfrac[0].crop[cft]);
      cultcftstand(flux_estab,alloc_today,cell,setasidestand,irrig,wtype,nofallow,npft,ncft,cft,year,day,config);
      //if(year==1546) fprintf(stderr,"2:setaside: %s landusetype: %d standfrac: %g s: %d day: %d CFT: %d flux_estab.carbon: %g frac: %g\n\n",
      //setasidestand->type->name,setasidestand->type->landusetype,setasidestand->frac,s,day,cft,flux_estab->carbon,cell->ml.landfrac[0].crop[cft]);
  }
//  else
//  {
////    if(year==1512)
////      foreachstand(stand,s,cell->standlist)
////         foreachpft(pft,p,&stand->pftlist)
////            fprintf(stderr,"sowingcft2 year: %d frac[%s]= %g standNR: %d PFT: %s \n",year,stand->type->name,stand->frac,s,pft->par->name);
//
//    foreachstand(stand,s,cell->standlist)
//    {
//      if(stand->type->landusetype==AGRICULTURE)
//      {
//        irrigation=stand->data;
//        // determine PFT-ID of crop grown here (use last as there is only one crop per cropstand)
//        foreachpft(pft,p,&stand->pftlist)
//          cft_id=pft->par->id-npft;
//        if(cell->ml.landfrac[irrig].crop[cft]>0 && irrigation->irrigation==irrig && stand->frac > (2*tinyfrac+epsilon) && stand->frac > (cell->ml.landfrac[irrig].crop[cft_id]+0.0001))
//        {
//          difffrac=min(stand->frac-tinyfrac,stand->frac-cell->ml.landfrac[irrig].crop[cft_id]-tinyfrac);
//          pos=addstand(&agriculture_stand,cell)-1;
//          cropstand=getstand(cell->standlist,pos);
//          data=cropstand->data;
//          cropstand->frac=difffrac;
//          stand->frac-=difffrac;
////          if(year==1512)
////            fprintf(stderr,"year: %d taking %g from %g of %s bm_inc %g pos: %d allocation: %d pftid: %d cft: %d datafrac: %g difffrac: %g cropfrac: %g\n",
////                year,difffrac,stand->frac,pft->par->name,pft->bm_inc.carbon,pos,*alloc_today,npft+cft,cft,cell->ml.landfrac[irrig].crop[cft_id],difffrac,cropstand->frac);
//          data->irrigation=irrig;
//          cropstand->type = &agriculture_stand;
//          cropstand->fire_sum=stand->fire_sum;
//          cropstand->slope_mean=stand->slope_mean;
//          cropstand->Hag_Beta=stand->Hag_Beta;
//          stand->soil.iswetland = FALSE;
//          for (l = 0; l<LASTLAYER; l++)
//          {
//            cropstand->soil.c_shift[l] = newvec(Poolpar, npft+cft);
//            check(cropstand->soil.c_shift[l]);
//          }
//          copysoil(&cropstand->soil, &stand->soil,npft+cft);
//          for (l = 0; l<NSOILLAYER; l++)
//            cropstand->frac_g[l] = stand->frac_g[l];
//          foreachpft(pft, p, &stand->pftlist)
//          {
//            croppft = addpft(cropstand,pft->par,year,365,config);
//            copypft(croppft,pft);
//          }
////          if(year==1512)
////           {
////             fprintf(stderr,"CHECKLU: irrig: %d  cft: %d landfrac: %g \n",irrig,cft,cell->standlist,cell->ml.landfrac[irrig].crop[cft]);
////             foreachpft(pft,p,&cropstand->pftlist)
////                fprintf(stderr,"SOWINGCFT AFTER1 frac[%s]= %g standNR: %d PFT: %s pftid: %d irrigation: %d\n",
////                    cropstand->type->name,cropstand->frac,pos,pft->par->name,pft->par->id,data->irrigation);
////             data=stand->data;
////             foreachpft(pft,p,&stand->pftlist)
////                fprintf(stderr,"SOWINGCFT AFTER2 frac[%s]= %g standNR: %d PFT: %s pftid: %d irrigation: %d\n",
////                    stand->type->name,stand->frac,s,pft->par->name,pft->par->id,data->irrigation);
////           }
//          cutpfts(cropstand,config);
//          //reclaim_land(stand,cropstand,cell,config->istimber,npft+ncft,config);
//          set_irrigsystem(cropstand,cft,npft,ncft,config);
//          //setaside(cell,cropstand,cell->ml.with_tillage,FALSE,npft,irrig,stand->soil.iswetland,year,config);
//          setasidestand=getstand(cell->standlist,pos);
//          cultcftstand(flux_estab,alloc_today,cell,cropstand,irrig,wtype,nofallow,npft,ncft,cft,year,day,config);
//
////         if(year==1512)
////          {
////            fprintf(stderr,"CHECKLU: irrig: %d  cft: %d landfrac: %g \n",irrig,cft,cell->standlist,cell->ml.landfrac[irrig].crop[cft]);
////            foreachpft(pft,p,&cropstand->pftlist)
////               fprintf(stderr,"SOWINGCFT AFTER2.1 frac[%s]= %g standNR: %d PFT: %s pftid: %d irrigation: %d\n",cropstand->type->name,cropstand->frac,pos,pft->par->name,pft->par->id,data->irrigation);
////            data=stand->data;
////            foreachpft(pft,p,&stand->pftlist)
////               fprintf(stderr,"SOWINGCFT AFTER2.2 frac[%s]= %g standNR: %d PFT: %s pftid: %d irrigation: %d\n",stand->type->name,stand->frac,s,pft->par->name,pft->par->id,data->irrigation);
////            foreachstand(stand,s,cell->standlist)
////                foreachpft(pft,p,&stand->pftlist)
////                 fprintf(stderr,"sowingcft3 year: %d frac[%s]= %g standNR: %d PFT: %s \n",year,stand->type->name,stand->frac,s,pft->par->name);
////          }
//        }//if too large stand->frac
//       if(difffrac>epsilon)
//        break;
//      } // if AGRICULTURE
//    } // foreachstand
//  }
#ifdef CHECK_BALANCE
  Real end=0;
  flux_carbon=cell->balance.flux_estab.carbon+flux_estab->carbon-flux_carbon;
  foreachstand(stand,s,cell->standlist)
  {
    end+=(standstocks(stand).carbon + soilmethane(&stand->soil))*stand->frac;
  }
  if (fabs(end-start-flux_carbon)>0.01)
         fprintf(stdout, "C-ERROR-SOWINGCFT: day: %d    %g start: %g  end: %g  sflux_estab.carbon: %g sbalance.flux_estab: %g flux_estab.carbon: %g balance.flux_estab: %g flux_carbon: %g \n",
           day,end-start-flux_carbon,start, end,sflux_estab,balance_carbon,flux_estab->carbon,cell->balance.flux_estab.carbon, flux_carbon);

#endif
} /* of 'sowingcft' */

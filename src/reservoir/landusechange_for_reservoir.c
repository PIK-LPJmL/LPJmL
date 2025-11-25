/**************************************************************************************/
/**                                                                                \n**/
/**     l a n d u s e c h a n g e _ f o r _ r e s e r v o i r . c                  \n**/
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
#include "tree.h"
#include "grass.h"
#include "natural.h"
#include "agriculture.h"
#include "soil.h"

static void deforest_for_reservoir(Cell *cell,    /**< pointer to cell */
                                   Real *difffrac, /**< fraction to deforest */
                                   Bool luc_timber, /**< setting land-use change timber */
                                   int ntotpft,   /**< total number of PFTs */
                                   int year,
                                   const Config *config /**< LPJmL configuration */
                                  )               /** \return void */
{
  int s,pos;
  Stand *natstand,*cutstand;
  Stocks stocks;
/* reduce the area of natural land here that is free for reservoirs than */

  int iteration = 0;
  while (*difffrac > epsilon && iteration < 2)
  {
    s = findlandusetype(cell->standlist, NATURAL);
    if (s == NOT_FOUND)
      s = findlandusetype(cell->standlist, WETLAND);
    if (s != NOT_FOUND)
    {
      pos = addstand(&natural_stand, cell) - 1;
      natstand = getstand(cell->standlist, s);
      cutstand = getstand(cell->standlist, pos);
      if (*difffrac + epsilon >= natstand->frac)
        cutstand->frac = natstand->frac;
      else
        cutstand->frac=*difffrac;

      reclaim_land(natstand, cutstand, cell, config->luc_timber, ntotpft, config);

      if (*difffrac + epsilon >= natstand->frac)
      {
        *difffrac -= natstand->frac;
        cutstand->frac = natstand->frac;
        delstand(cell->standlist, s);
        pos = findlandusetype(cell->standlist, NATURAL);
       }
      else
      {
        natstand->frac -= *difffrac;
        *difffrac = 0;
      }

      /* all the water from the cutstand goes in the reservoir */
      cell->ml.resdata->dmass+=soilwater(&cutstand->soil)*cutstand->frac*cell->coord.area;

      /* all the carbon and nitrogen from the reservoir goes into a pool  */
      stocks=soilstocks(&cutstand->soil);
      cell->ml.resdata->pool.carbon+=(stocks.carbon+soilmethane(&cutstand->soil)*WC/WCH4)*cutstand->frac;
      cell->ml.resdata->pool.nitrogen+=stocks.nitrogen*cutstand->frac;
      /* delete the cutstand */
      delstand(cell->standlist,pos);
    }

    iteration++;
  }


  if(*difffrac>epsilon)
    fail(NO_NATURAL_STAND_ERR,TRUE,TRUE,"No natural stand for deforest_for_reservoir()");

} /* of 'deforest_for_reservoir' */

static Real from_setaside_for_reservoir(Cell *cell,          /**< pointer to cell */
                                        Real difffrac,       /**< requested reservoir fraction */
                                        Bool intercrop,      /**< intercrop setting */
                                        int npft,            /**< number of PFTs */
                                        int ncft,            /**< number of CFTs */
                                        int year,            /**< simulation year */
                                        const Config *config /**< LPJmL configuration */
                                       )                     /** \return reservoir fraction that could be created from setaside */
{
  int s,s2,pos;
  Stand *setasidestand,*setasidestand_ir,*cutstand, *stand;
  Real factor;
  Stocks stocks;
  String line;
#ifndef IMAGE
  Real carbon;
  Real totw_before,totw_after,balanceW;
  Stocks tot_before={0,0},tot_after={0,0},balance={0,0},stocks2={0,0}; /* to check the water and c balance in the cells */
  totw_before=cell->balance.awater_flux+cell->balance.excess_water;
  foreachstand(stand,s,cell->standlist)
    totw_before+=soilwater(&stand->soil)*stand->frac;
  totw_before+=(cell->discharge.dmass_lake)/cell->coord.area;
  totw_before+=cell->ml.resdata->dmass/cell->coord.area;
  totw_before+=cell->ml.resdata->dfout_irrigation/cell->coord.area;
  /* carbon */
  foreachstand(stand,s,cell->standlist)
  {
    stocks2=standstocks(stand);
    tot_before.carbon+=(stocks2.carbon+soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
    tot_before.nitrogen+=stocks2.nitrogen*stand->frac;
  }
  carbon=tot_before.carbon;
  tot_before.carbon+=cell->ml.resdata->pool.carbon;
  tot_before.nitrogen+=cell->ml.resdata->pool.nitrogen;
  tot_before.carbon+=cell->balance.timber_harvest.carbon;
  tot_before.nitrogen+=cell->balance.timber_harvest.nitrogen;
  tot_before.carbon+=cell->balance.deforest_emissions.carbon;
  tot_before.nitrogen+=cell->balance.deforest_emissions.nitrogen;
  tot_before.carbon-=cell->balance.flux_estab.carbon;
  tot_before.nitrogen-=cell->balance.flux_estab.nitrogen;
#endif

  s=s2=NOT_FOUND;
  /* first check if there is any SETASIDE in the cell */
  s=findlandusetype(cell->standlist,SETASIDE_RF);
  if(s==NOT_FOUND)
   s=findlandusetype(cell->standlist,SETASIDE_IR);
  else
   s2=findlandusetype(cell->standlist,SETASIDE_IR);
  if(s2==NOT_FOUND)
    s2=findlandusetype(cell->standlist,SETASIDE_WETLAND);

  if(s!=NOT_FOUND)
  {
    setasidestand=getstand(cell->standlist,s);

    factor=0.0; /* initialize factor with 0, assumed that there is enough setaside stand in the cell */

    /* check if there is ENOUGH setaside stand to put the reservoir */

    if(difffrac>setasidestand->frac+epsilon) /* IF not: reduce cropland with factor and add to setasidestand */
    {
      if(s2!=NOT_FOUND) /* if irrigated setaside is available, merge as much as needed into the rainfed setaside*/
      {
        setasidestand_ir=getstand(cell->standlist,s2);
        if((difffrac-setasidestand->frac)>=setasidestand_ir->frac)
        {
          /* remove all vegetation on irrigated setaside */
          cutpfts(setasidestand_ir,config);
          mixsetaside(setasidestand,setasidestand_ir,intercrop,year,npft+ncft,config);
          delstand(cell->standlist,s2);                                                        // this changes the order of the standlist
        }
        else
        {
          pos=addstand(&natural_stand,cell)-1;
          cutstand=getstand(cell->standlist,pos);
          cutstand->frac=difffrac-setasidestand->frac;
          reclaim_land(setasidestand_ir,cutstand,cell,config->luc_timber,npft+ncft,config);
          setasidestand_ir->frac-=difffrac-setasidestand->frac;
          mixsetaside(setasidestand,cutstand,intercrop,year,npft+ncft,config);       // here setaside fraction is increased
          delstand(cell->standlist,pos);
        }
      }

      if(difffrac>setasidestand->frac)                         // second time if we need to reduce SETASIDE_WETLAND as well
      {
        s=s2=NOT_FOUND;
        s=findlandusetype(cell->standlist,SETASIDE_RF);
        if(s==NOT_FOUND)
         s=findlandusetype(cell->standlist,SETASIDE_IR);
        else
         s2=findlandusetype(cell->standlist,SETASIDE_IR);
        if(s2==NOT_FOUND)
          s2=findlandusetype(cell->standlist,SETASIDE_WETLAND);
        setasidestand=getstand(cell->standlist,s);
        if(s2!=NOT_FOUND)                                             //if irrigated setaside is available, merge as much as needed second time needed if all setasides exist 2 times becao
        {
          setasidestand_ir=getstand(cell->standlist,s2);
          if((difffrac-setasidestand->frac)>=setasidestand_ir->frac)
          {
            /* remove all vegetation on irrigated setaside */
            cutpfts(setasidestand_ir,config);
            mixsetaside(setasidestand,setasidestand_ir,intercrop,year,npft+ncft,config);
            delstand(cell->standlist,s2);
          }
          else
          {
            pos=addstand(&natural_stand,cell)-1;
            cutstand=getstand(cell->standlist,pos);
            cutstand->frac=difffrac-setasidestand->frac;
            reclaim_land(setasidestand_ir,cutstand,cell,config->luc_timber,npft+ncft,config);
            setasidestand_ir->frac-=difffrac-setasidestand->frac;
            mixsetaside(setasidestand,cutstand,intercrop,year,npft+ncft,config);
            delstand(cell->standlist,pos);
          }
        }
      }
      if(difffrac>setasidestand->frac)
      {
        s2=NOT_FOUND;
        s2=findlandusetype(cell->standlist,SETASIDE_RF);
        if(s2==NOT_FOUND)
          s2=findlandusetype(cell->standlist,SETASIDE_IR);
        if(s2==NOT_FOUND)
          s2=findlandusetype(cell->standlist,SETASIDE_WETLAND);
        setasidestand=getstand(cell->standlist,s2);
        factor=min(1,(difffrac-setasidestand->frac)/(cell->ml.cropfrac_rf+cell->ml.cropfrac_ir+cell->ml.cropfrac_wl-setasidestand->frac));
        if(factor>1.0+epsilon)
          fprintf(stderr,"ERROR187: factor=%g >1 in cell (%s).\n",
                  factor,sprintcoord(line,&cell->coord));
        foreachstand(stand,s,cell->standlist)
          if(stand->type->landusetype!=NATURAL  && stand->type->landusetype!=WETLAND &&
             stand->type->landusetype!=SETASIDE_RF && stand->type->landusetype!=SETASIDE_IR &&
             stand->type->landusetype!=SETASIDE_WETLAND && stand->type->landusetype!=KILL)
          {
            if((difffrac-setasidestand->frac+epsilon)>stand->frac)
            {
              /* remove all vegetation on irrigated setaside */
              cutpfts(stand,config);
              mixsetaside(setasidestand,stand,intercrop,year,npft+ncft,config);
              delstand(cell->standlist,s);
              s--;
            }
            else
            {
              pos=addstand(&natural_stand,cell)-1;
              cutstand=getstand(cell->standlist,pos);
              cutstand->frac=difffrac-setasidestand->frac;
              reclaim_land(stand,cutstand,cell,config->luc_timber,npft+ncft,config);
              stand->frac-=cutstand->frac;
              mixsetaside(setasidestand,cutstand,intercrop,year,npft+ncft,config);
              delstand(cell->standlist,pos);
            }
          }
      }
    }
  }
  else /* if there IS no setaside in the the cell: */
  {
    factor=difffrac/(cell->ml.cropfrac_rf+cell->ml.cropfrac_ir+cell->ml.cropfrac_wl);
    foreachstand(stand,s,cell->standlist)
    if(stand->type->landusetype!=NATURAL && stand->type->landusetype!=WETLAND && stand->type->landusetype!=KILL) /* existence of SETASIDES has been ruled out */
    {
      pos=addstand(&natural_stand,cell)-1;
      cutstand=getstand(cell->standlist,pos);
      cutstand->frac=factor*stand->frac;
      reclaim_land(stand,cutstand,cell,config->luc_timber,npft+ncft,config);
      stand->frac-=cutstand->frac;
      if(setaside(cell,getstand(cell->standlist,pos),cell->ml.with_tillage,intercrop,npft,ncft,FALSE,FALSE,year,config))
        delstand(cell->standlist,pos);
    }
  }

  /* if previous step correct, there is now enough setaside
   * created to put the be replaced by the reservoir
   */
  s=findlandusetype(cell->standlist,SETASIDE_RF);
  if(s==NOT_FOUND)
    s=findlandusetype(cell->standlist,SETASIDE_IR);
  if(s==NOT_FOUND)
    s=findlandusetype(cell->standlist,SETASIDE_WETLAND);

  setasidestand=getstand(cell->standlist,s);

  if(difffrac<=setasidestand->frac+epsilon)
  {
    pos=addstand(&natural_stand,cell)-1;
    cutstand=getstand(cell->standlist,pos);
    cutstand->frac=difffrac;
    reclaim_land(setasidestand,cutstand,cell,config->luc_timber,npft+ncft,config);
    setasidestand->frac-=difffrac;

    /* all the water from the cutstand goes in the reservoir */
    cell->ml.resdata->dmass+=soilwater(&cutstand->soil)*cutstand->frac*cell->coord.area;

    /* all the carbon from the reservoir goes into a pool */
    stocks=soilstocks(&cutstand->soil);
    cell->ml.resdata->pool.carbon+=(stocks.carbon+soilmethane(&cutstand->soil)*WC/WCH4)*cutstand->frac;
    cell->ml.resdata->pool.nitrogen+=stocks.nitrogen*cutstand->frac;

    /* delete the cutstand, and the setasidestand if empty */
    delstand(cell->standlist,pos);
    if(setasidestand->frac<epsilon)
      delstand(cell->standlist,s);

    if(factor>0.9999) /* If the total gridcell is occupied by the reservoir,
                         remove all stands */
      while(!isempty(cell->standlist))
        delstand(cell->standlist,0);

  }
  else
  {
    fprintstandlist(stderr,cell->standlist,config->pftpar,npft+ncft);
    fail(NO_SETASIDE_ERR,TRUE,TRUE,
        "Not enough setaside stand created to put the reservoir in cell (%s): diff=%g",
        sprintcoord(line,&cell->coord),difffrac-setasidestand->frac);
  }

#ifndef IMAGE
  totw_after=cell->balance.awater_flux+cell->balance.excess_water;
  foreachstand(stand,s,cell->standlist)
    totw_after+=soilwater(&stand->soil)*stand->frac;
  totw_after+=(cell->discharge.dmass_lake)/cell->coord.area;
  totw_after+=cell->ml.resdata->dmass/cell->coord.area;
  totw_after+=cell->ml.resdata->dfout_irrigation/cell->coord.area;
  stocks2.carbon=stocks2.nitrogen=0;
  /* carbon */
  foreachstand(stand,s,cell->standlist)
  {
    stocks2=standstocks(stand);
    tot_after.carbon+=(stocks2.carbon+soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
    tot_after.nitrogen+=stocks2.nitrogen*stand->frac;
  }
  carbon=tot_after.carbon;
  tot_after.carbon+=cell->ml.resdata->pool.carbon;
  tot_after.nitrogen+=cell->ml.resdata->pool.nitrogen;
  tot_after.carbon+=cell->balance.timber_harvest.carbon;
  tot_after.nitrogen+=cell->balance.timber_harvest.nitrogen;
  tot_after.carbon+=cell->balance.deforest_emissions.carbon;
  tot_after.nitrogen+=cell->balance.deforest_emissions.nitrogen;
  tot_after.carbon-=cell->balance.flux_estab.carbon;
  tot_after.nitrogen-=cell->balance.flux_estab.nitrogen;
  /* check if the same */
  balance.carbon=tot_before.carbon-tot_after.carbon;
  balance.nitrogen=tot_before.nitrogen-tot_after.nitrogen;
  balanceW=totw_before-totw_after;
  if(fabs(balanceW)>0.01)
  {
    fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,TRUE,
         "water balance error in cell (%g,%g) in from_setaside_for_reservoir, balanceW=%g dmass=%g  year: %d",
         cell->coord.lat,cell->coord.lon,balanceW, cell->ml.resdata->dmass/cell->coord.area,year);
    fflush(stderr);
  }
  if(fabs(balance.nitrogen)>0.001)
  {
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,TRUE,
         "nitrogen balance error in cell (%g,%g) infrom_setaside_for_reservoir, balanceN=%g year: %d",
         cell->coord.lat,cell->coord.lon,balance.nitrogen,year);
    fflush(stderr);
  }
  if(fabs(balance.carbon)>0.001)
  {
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,TRUE,
         "carbon balance error in cell (%g,%g) stands: %d in from_setaside_for_reservoir balanceC=%g total.carbon: %g resdata->pool.carbon: %g timber: %g deforest: %g carbon: %g year: %d ",
         cell->coord.lat,cell->coord.lon,cell->standlist->n,balance.carbon,tot_after.carbon,cell->ml.resdata->pool.carbon,cell->balance.timber_harvest.carbon,cell->balance.deforest_emissions.carbon,carbon,year);
    fflush(stderr);
  }
#endif
  return difffrac;
} /* of 'from setaside for reservoir' */

void landusechange_for_reservoir(Cell *cell,          /**< pointer to cell */
                                 int npft,            /**< number of natural PFTs */
                                 int ncft,            /**< number of CFTs */
                                 Bool intercrop,      /**< intercrop setting */
                                 int year,            /**< simulation year */
                                 const Config *config /**< LPJmL configuration */
                                )                     /** \return void */
/* needs to be called before establishment, to ensure that regrowth is possible in the
   following year */
{
  Real difffrac,difffrac2;
  Stand *stand;
  Pft *pft;
  Bool isrice=FALSE;
  Real sum[2],sum_wl;
  Real minnatfrac_res;
  int s,p;
#ifndef IMAGE
  Real totw_before,totw_after,balanceW;
  Stocks tot_before={0,0},tot_after={0,0},balance,stocks; /* to check the water and c balance in the cells */
#endif
  Irrigation *data;
#if defined IMAGE && defined COUPLED
  minnatfrac_res=0.0002;
#else
  minnatfrac_res=0;
#endif

  /** First check if a new dam causes fraction >1, natural stand is lowered with
   * dam area
   * if this is not possible the natural stand is removed and the setaside stands are reduced
   * if this is also not possible reduce the crops evenly until there is space for the reservoir
   **/
  if(check_stand_fracs_for_reservoir(cell,&difffrac))
  {
    /* total water and carbon calculation before the correction of fractions
     * for reservoir water
     */
#ifndef IMAGE
    totw_before=cell->balance.awater_flux+cell->balance.excess_water;
    foreachstand(stand,s,cell->standlist)
      totw_before+=soilwater(&stand->soil)*stand->frac;
    totw_before+=(cell->discharge.dmass_lake)/cell->coord.area;
    totw_before+=cell->ml.resdata->dmass/cell->coord.area;
    totw_before+=cell->ml.resdata->dfout_irrigation/cell->coord.area;
    /* carbon */
    foreachstand(stand,s,cell->standlist)
    {
      stocks=standstocks(stand);
      tot_before.carbon+=(stocks.carbon+soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
      tot_before.nitrogen+=stocks.nitrogen*stand->frac;
    }
    tot_before.carbon+=cell->ml.resdata->pool.carbon;
    tot_before.nitrogen+=cell->ml.resdata->pool.nitrogen;
    tot_before.carbon+=cell->balance.timber_harvest.carbon;
    tot_before.nitrogen+=cell->balance.timber_harvest.nitrogen;
    tot_before.carbon+=cell->balance.deforest_emissions.carbon;
    tot_before.nitrogen+=cell->balance.deforest_emissions.nitrogen;
    tot_before.carbon-=cell->balance.flux_estab.carbon;
    tot_before.nitrogen-=cell->balance.flux_estab.nitrogen;
#endif
    /* cut cut entire natural stand if lakefraction+reservoir fraction equals 1 */
    if(difffrac>=1-cell->lakefrac-minnatfrac_res)
      minnatfrac_res=0.0;
    difffrac2=difffrac;
    /* first choice: deforest natural land */
    if(difffrac>epsilon && (1-cell->lakefrac-cell->ml.cropfrac_rf-cell->ml.cropfrac_ir-cell->ml.cropfrac_wl-minnatfrac_res)>=difffrac)
    {  /* deforestation to built the reservoir */

       s=findlandusetype(cell->standlist,NATURAL);
       if(s==NOT_FOUND)
         s=findlandusetype(cell->standlist,WETLAND);
       if(s!=NOT_FOUND){
         stand=getstand(cell->standlist,s);
         deforest_for_reservoir(cell,&difffrac,config->luc_timber,npft+ncft,year,config);
         difffrac2=difffrac;
       }
    }

    /* update the cropfactor to include the 'new' setaside */
    isrice=FALSE;
    sum_wl=sum[0]=sum[1]=0;
    foreachstand(stand,s,cell->standlist)
    {
      if(stand->type->landusetype!=NATURAL && stand->type->landusetype!=WETLAND && stand->type->landusetype!=KILL)
      {
        data=stand->data;
        foreachpft(pft, p, &stand->pftlist)
          if(!strcmp(pft->par->name,"rice")) isrice=TRUE;
        if(isrice==TRUE||stand->type->landusetype==SETASIDE_WETLAND)
          sum_wl+=stand->frac;
        else
          sum[data->irrigation]+=stand->frac;

      }
      isrice=FALSE;
    }
    cell->ml.cropfrac_rf=sum[0];
    cell->ml.cropfrac_ir=sum[1];/* could be different from landusefraction input, due to not harvested winter cereals */
    cell->ml.cropfrac_wl=sum_wl;


    /* if this is not possible: deforest all the natural land and then reduce crops  */
    if(difffrac>epsilon && (1-cell->lakefrac-cell->ml.cropfrac_rf-cell->ml.cropfrac_ir-cell->ml.cropfrac_wl-minnatfrac_res)<difffrac)
    {
      s=findlandusetype(cell->standlist,NATURAL); /* 1 check if everyting is deforested */
      if(s==NOT_FOUND)
        s=findlandusetype(cell->standlist,WETLAND);
      if(s!=NOT_FOUND)
      {
        stand=getstand(cell->standlist,s);
        deforest(cell,difffrac,intercrop,npft,FALSE,FALSE,stand->soil.iswetland,ncft,year,minnatfrac_res,config); /* 1 deforest */
        difffrac2-=stand->frac;
      }
      s=findlandusetype(cell->standlist,NATURAL); /* 1. check if everyting is deforested */
      if(s==NOT_FOUND)
        s=findlandusetype(cell->standlist,WETLAND);
      if(s!=NOT_FOUND && difffrac2>minnatfrac_res)
      {
        stand=getstand(cell->standlist,s);
        if(stand->frac>minnatfrac_res+epsilon)
        {
          s=findlandusetype(cell->standlist,NATURAL);
          if(s!=NOT_FOUND)
          {
            stand=getstand(cell->standlist,s);
            deforest(cell,difffrac2,intercrop,npft,FALSE,FALSE,stand->soil.iswetland,ncft,year,minnatfrac_res,config); /* 1 deforest */
            difffrac2-=stand->frac;
          }
          if(s==NOT_FOUND)
            s=findlandusetype(cell->standlist,WETLAND);
          if(s!=NOT_FOUND && difffrac2>minnatfrac_res)
          {
            stand=getstand(cell->standlist,s);
            deforest(cell,difffrac2,intercrop,npft,FALSE,FALSE,stand->soil.iswetland,ncft,year,minnatfrac_res,config); /* 1 deforest */
            difffrac2-=stand->frac;
          }
          s=findlandusetype(cell->standlist,NATURAL); /* 2. check if everyting is deforested */
          if(s==NOT_FOUND)
            s=findlandusetype(cell->standlist,WETLAND);
          if(s!=NOT_FOUND)
          {
            stand=getstand(cell->standlist,s);
            fprintf(stderr,"deforest reservoir 2 wrong loop %d difffrac: %g diffrac2: %g %g %g %g %g  %g %g \n in cell lon %.2f lat %.2f\n",
                   s,difffrac,difffrac2,
                   1-cell->lakefrac-cell->ml.cropfrac_rf-cell->ml.cropfrac_ir-cell->ml.cropfrac_wl,
                   cell->lakefrac,cell->ml.cropfrac_rf, cell->ml.cropfrac_ir,cell->ml.cropfrac_wl,
                   cell->ml.reservoirfrac, cell->coord.lon, cell->coord.lat);
            fflush(stderr);
 //           fail(FOREST_LEFT_ERR,TRUE,TRUE,
 //                "wrong loop, there is still natural land to deforest left");
          }
        }
      }

      /* update the cropfactor to include the 'new' setaside */
      isrice=FALSE;
      sum_wl=sum[0]=sum[1]=0;
      foreachstand(stand,s,cell->standlist)
      {
        if(stand->type->landusetype!=NATURAL && stand->type->landusetype!=WETLAND && stand->type->landusetype!=KILL)
        {
          data=stand->data;
          foreachpft(pft, p, &stand->pftlist)
            if(!strcmp(pft->par->name,"rice")) isrice=TRUE;
          if(isrice==TRUE||stand->type->landusetype==SETASIDE_WETLAND)
            sum_wl+=stand->frac;
          else
            sum[data->irrigation]+=stand->frac;

        }
        isrice=FALSE;
      }
      cell->ml.cropfrac_rf=sum[0];
      cell->ml.cropfrac_ir=sum[1];/* could be different from landusefraction input, due to not harvested winter cereals */
      cell->ml.cropfrac_wl=sum_wl;

      /*3 cut setaside stand to built the reservoir */
      if(difffrac>minnatfrac_res)
       difffrac-=from_setaside_for_reservoir(cell,difffrac,
                                            intercrop,npft,ncft,year,config);

      /* update the cropfactor */
      isrice=FALSE;
      sum_wl=sum[0]=sum[1]=0;
      foreachstand(stand,s,cell->standlist)
      {
        if(stand->type->landusetype!=NATURAL && stand->type->landusetype!=WETLAND && stand->type->landusetype!=KILL)
        {
          data=stand->data;
          foreachpft(pft, p, &stand->pftlist)
            if(!strcmp(pft->par->name,"rice")) isrice=TRUE;
          if(isrice==TRUE||stand->type->landusetype==SETASIDE_WETLAND)
            sum_wl+=stand->frac;
          else
            sum[data->irrigation]+=stand->frac;
        }
        isrice=FALSE;
      }
      cell->ml.cropfrac_rf=sum[0];
      cell->ml.cropfrac_ir=sum[1];/* could be different from landusefraction input, due to not harvested winter cereals */
      cell->ml.cropfrac_wl=sum_wl;
      cell->ml.cropfrac_wl=sum_wl;

    }
    /* total water and carbon calculation after the correction of fractions
     * for reservoir water
     */
#ifndef IMAGE /*  Because the timber harvest is not accounted for in the carbon balance check*/
    totw_after=cell->balance.awater_flux+cell->balance.excess_water;
    foreachstand(stand,s,cell->standlist)
      totw_after+=soilwater(&stand->soil)*stand->frac;
    totw_after+=(cell->discharge.dmass_lake)/cell->coord.area;
    totw_after+=cell->ml.resdata->dmass/cell->coord.area;
    totw_after+=cell->ml.resdata->dfout_irrigation/cell->coord.area;
    stocks.carbon=stocks.nitrogen=0;
    /* carbon */
    foreachstand(stand,s,cell->standlist)
    {
      stocks=standstocks(stand);
      tot_after.carbon+=(stocks.carbon+soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
      tot_after.nitrogen+=stocks.nitrogen*stand->frac;
    }
    tot_after.carbon+=cell->ml.resdata->pool.carbon;
    tot_after.nitrogen+=cell->ml.resdata->pool.nitrogen;
    tot_after.carbon+=cell->balance.timber_harvest.carbon;
    tot_after.nitrogen+=cell->balance.timber_harvest.nitrogen;
    tot_after.carbon+=cell->balance.deforest_emissions.carbon;
    tot_after.nitrogen+=cell->balance.deforest_emissions.nitrogen;
    tot_after.carbon-=cell->balance.flux_estab.carbon;
    tot_after.nitrogen-=cell->balance.flux_estab.nitrogen;
    /* check if the same */
    balance.carbon=tot_before.carbon-tot_after.carbon;
    balance.nitrogen=tot_before.nitrogen-tot_after.nitrogen;
#endif

#ifndef IMAGE /*  Because the timber harvest is not accounted for in the carbon balance check*/
    balanceW=totw_before-totw_after;
    if(fabs(balanceW)>0.01)
      fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,TRUE,
           "water balance error in cell (%g,%g) in the building of the reservoir, balanceW=%g dmass=%g  year: %d",
           cell->coord.lat,cell->coord.lon,balanceW, cell->ml.resdata->dmass/cell->coord.area,year);
    if(fabs(balance.nitrogen)>0.001)
    {
      fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,TRUE,
           "nitrogen balance error in cell (%g,%g) in the building of the reservoir, balanceN=%g year: %d",
           cell->coord.lat,cell->coord.lon,balance.nitrogen,year);
      fflush(stderr);
    }
    if(fabs(balance.carbon)>0.001)
      fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,TRUE,
           "carbon balance error in cell (%g,%g) in the building of the reservoir, balanceC=%g year: %d after: %.5f before: %.5f resdata: %.5f timber: %.5f deforest: %.5f estab: %.5f ",
           cell->coord.lat,cell->coord.lon,balance.carbon,year,tot_after.carbon,tot_before.carbon,cell->ml.resdata->pool.carbon,cell->balance.timber_harvest.carbon,cell->balance.deforest_emissions.carbon,cell->balance.flux_estab.carbon);
#endif

    /* check if total fractions add up to 1 again */
    check_stand_fracs(cell,cell->lakefrac+cell->ml.reservoirfrac,ncft);
  }
} /* end of function */

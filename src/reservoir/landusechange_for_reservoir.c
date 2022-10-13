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
                                   Real difffrac, /**< fraction to deforest */
                                   Bool istimber, /**< setting istimber */
                                   int ntotpft,   /**< total number of PFTs */
                                   const Config *config
                                  )               /** \return void */
{
  int s,pos;
  Stand *natstand,*cutstand;
  Stocks stocks;
  s=findlandusetype(cell->standlist,NATURAL);
  if(s!=NOT_FOUND)
  {
    pos=addstand(&natural_stand,cell)-1;
    natstand=getstand(cell->standlist,s);
    cutstand=getstand(cell->standlist,pos);
    cutstand->frac=difffrac;
    reclaim_land(natstand,cutstand,cell,istimber,ntotpft,config);
    if(difffrac+epsilon>=natstand->frac)
    {
      difffrac=natstand->frac;
      delstand(cell->standlist,s);
      pos=s;
    }
    else
      natstand->frac-=difffrac;


    /* all the water from the cutstand goes in the reservoir */
    cell->ml.resdata->dmass+=soilwater(&cutstand->soil)*cutstand->frac*cell->coord.area;
    /* all the carbon and nitrogen from the reservoir goes into a pool  */
    stocks=soilstocks(&cutstand->soil);
    cell->ml.resdata->pool.carbon+=stocks.carbon*cutstand->frac;
    cell->ml.resdata->pool.nitrogen+=stocks.nitrogen*cutstand->frac;
    /* delete the cutstand */
    delstand(cell->standlist,pos);
  }
  else
    fail(NO_NATURAL_STAND_ERR,TRUE,"No natural stand for deforest_for_reservoir()");

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
  Irrigation *data;

  /* first check if there is any SETASIDE in the cell */
  s=findlandusetype(cell->standlist,SETASIDE_RF);
  s2=findlandusetype(cell->standlist,SETASIDE_IR);
  /* if there IS already setaside in the the cell: */
  if(s!=NOT_FOUND)
  {
    setasidestand=getstand(cell->standlist,s);
    factor=0.0; /* initialize factor with 0, assumed that there is enough setaside stand in the cell */
    /* check if there is ENOUGH setaside stand to put the reservoir */
    if(difffrac>setasidestand->frac+epsilon) /* IF not: reduce cropland with factor and add to
                                        setasidestand */
    {
      if(s2!=NOT_FOUND) /* if irrigated setaside is available, merge as much as needed */
      {
        setasidestand_ir=getstand(cell->standlist,s2);
        if(difffrac+epsilon>=setasidestand->frac+setasidestand_ir->frac)
        {
          /* remove all vegetation on irrigated setaside */
          cutpfts(setasidestand_ir,config);
          mixsetaside(setasidestand,setasidestand_ir,intercrop,year,config);
          delstand(cell->standlist,s2);
        }
        else{
          pos=addstand(&natural_stand,cell)-1;
          cutstand=getstand(cell->standlist,pos);
          cutstand->frac=difffrac-setasidestand->frac;
          reclaim_land(setasidestand_ir,cutstand,cell,config->istimber,npft+ncft,config);
          setasidestand_ir->frac-=difffrac-setasidestand->frac;
          mixsetaside(setasidestand,cutstand,intercrop,year,config);
          delstand(cell->standlist,pos);
        }
      }
      if(difffrac>setasidestand->frac+epsilon)
      {
        factor=(difffrac-setasidestand->frac)/(cell->ml.cropfrac_rf+cell->ml.cropfrac_ir+cell->ml.cropfrac_wl[0]+cell->ml.cropfrac_wl[1]-setasidestand->frac);
        fprintf(stderr,"WARNING025: not enough setasidestand in cell (%s) to put the reservoir, reduce cropland by %g\n",sprintcoord(line,&cell->coord),factor);
        if(factor>1.0+epsilon)
          fprintf(stderr,"ERROR187: factor=%g >1 in cell (%s).\n",
                factor,sprintcoord(line,&cell->coord));
        foreachstand(stand,s,cell->standlist)
          if(stand->type->landusetype!=NATURAL  && stand->type->landusetype!=WETLAND &&
            stand->type->landusetype!=SETASIDE_RF && stand->type->landusetype!=SETASIDE_WETLAND &&
            stand->type->landusetype!=SETASIDE_IR)
         {
            data=stand->data;
            pos=addstand(&natural_stand,cell)-1;
            cutstand=getstand(cell->standlist,pos);
            cutstand->frac=factor*stand->frac;
            reclaim_land(stand,cutstand,cell,config->istimber,npft+ncft,config);
            stand->frac-=cutstand->frac;

            cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount+cutstand->soil.litter.agtop_moist)*cell->coord.area*cutstand->frac;
            cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount+cutstand->soil.litter.agtop_moist)*cutstand->frac;

            if(setaside(cell,getstand(cell->standlist,pos),cell->ml.with_tillage,intercrop,npft,FALSE,stand->soil.iswetland,year,config))
             delstand(cell->standlist,pos);
        }
      }
    }
  }
  else /* if there IS no setaside in the the cell: */
  {
    factor=difffrac/(cell->ml.cropfrac_rf+cell->ml.cropfrac_ir+cell->ml.cropfrac_wl[0]+cell->ml.cropfrac_wl[1]);
    fprintf(stderr,"WARNING026: no setasidestand in cell (%s) to put the reservoir, reduce cropland by %g\n",sprintcoord(line,&cell->coord),factor);
    foreachstand(stand,s,cell->standlist)
      if(stand->type->landusetype!=NATURAL && stand->type->landusetype!=WETLAND) /*&&
         stand->type->landusetype!=SETASIDE)*/ /* existence of SETASIDES has been ruled out */
      {
        data=stand->data;
        pos=addstand(&natural_stand,cell)-1;
        cutstand=getstand(cell->standlist,pos);
        cutstand->frac=factor*stand->frac;
        reclaim_land(stand,cutstand,cell,config->istimber,npft+ncft,config);
        stand->frac-=cutstand->frac;

        cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount+cutstand->soil.litter.agtop_moist)*cell->coord.area*cutstand->frac;
        cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount+cutstand->soil.litter.agtop_moist)*cutstand->frac;

        if(setaside(cell,getstand(cell->standlist,pos),cell->ml.with_tillage,intercrop,npft,FALSE,stand->soil.iswetland,year,config))
          delstand(cell->standlist,pos);
      }

  }

  /* if previous step correct, there is now enough setaside
   * created to put the be replaced by the reservoir
   */
  s=findlandusetype(cell->standlist,SETASIDE_RF);
  setasidestand=getstand(cell->standlist,s);

  if(difffrac<=setasidestand->frac+epsilon)
  {
    pos=addstand(&natural_stand,cell)-1;
    cutstand=getstand(cell->standlist,pos);
    cutstand->frac=difffrac;
    reclaim_land(setasidestand,cutstand,cell,config->istimber,npft+ncft,config);
    setasidestand->frac-=difffrac;

    /* all the water from the cutstand goes in the reservoir */
    cell->ml.resdata->dmass+=soilwater(&cutstand->soil)*cutstand->frac*cell->coord.area;

    /* all the carbon from the reservoir goes into a pool */
    stocks=soilstocks(&cutstand->soil);
    cell->ml.resdata->pool.carbon+=stocks.carbon*cutstand->frac;
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
    fail(NO_SETASIDE_ERR,TRUE,
         "Not enough setaside stand created to put the reservoir in cell (%s): diff=%g",
         sprintcoord(line,&cell->coord),difffrac-setasidestand->frac);

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
  Real difffrac;
  Stand *stand;
  Real sum[2],sum_wl[2];
  Real minnatfrac_res;
  int s;
  Real totw_before,totw_after,balanceW;
  Stocks tot_before={0,0},tot_after={0,0},balance,stocks; /* to check the water and c balance in the cells */
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
    //fprintf(stderr,"reservoir caused frac >1 in cell (%g/%g). diffrac = %.5f, reservoirfrac = %f\n",cell->coord.lon,cell->coord.lat,difffrac,cell->reservoirfrac);
    /* total water and carbon calculation before the correction of fractions
     * for reservoir water
     */
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
      tot_before.carbon+=stocks.carbon*stand->frac;
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
    /* cut cut entire natural stand if lakefraction+reservoir fraction equals 1 */
    if(difffrac>=1-cell->lakefrac-minnatfrac_res)
      minnatfrac_res=0.0;

    /* first choice: deforest natural land */
    if(difffrac>epsilon && (1-cell->lakefrac-cell->ml.cropfrac_rf-cell->ml.cropfrac_ir-cell->ml.cropfrac_wl[0]-cell->ml.cropfrac_wl[1]-minnatfrac_res)>=difffrac)
    {  /* deforestation to built the reservoir */
       s=findlandusetype(cell->standlist,NATURAL);
       if(s!=NOT_FOUND)
         deforest_for_reservoir(cell,difffrac,config->istimber,npft+ncft,config);
    }
    /* if this is not possible: deforest all the natural land and then reduce crops  */
    if(difffrac>epsilon && 1-cell->lakefrac-cell->ml.cropfrac_rf-cell->ml.cropfrac_ir-cell->ml.cropfrac_wl[0]-cell->ml.cropfrac_wl[1]-minnatfrac_res<difffrac)
    {
      deforest(cell,difffrac,intercrop,npft,FALSE,FALSE,FALSE,ncft,year,minnatfrac_res,config); /* 1 deforest */
      s=findlandusetype(cell->standlist,NATURAL); /* 2 check if everyting is deforested */
      if(s!=NOT_FOUND)
      {
        stand=getstand(cell->standlist,s);
        if(stand->frac>minnatfrac_res+epsilon)
        {
          printf("defor res 1 wrong loop %d %g %g %g %g %g %g %g %g \n in cell lon %.2f lat %.2f\n",
                 s,difffrac,
                 1-cell->lakefrac-cell->ml.cropfrac_rf-cell->ml.cropfrac_ir-cell->ml.cropfrac_wl[0]-cell->ml.cropfrac_wl[1],
                 cell->lakefrac,cell->ml.cropfrac_rf, cell->ml.cropfrac_ir,cell->ml.cropfrac_wl[0],cell->ml.cropfrac_wl[1],
                 cell->ml.reservoirfrac, cell->coord.lon, cell->coord.lat);
          fflush(stdout);
/*        fail(FOREST_LEFT_ERR,TRUE,
               "wrong loop, there is still natural land to deforest left"); */

          deforest(cell,difffrac,intercrop,npft,FALSE,FALSE,FALSE,ncft,year,minnatfrac_res,config); /* 1 deforest */
          s=findlandusetype(cell->standlist,NATURAL); /* 2 check if everyting is deforested */
          if(s!=NOT_FOUND)
          {
            printf("defor res 2 wrong loop %d %g %g %g %g %g %g %g %g\n in cell lon %.2f lat %.2f\n",
                   s,difffrac,
                   1-cell->lakefrac-cell->ml.cropfrac_rf-cell->ml.cropfrac_ir-cell->ml.cropfrac_wl[0]-cell->ml.cropfrac_wl[1],
                   cell->lakefrac,cell->ml.cropfrac_rf, cell->ml.cropfrac_ir,cell->ml.cropfrac_wl[0],cell->ml.cropfrac_wl[1],
                   cell->ml.reservoirfrac, cell->coord.lon, cell->coord.lat);
            fflush(stdout);
            fail(FOREST_LEFT_ERR,TRUE,
                 "wrong loop, there is still natural land to deforest left");
          }
        }
      }

      /* update the cropfactor to include the 'new' setaside */
      sum[0]=sum[1]=sum_wl[0]=sum_wl[1]=0.0;
      foreachstand(stand,s,cell->standlist)
        if(stand->type->landusetype!=NATURAL && stand->type->landusetype!=WETLAND){
          data=stand->data;
          sum[data->irrigation]+=stand->frac;
          if(stand->soil.iswetland)
            sum_wl[data->irrigation]+=stand->frac;
        }
      cell->ml.cropfrac_rf=sum[0]-sum_wl[0];
      cell->ml.cropfrac_ir=sum[1]-sum_wl[1];
      cell->ml.cropfrac_wl[0]=sum_wl[0];
      cell->ml.cropfrac_wl[1]=sum_wl[1];

      difffrac-=from_setaside_for_reservoir(cell,difffrac,
                                            intercrop,npft,ncft,year,config);
      /*3 cut setaside stand to built the reservoir */

      /* update the cropfactor */
      sum[0]=sum[1]=sum_wl[0]=sum_wl[1]=0.0;
      foreachstand(stand,s,cell->standlist)
        if(stand->type->landusetype!=NATURAL && stand->type->landusetype!=WETLAND){
          data=stand->data;
          sum[data->irrigation]+=stand->frac;
          if(stand->soil.iswetland)
            sum_wl[data->irrigation]+=stand->frac;
        }
      cell->ml.cropfrac_rf=sum[0]-sum_wl[0];
      cell->ml.cropfrac_ir=sum[1]-sum_wl[1];
      cell->ml.cropfrac_wl[0]=sum_wl[0];
      cell->ml.cropfrac_wl[1]=sum_wl[1];

    }
    /* total water and carbon calculation after the correction of fractions
     * for reservoir water
     */
    totw_after=cell->balance.awater_flux+cell->balance.excess_water;
    foreachstand(stand,s,cell->standlist)
      totw_after+=soilwater(&stand->soil)*stand->frac;
    totw_after+=(cell->discharge.dmass_lake)/cell->coord.area;
    totw_after+=cell->ml.resdata->dmass/cell->coord.area;
    totw_after+=cell->ml.resdata->dfout_irrigation/cell->coord.area;
    /* carbon */
    foreachstand(stand,s,cell->standlist)
    {
      stocks=standstocks(stand);
      tot_after.carbon+=stocks.carbon*stand->frac;
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
    balanceW=totw_before-totw_after;

#ifndef IMAGE /*  Because the timber harvest is not accounted for in the carbon balance check*/
    if(fabs(balanceW)>0.01)
#ifdef NO_FAIL_BALANCE
      fprintf(stderr,"ERROR005: "
#else
      fail(INVALID_WATER_BALANCE_ERR,FALSE,
#endif
           "water balance error in the building of the reservoir, balanceW=%g",
           balanceW);
    if(fabs(balance.nitrogen)>0.1)
    {
#ifdef NO_FAIL_BALANCE
      fprintf(stderr,"ERROR037: "
#else
      fail(INVALID_NITROGEN_BALANCE_ERR,FALSE,
#endif
           "nitrogen balance error in cell (%g,%g) in the building of the reservoir, balanceN=%g",
           cell->coord.lat,cell->coord.lon,balance.nitrogen);
      fflush(stderr);
    }
    if(fabs(balance.carbon)>2)
#ifdef NO_FAIL_BALANCE
      fprintf(stderr,"ERROR004: "
#else
      fail(INVALID_CARBON_BALANCE_ERR,FALSE,
#endif
           "carbon balance error in the building of the reservoir, balanceC=%g",
           balance.carbon);
#endif
    /* check if total fractions add up to 1 again */
    check_stand_fracs(cell,cell->lakefrac+cell->ml.reservoirfrac,3);

  }
} /* end of function */

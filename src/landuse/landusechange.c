/**************************************************************************************/
/**                                                                                \n**/
/**               l  a  n  d  u  s  e  c  h  a  n  g  e  .  c                      \n**/
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
#include "natural.h"
#include "crop.h"
#include "grassland.h"
#include "agriculture.h"
#include "agriculture_tree.h"
#include "agriculture_grass.h"
#include "biomass_grass.h"
#include "biomass_tree.h"
#include "reservoir.h"
#include "woodplantation.h"
#include "wetland.h"
//#define CHECK_BALANCE

typedef enum {PASTURE=1, OTHER_PASTURE,BIOMASS_TREE_PLANTATION, BIOMASS_GRASS_PLANTATION, AGRICULTURE_TREE_PLANTATION, WOOD_PLANTATION } Cultivation_type;

#ifdef IMAGE
#define minnatfrac_luc 0.0002
#else
#define minnatfrac_luc 0.0
#endif

void deforest(Cell *cell,          /**< pointer to cell */
              Real difffrac,       /**< stand fraction to deforest (0..1) */
              Bool intercrop,      /**< intercropping possible (TRUE/FALSE) */
              int npft,            /**< number of natural PFTs */
              Bool timberharvest,
              Bool irrig,          /**< irrigated stand (TRUE/FALSE) */
              Bool iswetland,
              int ncft,            /**< number of crop PFTs */
              int year,            /**< simulation year (AD) */
              Real minnatfrac,     /**< minimum fraction of natural vegetation */
              const Config *config /**< LPJmL configuration */
             )
{
  int s,pos;
  Real difffrac_old=difffrac;
  Real difffrac_wetland=0;
  Stand *stand,*cutstand;
  String line;
#ifdef CHECK_BALANCE2
  int j;
#endif
#ifdef CHECK_BALANCE
//anpp, influx and arh do not change
  Stand *checkstand;
  Pft *pft;
  Stocks start = {0,0};
  Stocks end = {0,0};
  Stocks st;
  Stocks fluxes_fire= {0,0};
  Stocks fluxes_estab= {0,0};
  Stocks fluxes_neg= {0,0};
  Stocks fluxes_prod= {0,0};
  Stocks fluxes_firewood={0,0};
  Stocks balance= {0,0};
  Real start_w = 0;
  Real end_w = 0;
  int p;
  foreachstand(checkstand, s, cell->standlist)
  {
    st=standstocks(checkstand);
    start.carbon+=(st.carbon+ soilmethane(&checkstand->soil)*WC/WCH4)*checkstand->frac;//-stand->cell->balance.flux_estab.carbon;
    start.nitrogen+=st.nitrogen*checkstand->frac;//-stand->cell->balance.flux_estab.nitrogen;
    start_w += soilwater(&checkstand->soil)*checkstand->frac;
  }
  start.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  start.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;

  fluxes_fire=cell->balance.fire;
  fluxes_estab=cell->balance.flux_estab;
  fluxes_neg=cell->balance.neg_fluxes;
  fluxes_prod.carbon=(cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon+cell->balance.trad_biofuel.carbon);
  fluxes_prod.nitrogen=(cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen+cell->balance.trad_biofuel.nitrogen);
  fluxes_firewood=cell->balance.flux_firewood;
#endif

  timberharvest=FALSE;
  if(iswetland)
    s=findlandusetype(cell->standlist,WETLAND);
  else
    s=findlandusetype(cell->standlist,NATURAL);
  if(s!=NOT_FOUND)
  {
    stand=getstand(cell->standlist,s);
    if(stand->frac>minnatfrac)
    {
      if(difffrac+epsilon>=stand->frac-minnatfrac)
        difffrac=stand->frac-minnatfrac;
      if(iswetland)
        pos=addstand(&wetland_stand,cell)-1;
      else
        pos=addstand(&natural_stand,cell)-1;
      cutstand=getstand(cell->standlist,pos);
      cutstand->frac=difffrac;
      reclaim_land(stand,cutstand,cell,config->luc_timber,npft+ncft,config);
      /*force one tillage event on new stand upon cultivation after deforestation of natural land */
      tillage(&cutstand->soil, param.residue_frac);
      updatelitterproperties(cutstand,cutstand->frac);
      if(config->soilpar_option==NO_FIXED_SOILPAR || (config->soilpar_option==FIXED_SOILPAR && year<config->soilpar_fixyear))
        pedotransfer(cutstand,NULL,NULL,cutstand->frac);
      if(difffrac+epsilon>=stand->frac)
      {
        difffrac-=stand->frac;
        cutstand->frac=stand->frac;
        delstand(cell->standlist,s);
        if(iswetland)
          pos=findlandusetype(cell->standlist,WETLAND);
        else
          pos=findlandusetype(cell->standlist,NATURAL);
      }
      else
      {
        stand->frac-=difffrac;
        difffrac=0;
      }
      if(!timberharvest)
      {
        /* stand was already tilled, so put FALSE to tillage argument */
        if(setaside(cell,getstand(cell->standlist,pos),FALSE,intercrop,npft,ncft,irrig,iswetland,year,config))
          delstand(cell->standlist,pos);
        check_stand_fracs(cell,cell->lakefrac+cell->ml.reservoirfrac);

      }
    }
    if(iswetland)
      difffrac_wetland=difffrac_old-difffrac;
  }
  if(difffrac>0)
  {
    difffrac_old=difffrac;
    if(iswetland)
      s=findlandusetype(cell->standlist,NATURAL); //now we search if natural is available
    else
      s=findlandusetype(cell->standlist,WETLAND);
    if(s!=NOT_FOUND){
      if(iswetland)
        pos=addstand(&natural_stand,cell)-1;
      else
        pos=addstand(&wetland_stand,cell)-1;
      stand=getstand(cell->standlist,s);
      cutstand=getstand(cell->standlist,pos);
      if(difffrac+epsilon>=stand->frac-minnatfrac)
        difffrac=stand->frac-minnatfrac;
      cutstand->frac=difffrac;

      reclaim_land(stand,cutstand,cell,config->luc_timber,npft+ncft,config);
      if(difffrac+epsilon>=stand->frac)
      {
        difffrac-=stand->frac;
        cutstand->frac=stand->frac;
        delstand(cell->standlist,s);
        if(iswetland)
          pos=findlandusetype(cell->standlist,NATURAL);  // NEEDS TO BE THAT WAY FIRST WE LOOK FOR WETLAND IF WETLAND AND SECOND FOR NATURAL HERE
        else
          pos=findlandusetype(cell->standlist,WETLAND);
      }
      else
      {
        stand->frac-=difffrac;
        difffrac=0;
      }
      if(!timberharvest)
      {
        if(setaside(cell,getstand(cell->standlist,pos),cell->ml.with_tillage,intercrop,npft,ncft,irrig,iswetland,year,config))
          delstand(cell->standlist,pos);
      }
    }
    if(iswetland)
      difffrac_wetland=difffrac_old-difffrac;

  }
  check_stand_fracs(cell,cell->lakefrac+cell->ml.reservoirfrac);
  cell->hydrotopes.wetland_area-=difffrac_wetland;
  cell->hydrotopes.wetland_area_runmean-=difffrac_wetland;

  if(s==NOT_FOUND && difffrac>0.01)
  {
#ifdef CHECK_BALANCE2
    Irrigation *data;
    Pftcrop *crop;
    foreachstand(stand,s,cell->standlist)
    {
      data=stand->data;
      foreachpft(pft,p,&stand->pftlist)
      {
        if(stand->type->landusetype!= NATURAL && stand->type->landusetype!= WETLAND)
        {
            crop=pft->data;
            fprintf(stderr,"type %s frac: %g s: %d iswetland_stand: %d irrig: %d  stand_irrigation: %d carbon:%g methan: %g PFT: %s growingdays: %d\n",
               stand->type->name,stand->frac,s,stand->soil.iswetland,irrig,data->irrigation,standstocks(stand).carbon,
               soilmethane(&stand->soil)*WC/WCH4,pft->par->name,crop->growingdays);
        }
        else
          fprintf(stderr,"type %s frac: %g s: %d iswetland_stand: %d irrig: %d  carbon:%g methan: %g PFT: %s \n",
             stand->type->name,stand->frac,s,stand->soil.iswetland,irrig,standstocks(stand).carbon,
             soilmethane(&stand->soil)*WC/WCH4,pft->par->name);

      }
    }
#endif
   fprintf(stderr,"WARNING: No natural stand or wetland for deforest in (%s), difffrac= %g iswetland: %d  \n",sprintcoord(line,&cell->coord),difffrac, iswetland);
#ifdef CHECK_BALANCE2
    for(j=0;j<ncft;j++)
      fprintf(stderr,"landfrac_rainfed: %g landfrac_irr: %g\n", cell->ml.landfrac[0].crop[j],cell->ml.landfrac[1].crop[j]);
    fprintf(stderr,"grassfrac_rainfed: %g grassfrac_irr: %g\n",(cell->ml.landfrac[0].grass[0]+cell->ml.landfrac[0].grass[1]),
        (cell->ml.landfrac[1].grass[0]+cell->ml.landfrac[1].grass[1]));
    fprintf(stderr,"ml.cropfrac_rf: %g ml.cropfrac_ir: %g  ml.cropfrac_wl: %g  ml.cropfrac_wl_irr: %g \n",
        cell->ml.cropfrac_rf,cell->ml.cropfrac_ir,cell->ml.cropfrac_wl[0],cell->ml.cropfrac_wl[1]);
#endif
  }
#ifdef CHECK_BALANCE
  end.carbon=end.nitrogen = end_w=0;
  foreachstand(checkstand, s, cell->standlist)
  {
    st=standstocks(checkstand);
    end.carbon+=(st.carbon+ soilmethane(&checkstand->soil)*WC/WCH4)*checkstand->frac;
    end.nitrogen+=st.nitrogen*checkstand->frac;
    end_w+= soilwater(&checkstand->soil)*checkstand->frac;
  }
  end.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  end.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;

  balance.carbon=(cell->balance.flux_estab.carbon-fluxes_estab.carbon)-(cell->balance.fire.carbon-fluxes_fire.carbon)-(cell->balance.neg_fluxes.carbon-fluxes_neg.carbon)
      -((cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon+cell->balance.trad_biofuel.carbon)-fluxes_prod.carbon)-(cell->balance.flux_firewood.carbon-fluxes_firewood.carbon);
  balance.nitrogen=(cell->balance.flux_estab.nitrogen-fluxes_estab.nitrogen)-(cell->balance.fire.nitrogen-fluxes_fire.nitrogen)-(cell->balance.neg_fluxes.nitrogen-fluxes_neg.nitrogen)
      -((cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen+cell->balance.trad_biofuel.nitrogen)-fluxes_prod.nitrogen)-(cell->balance.flux_firewood.nitrogen-fluxes_firewood.nitrogen);
  if(fabs(start.carbon-end.carbon+balance.carbon)>0.001)
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid carbon balance in %s at the end: year=%d: C_ERROR=%g start : %g end : %g balance.carbon: %g",
         __FUNCTION__,year,start.carbon-end.carbon+balance.carbon,start.carbon,end.carbon,balance.carbon);
  if (fabs(start_w - end_w)>0.001)
    fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid water balance in %s at the end: year=%d: W_ERROR=%g start : %g end : %g\n",
         __FUNCTION__,year, start_w - end_w, start_w, end_w);
  if (fabs(start.nitrogen-end.nitrogen+balance.nitrogen)>0.001)
    fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid nitrogen balance in %s at the end: year=%d: error=%g start : %g end : %g balance.nitrogen: %g\n",
         __FUNCTION__,year, start.nitrogen-end.nitrogen+balance.nitrogen,start.nitrogen,end.nitrogen,balance.nitrogen);
#endif
} /* of 'deforest' */

#ifdef IMAGE
void deforest_for_timber(Cell *cell,     /**< pointer to cell */
                         Real difffrac,  /**< stand fraction to deforest (0..1) */
                         int npft,       /**< number of natural PFTs */
                         Bool luc_timber,
                         int ncft,       /**< number of crop PFTs */
                         Real minnatfrac,
                         int year,
                         const Config *config /**< LPJmL configuration */
                        )
{
  int s, pos, p;
  Pft *pft;
  Stand *natstand, *cutstand;
  String line;
  s = findlandusetype(cell->standlist, NATURAL);
  if (s != NOT_FOUND)
  {
    natstand = getstand(cell->standlist, s);

    if (natstand->frac>minnatfrac)
    {
      pos = addstand(&natural_stand, cell) - 1;
      cutstand = getstand(cell->standlist, pos);
      cutstand->frac = difffrac;

      reclaim_land(natstand, cutstand, cell, luc_timber, npft + ncft,config);

      /* merge natstand and cutstand following procedures in regrowth */
      if (difffrac + epsilon >= natstand->frac)
      {
        /* whole original natural stand is cut so cutstand becomes the natural stand
        and the original natural stand is deleted  */
        cutstand->type = &natural_stand;
        delstand(cell->standlist, s);
      }
      else
      {
        /* only part of original natural stand is cut so merge natstand and cutstand */
        natstand->frac -= difffrac;
        mixsoil(natstand, cutstand,year,npft+ncft,config);
        foreachpft(pft, p, &natstand->pftlist)
          mix_veg(pft, natstand->frac / (natstand->frac + difffrac));  // PB + difffrac I presume...
        natstand->frac += cutstand->frac;
        delstand(cell->standlist, pos);
        //fprintf(stderr,"deforest_for_timber, part of natstand is cut so natstand and cutstand merged\n");
      }
    }
  }
  else
    fail(NO_NATURAL_STAND_ERR,TRUE,"No natural stand for deforest in cell (%s), difffrac=%g",
         sprintcoord(line,&cell->coord),difffrac);
} /* of 'deforest_for_timber' */
#endif

static void regrowth(Cell *cell, /* pointer to cell */
                     Real difffrac, /* stand fraction to regrowth (-1...0) */
                     int npft, /* number of natural PFTs */
                     Bool irrig,
                     Bool iswetland,
                     int ncft, /* number of crop PFTs */
                     int year, /* simulation year (AD) */
                     const Config *config /* LPJmL configuration */
                    )
{
  int s,pos,p;
  Stocks flux_estab;
  Pft *pft;
  Stand *setasidestand,*natstand,*mixstand;
#ifdef CHECK_BALANCE
//anpp, influx and arh do not change
  Stand *checkstand;
  Stocks start = {0,0};
  Stocks end = {0,0};
  Stocks st;
  Stocks fluxes_fire= {0,0};
  Stocks fluxes_estab= {0,0};
  Stocks fluxes_neg= {0,0};
  Stocks fluxes_prod= {0,0};
  Stocks fluxes_firewood={0,0};
  Stocks balance= {0,0};
  Real start_w = 0;
  Real end_w = 0;
  foreachstand(checkstand, s, cell->standlist)
  {
    st=standstocks(checkstand);
    start.carbon+=(st.carbon+ soilmethane(&checkstand->soil)*WC/WCH4)*checkstand->frac;//-stand->cell->balance.flux_estab.carbon;
    start.nitrogen+=st.nitrogen*checkstand->frac;//-stand->cell->balance.flux_estab.nitrogen;
    start_w += soilwater(&checkstand->soil)*checkstand->frac;
  }
  start.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  start.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;

  fluxes_fire=cell->balance.fire;
  fluxes_estab=cell->balance.flux_estab;
  fluxes_neg=cell->balance.neg_fluxes;
  fluxes_prod.carbon=(cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon+cell->balance.trad_biofuel.carbon);
  fluxes_prod.nitrogen=(cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen+cell->balance.trad_biofuel.nitrogen);
  fluxes_firewood=cell->balance.flux_firewood;
#endif

  if(iswetland)
    s=findlandusetype(cell->standlist,SETASIDE_WETLAND);
  else
    s=findlandusetype(cell->standlist,irrig ? SETASIDE_IR : SETASIDE_RF);

  if(s!=NOT_FOUND)
  {
#ifdef IMAGE
    // guarentee setaside stand at beginning of year
    setasidestand=getstand(cell->standlist,s);

    if (setasidestand->frac>1.6e-7+epsilon)
    {
      difffrac=max(difffrac,-setasidestand->frac+1.6e-7);
      if(iswetland)
        pos=addstand(&setaside_wetland_stand,cell)-1;
      else
        pos=addstand(irrig==TRUE ? &setaside_ir_stand :&setaside_rf_stand,cell)-1; /*setaside big enough for regrowth*/
      mixstand=getstand(cell->standlist,pos);
      mixstand->frac= -difffrac;
      reclaim_land(setasidestand,mixstand,cell,config->luc_timber,npft+ncft,config);
      setasidestand->frac+=difffrac;
#else
    setasidestand=getstand(cell->standlist,s);
    if(setasidestand->frac<=epsilon-difffrac)
    {          /*setaside stand has not enough space for regrowth*/
      mixstand=getstand(cell->standlist,s);
      cutpfts(mixstand,config);
      difffrac=-mixstand->frac;
      pos=s;
    }
    else
    {
      if(iswetland)
        pos=addstand(&setaside_wetland_stand,cell)-1;
      else
        pos=addstand(irrig==TRUE ? &setaside_ir_stand :&setaside_rf_stand,cell)-1; /*setaside big enough for regrowth*/
      mixstand=getstand(cell->standlist,pos);
      mixstand->frac= -difffrac;
      reclaim_land(setasidestand,mixstand,cell,config->luc_timber,npft+ncft,config);
      setasidestand->frac+=difffrac;
    }
#endif

    if(iswetland)
      s=findlandusetype(cell->standlist,WETLAND);
    else
      s=findlandusetype(cell->standlist,NATURAL);
    if(s!=NOT_FOUND)
    {        /*mixing of natural vegetation with regrowth*/
      natstand=getstand(cell->standlist,s);
      mixsoil(natstand,mixstand,year,npft+ncft,config);
      mixstand->slope_mean=(mixstand->slope_mean*mixstand->frac+natstand->slope_mean*natstand->frac)/(mixstand->frac+natstand->frac);
      mixstand->Hag_Beta=min(1,(0.06*log(mixstand->slope_mean+0.1)+0.22)/0.43);
      foreachpft(pft,p,&natstand->pftlist)
        mix_veg(pft,natstand->frac/(natstand->frac-difffrac));
      natstand->frac+=mixstand->frac;
      natstand->soil.iswetland=iswetland;
      delstand(cell->standlist,pos);
    }
    else
    {
      mixstand->type->freestand(mixstand);
      if(iswetland)
        mixstand->type=&wetland_stand;
      else
        mixstand->type=&natural_stand;
      mixstand->type->newstand(mixstand);
      natstand=mixstand;
      natstand->soil.iswetland=iswetland;
    }
    natstand->prescribe_landcover = NO_LANDCOVER;

    flux_estab=establishmentpft(natstand,npft,PREC_MAX,year,config);
    getoutput(&cell->output,FLUX_ESTABC,config)+=flux_estab.carbon*natstand->frac;
    getoutput(&cell->output,FLUX_ESTABN,config)+=flux_estab.nitrogen*natstand->frac;
    cell->balance.flux_estab.carbon+=flux_estab.carbon*natstand->frac;
    cell->balance.flux_estab.nitrogen+=flux_estab.nitrogen*natstand->frac;
  }
#ifdef IMAGE
  }
#endif


#ifdef CHECK_BALANCE
  end.carbon=end.nitrogen = end_w=0;
  foreachstand(checkstand, s, cell->standlist)
  {
    st=standstocks(checkstand);
    end.carbon+=(st.carbon+ soilmethane(&checkstand->soil)*WC/WCH4)*checkstand->frac;
    end.nitrogen+=st.nitrogen*checkstand->frac;
    end_w+= soilwater(&checkstand->soil)*checkstand->frac;
  }
  end.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  end.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;

  balance.carbon=(cell->balance.flux_estab.carbon-fluxes_estab.carbon)-(cell->balance.fire.carbon-fluxes_fire.carbon)-(cell->balance.neg_fluxes.carbon-fluxes_neg.carbon)
      -((cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon+cell->balance.trad_biofuel.carbon)-fluxes_prod.carbon)-(cell->balance.flux_firewood.carbon-fluxes_firewood.carbon);
  balance.nitrogen=(cell->balance.flux_estab.nitrogen-fluxes_estab.nitrogen)-(cell->balance.fire.nitrogen-fluxes_fire.nitrogen)-(cell->balance.neg_fluxes.nitrogen-fluxes_neg.nitrogen)
      -((cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen+cell->balance.trad_biofuel.nitrogen)-fluxes_prod.nitrogen)-(cell->balance.flux_firewood.nitrogen-fluxes_firewood.nitrogen);
  if(fabs(start.carbon-end.carbon+balance.carbon)>0.001)
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid carbon balance in %s at the end: year=%d: C_ERROR=%g start : %g end : %g balance.carbon: %g\n",
         __FUNCTION__,year,start.carbon-end.carbon+balance.carbon,start.carbon,end.carbon,balance.carbon);
  if (fabs(start_w - end_w)>0.001)
    fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid water balance in %s at the end: year=%d: W_ERROR=%g start : %g end : %g\n",
         __FUNCTION__,year, start_w - end_w, start_w, end_w);
  if (fabs(start.nitrogen-end.nitrogen+balance.nitrogen)>0.001)
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid nitrogen balance in %s at the end: year=%d: error=%g start : %g end : %g balance.nitrogen: %g\n",
         __FUNCTION__,year, start.nitrogen-end.nitrogen+balance.nitrogen,start.nitrogen,end.nitrogen,balance.nitrogen);
#endif

}/* of 'regrowth' */

static void landexpansion(Cell *cell,            /* cell pointer */
                          Real difffrac,         /* stand fraction to expand */
                          int npft,              /* number of natural PFTs */
                          Stand *grassstand,     /* grassland stand or NULL */
                          Bool irrigation,       /* irrigated stand (TRUE/FALSE) */
                          Cultivation_type cultivate_type,
                          int pft_id,            /* PFT id of agriculture tree established */
                          int ncft,              /* number of crop PFTs */
                          int year,
                          const Config *config   /* LPJmL configuration */
                         )
{
  int s,p,pos,q,t,ws;
  Stocks flux_estab={0,0};
  int *n_est;
  Pft *pft;
  Irrigation *data;
  Biomass_tree *biomass_tree;
  Stand *setasidestand,*mixstand,*mixstand2,*wetstand,*stand;
  Real frac_change=0;
  Real difffrac2=0;
  Real old_mixfrac=0;
  mixstand=NULL;
  mixstand2=NULL;
#ifdef CHECK_BALANCE
//anpp, influx and arh do not change
  Stand *checkstand;
  Stocks start = {0,0};
  Stocks end = {0,0};
  Stocks st;
  Stocks fluxes_fire= {0,0};
  Stocks fluxes_estab= {0,0};
  Stocks fluxes_neg= {0,0};
  Stocks fluxes_prod= {0,0};
  Stocks fluxes_firewood={0,0};
  Stocks balance= {0,0};
  Real start_w = 0;
  Real end_w = 0;
  foreachstand(checkstand, s, cell->standlist)
  {
    st=standstocks(checkstand);
    start.carbon+=(st.carbon+ soilmethane(&checkstand->soil)*WC/WCH4)*checkstand->frac;//-stand->cell->balance.flux_estab.carbon;
    start.nitrogen+=st.nitrogen*checkstand->frac;//-stand->cell->balance.flux_estab.nitrogen;
    start_w += soilwater(&checkstand->soil)*checkstand->frac;
  }
  start.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  start.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;

  fluxes_fire=cell->balance.fire;
  fluxes_estab=cell->balance.flux_estab;
  fluxes_neg=cell->balance.neg_fluxes;
  fluxes_prod.carbon=(cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon+cell->balance.trad_biofuel.carbon);
  fluxes_prod.nitrogen=(cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen+cell->balance.trad_biofuel.nitrogen);
  fluxes_firewood=cell->balance.flux_firewood;
#endif

  s=findlandusetype(cell->standlist,irrigation ? SETASIDE_IR : SETASIDE_RF);
  ws=findlandusetype(cell->standlist,SETASIDE_WETLAND);

  if(s!=NOT_FOUND || ws!=NOT_FOUND)
  {
    if(s!=NOT_FOUND)
    {
#ifdef IMAGE
      // guarentee setaside stand at beginning of year
      setasidestand=getstand(cell->standlist,s);
      if(setasidestand->frac>1.6e-7+epsilon)
      {
        difffrac=max(difffrac,-setasidestand->frac+1.6e-7);
        difffrac2=max(difffrac,-setasidestand->frac+1.6e-7);
        pos=addstand(&natural_stand,cell)-1;
        mixstand=getstand(cell->standlist,pos);
        mixstand->frac= -difffrac;
        mixstand->typee=&kill_stand;
        reclaim_land(setasidestand,mixstand,cell,config->luc_timber,npft+ncft,config);
        setasidestand->frac+=difffrac;
#else
      setasidestand=getstand(cell->standlist,s);
      if(setasidestand->frac<=epsilon-difffrac)
      {          /*setaside stand has not enough space for grassland expansion*/
        mixstand=getstand(cell->standlist,s);
        cutpfts(mixstand,config);
        mixstand->type=&kill_stand;
        difffrac-=-mixstand->frac;
        difffrac2=-mixstand->frac;
        frac_change=-mixstand->frac;
        pos=s;
      }
      else
      {
        pos=addstand(&natural_stand,cell)-1; /*setaside big enough for grassland expansion*/
        mixstand=getstand(cell->standlist,pos);
        mixstand->type=&kill_stand;
        mixstand->frac=-difffrac;
        difffrac2=-mixstand->frac;
        reclaim_land(setasidestand,mixstand,cell,config->luc_timber,npft+ncft,config);
        setasidestand->frac+=difffrac;
        difffrac=0;
      }
#endif
    }
    if(difffrac<-epsilon)
    {
      s=findlandusetype(cell->standlist,SETASIDE_WETLAND);
      if(s!=NOT_FOUND)
      {
        wetstand=getstand(cell->standlist,s);
        if(wetstand->frac<=epsilon-difffrac)
        {                                                     /*setaside stand has not enough space for grassland expansion*/
          if(mixstand==NULL || mixstand->type->landusetype==KILL)
          {
            if(mixstand==NULL)
            {
              mixstand=getstand(cell->standlist,s);
              mixstand->type=&kill_stand;
              mixstand->soil.iswetland=FALSE;
              cutpfts(mixstand,config);
              difffrac-=-mixstand->frac;
              difffrac2=-mixstand->frac;
              cell->hydrotopes.wetland_area-=mixstand->frac;
              cell->hydrotopes.wetland_area_runmean-=mixstand->frac;
            }
            else
            {
              mixstand2=getstand(cell->standlist,s);
              mixstand2->type=&kill_stand;
              mixstand2->soil.iswetland=FALSE;
              cutpfts(mixstand2,config);
              cell->hydrotopes.wetland_area-=mixstand2->frac;
              cell->hydrotopes.wetland_area_runmean-=mixstand2->frac;
            }
            pos=s;
          }
          else
          {
            old_mixfrac=mixstand->frac;
            mixsoil(mixstand,wetstand,year,npft+ncft,config);
            mixstand->slope_mean=(mixstand->slope_mean*mixstand->frac+wetstand->slope_mean*wetstand->frac)/(mixstand->frac+wetstand->frac);
            mixstand->Hag_Beta=min(1,(0.06*log(mixstand->slope_mean+0.1)+0.22)/0.43);
            foreachpft(pft,p,&mixstand->pftlist)
                mix_veg(pft,mixstand->frac/(mixstand->frac+wetstand->frac));
            difffrac=-wetstand->frac;
            mixstand->frac+=-difffrac;
            difffrac2-=wetstand->frac;
            mixstand->soil.iswetland=FALSE;
            delstand(cell->standlist,s);
            frac_change+=-(mixstand->frac-old_mixfrac);
            cell->hydrotopes.wetland_area-=(mixstand->frac-old_mixfrac);
            cell->hydrotopes.wetland_area_runmean-=(mixstand->frac-old_mixfrac);
          }
        }
        else
        {
          if(mixstand==NULL || mixstand->type->landusetype==KILL)
          {
            pos=addstand(&wetland_stand,cell)-1; /*setaside big enough for grassland expansion*/
            if(mixstand==NULL)
            {
              mixstand=getstand(cell->standlist,pos);
              mixstand->type=&kill_stand;
              mixstand->soil.iswetland=FALSE;
              mixstand->frac=-difffrac;
              reclaim_land(wetstand,mixstand,cell,config->luc_timber,npft+ncft,config);
              wetstand->frac+=difffrac;
              cell->hydrotopes.wetland_area-=-difffrac;
              cell->hydrotopes.wetland_area_runmean-=-difffrac;
              difffrac2+=difffrac;
              difffrac=0;
            }
            else
            {
              mixstand2=getstand(cell->standlist,pos);
              mixstand2->type=&kill_stand;
              mixstand2->soil.iswetland=FALSE;
              mixstand2->frac=-difffrac;
              reclaim_land(wetstand,mixstand2,cell,config->luc_timber,npft+ncft,config);
              wetstand->frac+=difffrac;
            }
          }
          else
          {
            old_mixfrac=wetstand->frac;
            wetstand->frac=-difffrac;
            mixsoil(mixstand,wetstand,year,npft+ncft,config);
            remove_vegetation_copy(&mixstand->soil,wetstand,cell,-difffrac/(mixstand->frac-difffrac),
                config->luc_timber,TRUE,config);
            wetstand->frac=old_mixfrac+difffrac;
            mixstand->frac-=difffrac;
            mixstand->soil.iswetland=FALSE;
            cell->hydrotopes.wetland_area-=-difffrac;
            cell->hydrotopes.wetland_area_runmean-=-difffrac;
            difffrac2+=difffrac;
            difffrac=0;
         }
       }
      }
    }
    if(grassstand!=NULL)
    {
      if(mixstand2!=NULL)
      {
        mixsoil(mixstand,mixstand2,year,npft+ncft,config);
        mixstand->frac+=mixstand2->frac;
        difffrac2+=-mixstand2->frac;
        difffrac=0;
        delstand(cell->standlist,pos);
      }
      mixsoil(grassstand,mixstand,year,npft+ncft,config);
      mixstand->slope_mean=(mixstand->slope_mean*mixstand->frac+grassstand->slope_mean*grassstand->frac)/(mixstand->frac+grassstand->frac);
      mixstand->Hag_Beta=min(1,(0.06*log(mixstand->slope_mean+0.1)+0.22)/0.43);
      foreachpft(pft,p,&grassstand->pftlist)
        mix_veg(pft,grassstand->frac/(grassstand->frac-difffrac2));
      data=grassstand->data;
      data->irrig_stor*=grassstand->frac/(grassstand->frac-difffrac2);
      data->irrig_amount*=grassstand->frac/(grassstand->frac-difffrac2);
      grassstand->frac+=mixstand->frac;
      foreachstand(stand,s,cell->standlist)
        if(stand->type->landusetype==KILL)
        {
          delstand(cell->standlist,s); /* deleting temporary mixstand copy */
          s--;
        }
    }
    else
    {
      n_est=newvec(int,config->ntypes);
      check(n_est);
      for(t=0;t<config->ntypes;t++)
        n_est[t]=0;
      switch(cultivate_type)
      {
        case PASTURE:
          for(p=0;p<npft;p++)
            if(establish(cell->gdd[p],config->pftpar+p,&cell->climbuf,mixstand->type->landusetype==WETLAND || mixstand->type->landusetype==SETASIDE_WETLAND) &&
              config->pftpar[p].type==GRASS && config->pftpar[p].cultivation_type==NONE)
            {
              addpft(mixstand,config->pftpar+p,year,0,config);
              n_est[config->pftpar[p].type]++;
            }
          mixstand->type->freestand(mixstand);
          mixstand->type=&grassland_stand;
          mixstand->type->newstand(mixstand);
          break;
       case OTHER_PASTURE:
          if(!config->others_to_crop)
          {
            for(p=0;p<npft;p++)
              if(establish(cell->gdd[p],config->pftpar+p,&cell->climbuf,mixstand->type->landusetype==WETLAND || mixstand->type->landusetype==SETASIDE_WETLAND) &&
                config->pftpar[p].type==GRASS && config->pftpar[p].cultivation_type==NONE)
              {
                addpft(mixstand,config->pftpar+p,year,0,config);
                n_est[config->pftpar[p].type]++;
              }
            mixstand->type->freestand(mixstand);
            mixstand->type=&others_stand;
            mixstand->type->newstand(mixstand);
          }
          break;
        case BIOMASS_TREE_PLANTATION:
          for(p=0;p<npft;p++)
            if(establish(cell->gdd[p],config->pftpar+p,&cell->climbuf,mixstand->type->landusetype==WETLAND || mixstand->type->landusetype==SETASIDE_WETLAND) &&
              config->pftpar[p].type==TREE && config->pftpar[p].cultivation_type==BIOMASS)
            {
              addpft(mixstand,config->pftpar+p,year,0,config);
              n_est[config->pftpar[p].type]++;
            }
          mixstand->type->freestand(mixstand);
          mixstand->type=&biomass_tree_stand;
          mixstand->type->newstand(mixstand);
          break;
        case AGRICULTURE_TREE_PLANTATION:
          //printf("establish=%s\n",pftpar[pft_id].name);
          if(strcmp(config->pftpar[pft_id].name,"cotton"))
          {
            for(p=0;p<npft;p++)
              if(establish(cell->gdd[p],config->pftpar+p,&cell->climbuf,mixstand->type->landusetype==WETLAND || mixstand->type->landusetype==SETASIDE_WETLAND) &&
                 config->pftpar[p].id==pft_id)
              {
                //printf("is establish=%s\n",pftpar[pft_id].name);
                addpft(mixstand,config->pftpar+p,year,0,config);
                n_est[config->pftpar[p].type]++;
              }
          }
          mixstand->type->freestand(mixstand);
          mixstand->type=(config->pftpar[pft_id].type==GRASS) ? &agriculture_grass_stand : &agriculture_tree_stand;
          mixstand->type->newstand(mixstand);
          biomass_tree=mixstand->data;
          biomass_tree->irrigation.pft_id=pft_id;
          break;
        case BIOMASS_GRASS_PLANTATION:
          for(p=0;p<npft;p++)
            if(establish(cell->gdd[p],config->pftpar+p,&cell->climbuf,mixstand->type->landusetype==WETLAND || mixstand->type->landusetype==SETASIDE_WETLAND) &&
              config->pftpar[p].type==GRASS && config->pftpar[p].cultivation_type==BIOMASS)
            {
              addpft(mixstand,config->pftpar+p,year,0,config);
              n_est[config->pftpar[p].type]++;
            }
          mixstand->type->freestand(mixstand);
          mixstand->type=&biomass_grass_stand;
          mixstand->type->newstand(mixstand);
          break;
        case WOOD_PLANTATION:
          for (p = 0;p < npft;p++)
            if (establish(cell->gdd[p], config->pftpar + p, &cell->climbuf,mixstand->type->landusetype==WETLAND || mixstand->type->landusetype==SETASIDE_WETLAND) &&
              config->pftpar[p].type == TREE && config->pftpar[p].cultivation_type == WP)
            {
              addpft(mixstand, config->pftpar + p, year, 0,config);
              n_est[config->pftpar[p].type]++;
            }
          mixstand->type->freestand(mixstand);
          mixstand->type = &woodplantation_stand;
          mixstand->type->newstand(mixstand);
        break;
          default:
            fail(WRONG_CULTIVATION_TYPE_ERR,TRUE,TRUE,
                 "WRONG CULTIVATION TYPE in landexpansion()");
            break;
      } /* of switch */
      data=mixstand->data;
      data->irrigation=irrigation;
      mixstand->frac_change=-frac_change;
      foreachpft(pft,q,&mixstand->pftlist)
      {
        flux_estab=establishment(pft,0,0,n_est[pft->par->type]);
        if (pft->par->cultivation_type == BIOMASS || pft->par->cultivation_type == WP)
        {
          cell->balance.estab_storage_tree[data->irrigation].carbon-=flux_estab.carbon*mixstand->frac;
          cell->balance.estab_storage_tree[data->irrigation].nitrogen-=flux_estab.nitrogen*mixstand->frac;
          flux_estab.carbon=flux_estab.nitrogen=0;
        }
        getoutput(&cell->output,FLUX_ESTABC,config)+=flux_estab.carbon*mixstand->frac;
        getoutput(&cell->output,FLUX_ESTABN,config)+=flux_estab.nitrogen*mixstand->frac;
        cell->balance.flux_estab.carbon+=flux_estab.carbon*mixstand->frac;
        cell->balance.flux_estab.nitrogen+=flux_estab.nitrogen*mixstand->frac;
      } /* of foreachpft */
      free(n_est);
    } /* if grassstand */
#ifdef IMAGE
    } /* if setasidestand */
#endif
  } /* if s or ws*/

#ifdef CHECK_BALANCE
  end.carbon=end.nitrogen = end_w=0;
  foreachstand(checkstand, s, cell->standlist)
  {
    st=standstocks(checkstand);
    end.carbon+=(st.carbon+ soilmethane(&checkstand->soil)*WC/WCH4)*checkstand->frac;
    end.nitrogen+=st.nitrogen*checkstand->frac;
    end_w+= soilwater(&checkstand->soil)*checkstand->frac;
  }
  end.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  end.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;

  balance.carbon=(cell->balance.flux_estab.carbon-fluxes_estab.carbon)-(cell->balance.fire.carbon-fluxes_fire.carbon)-(cell->balance.neg_fluxes.carbon-fluxes_neg.carbon)
      -((cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon+cell->balance.trad_biofuel.carbon)-fluxes_prod.carbon)-(cell->balance.flux_firewood.carbon-fluxes_firewood.carbon);
  balance.nitrogen=(cell->balance.flux_estab.nitrogen-fluxes_estab.nitrogen)-(cell->balance.fire.nitrogen-fluxes_fire.nitrogen)-(cell->balance.neg_fluxes.nitrogen-fluxes_neg.nitrogen)
      -((cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen+cell->balance.trad_biofuel.nitrogen)-fluxes_prod.nitrogen)-(cell->balance.flux_firewood.nitrogen-fluxes_firewood.nitrogen);
  if(fabs(start.carbon-end.carbon+balance.carbon)>0.001)
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid carbon balance in %s at the end: year=%d: C_ERROR=%g start : %g end : %g balance.carbon: %g",
         __FUNCTION__,year,start.carbon-end.carbon+balance.carbon,start.carbon,end.carbon,balance.carbon);
  if (fabs(start_w - end_w)>0.001)
    fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid water balance in %s at the end: year=%d: W_ERROR=%g start : %g end : %g",
         __FUNCTION__,year, start_w - end_w, start_w, end_w);
  if (fabs(start.nitrogen-end.nitrogen+balance.nitrogen)>0.001)
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid nitrogen balance in %s at the end: year=%d: error=%g start : %g end : %g balance.nitrogen: %g",
         __FUNCTION__,year, start.nitrogen-end.nitrogen+balance.nitrogen,start.nitrogen,end.nitrogen,balance.nitrogen);
#endif

} /* of 'landexpansion' */

static void grasslandreduction(Cell *cell,            /* cell pointer */
                               Real difffrac,         /* stand fraction to reduce (0..1) */
                               Bool intercrop,        /* intercropping possible (TRUE/FALSE) */
                               int npft,              /* number of natural PFTs */
                               int s,                 /* index in stand list */
                               Stand *grassstand,     /* pointer to grassland stand */
                               int ncft,              /* number of crop PFTs */
                               int year,
                               const Config *config   /* LPJmL configuration */
                              )
{
  int pos,index;
  Stand *cutstand;
  Irrigation *data;
  Output *output;

  data=grassstand->data;
  output=&grassstand->cell->output;
#ifdef CHECK_BALANCE
//anpp, influx and arh do not change
  Stand *checkstand;
  Stocks start = {0,0};
  Stocks end = {0,0};
  Stocks st;
  Stocks fluxes_fire= {0,0};
  Stocks fluxes_estab= {0,0};
  Stocks fluxes_neg= {0,0};
  Stocks fluxes_prod= {0,0};
  Stocks fluxes_firewood={0,0};
  Stocks balance= {0,0};
  Real start_w = 0;
  Real end_w = 0;
  foreachstand(checkstand, s, cell->standlist)
  {
    st=standstocks(checkstand);
    start.carbon+=(st.carbon+ soilmethane(&checkstand->soil)*WC/WCH4)*checkstand->frac;//-stand->cell->balance.flux_estab.carbon;
    start.nitrogen+=st.nitrogen*checkstand->frac;//-stand->cell->balance.flux_estab.nitrogen;
    start_w += soilwater(&checkstand->soil)*checkstand->frac;
  }
  start.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  start.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;

  fluxes_fire=cell->balance.fire;
  fluxes_estab=cell->balance.flux_estab;
  fluxes_neg=cell->balance.neg_fluxes;
  fluxes_prod.carbon=(cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon+cell->balance.trad_biofuel.carbon);
  fluxes_prod.nitrogen=(cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen+cell->balance.trad_biofuel.nitrogen);
  fluxes_firewood=cell->balance.flux_firewood;
#endif

  index=data->irrigation*getnirrig(ncft,config)+(grassstand->type->landusetype==GRASSLAND ? rmgrass(ncft) : rothers(ncft));

  if(grassstand->frac<=difffrac+epsilon)
  {
    /* empty irrig stor and pay back conveyance losses that have been consumed by transport into irrig_stor, only evaporative conv. losses, drainage conv. losses already returned */
    cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*grassstand->cell->coord.area*grassstand->frac;
    cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*grassstand->frac;
    getoutput(output,STOR_RETURN,config)+=(data->irrig_stor+data->irrig_amount)*grassstand->frac;
    grassstand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*grassstand->cell->coord.area*grassstand->frac;
    cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*grassstand->frac;
    getoutput(output,CONV_LOSS_EVAP,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*grassstand->frac;
    cell->balance.aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*grassstand->frac;
    getoutput(output,CONV_LOSS_DRAIN,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*grassstand->frac;
    cell->balance.aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*grassstand->frac;
#if defined IMAGE && defined COUPLED
    if(cell->ml.image_data!=NULL)
    {
      cell->ml.image_data->mirrwatdem[0]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*grassstand->frac;
      cell->ml.image_data->mevapotr[0]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*grassstand->frac;
    }
#endif
    if(config->pft_output_scaled)
    {
      getoutputindex(output,CFT_CONV_LOSS_EVAP,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*grassstand->frac;
      getoutputindex(output,CFT_CONV_LOSS_DRAIN,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*grassstand->frac;
    }
    else
    {
      getoutputindex(output,CFT_CONV_LOSS_EVAP,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
      getoutputindex(output,CFT_CONV_LOSS_DRAIN,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
    }

    data->irrig_stor=0;
    data->irrig_amount=0;

    cutpfts(grassstand,config);
    /*force one tillage event on new stand upon cultivation of previous grassland,  */

    if(grassstand->soil.iswetland)
    {
      if(setaside(cell,getstand(cell->standlist,s),TRUE,intercrop,npft,ncft,data->irrigation,TRUE,max(config->till_startyear,year),config))
        delstand(cell->standlist,s);
    }
    else
    {
      if(setaside(cell,getstand(cell->standlist,s),TRUE,intercrop,npft,ncft,data->irrigation,FALSE,max(config->till_startyear,year),config))
        delstand(cell->standlist,s);
    }
  }
  else
  {
    pos=addstand(&natural_stand,cell)-1;
    cutstand=getstand(cell->standlist,pos);
    cutstand->frac=difffrac;
    reclaim_land(grassstand,cutstand,cell,config->luc_timber,npft+ncft,config);
    grassstand->frac-=difffrac;
    /*force one tillage event on new stand upon cultivation of previous grassland */
    tillage(&cutstand->soil, param.residue_frac);
    updatelitterproperties(cutstand,cutstand->frac);
    if(config->soilpar_option==NO_FIXED_SOILPAR || (config->soilpar_option==FIXED_SOILPAR && year<config->soilpar_fixyear))
      pedotransfer(cutstand,NULL,NULL,cutstand->frac);
    /* empty irrig stor and pay back conveyance losses that have been consumed by transport into irrig_stor, only evaporative conv. losses, drainage conv. losses already returned */
    cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*grassstand->cell->coord.area*difffrac;
    cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*difffrac;
    getoutput(output,STOR_RETURN,config)+=(data->irrig_stor+data->irrig_amount)*difffrac;
    cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*grassstand->cell->coord.area*difffrac;
    cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*difffrac;
    getoutput(output,CONV_LOSS_EVAP,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*difffrac;
    cell->balance.aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*difffrac;
    getoutput(output,CONV_LOSS_DRAIN,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*difffrac;
    cell->balance.aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*difffrac;
#if defined IMAGE && defined COUPLED
    if(cell->ml.image_data!=NULL)
    {
      cell->ml.image_data->mirrwatdem[0]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*difffrac;
      cell->ml.image_data->mevapotr[0]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*difffrac;
    }
#endif

    if(config->pft_output_scaled)
    {
      getoutputindex(output,CFT_CONV_LOSS_EVAP,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*difffrac;
      getoutputindex(output,CFT_CONV_LOSS_DRAIN,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*difffrac;
    }
    else
    {
      getoutputindex(output,CFT_CONV_LOSS_EVAP,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
      getoutputindex(output,CFT_CONV_LOSS_DRAIN,index,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
    }
    if(grassstand->soil.iswetland)
    {
      if(setaside(cell,getstand(cell->standlist,pos),cell->ml.with_tillage,intercrop,npft,ncft,data->irrigation,TRUE,year,config))
        delstand(cell->standlist,pos);
    }
    else
    {
      if(setaside(cell,getstand(cell->standlist,pos),cell->ml.with_tillage,intercrop,npft,ncft,data->irrigation,FALSE,year,config))
        delstand(cell->standlist,pos);
    }
  }
#ifdef CHECK_BALANCE
  end.carbon=end.nitrogen = end_w=0;
  foreachstand(checkstand, s, cell->standlist)
  {
    st=standstocks(checkstand);
    end.carbon+=(st.carbon+ soilmethane(&checkstand->soil)*WC/WCH4)*checkstand->frac;
    end.nitrogen+=st.nitrogen*checkstand->frac;
    end_w+= soilwater(&checkstand->soil)*checkstand->frac;
  }
  end.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  end.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;

  balance.carbon=(cell->balance.flux_estab.carbon-fluxes_estab.carbon)-(cell->balance.fire.carbon-fluxes_fire.carbon)-(cell->balance.neg_fluxes.carbon-fluxes_neg.carbon)
      -((cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon+cell->balance.trad_biofuel.carbon)-fluxes_prod.carbon)-(cell->balance.flux_firewood.carbon-fluxes_firewood.carbon);
  balance.nitrogen=(cell->balance.flux_estab.nitrogen-fluxes_estab.nitrogen)-(cell->balance.fire.nitrogen-fluxes_fire.nitrogen)-(cell->balance.neg_fluxes.nitrogen-fluxes_neg.nitrogen)
      -((cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen+cell->balance.trad_biofuel.nitrogen)-fluxes_prod.nitrogen)-(cell->balance.flux_firewood.nitrogen-fluxes_firewood.nitrogen);
  if(fabs(start.carbon-end.carbon+balance.carbon)>0.001)
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid carbon balance in %s at the end: year=%d: C_ERROR=%g start : %g end : %g balance.carbon: %g",
         __FUNCTION__,year,start.carbon-end.carbon+balance.carbon,start.carbon,end.carbon,balance.carbon);
  if (fabs(start_w - end_w)>0.001)
    fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid water balance in %s at the end: year=%d: W_ERROR=%g start : %g end : %g",
         __FUNCTION__,year, start_w - end_w, start_w, end_w);
  if (fabs(start.nitrogen-end.nitrogen+balance.nitrogen)>0.001)
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid nitrogen balance in %s at the end: year=%d: error=%g start : %g end : %g balance.nitrogen: %g",
         __FUNCTION__,year, start.nitrogen-end.nitrogen+balance.nitrogen,start.nitrogen,end.nitrogen,balance.nitrogen);
#endif

} /* of 'grasslandreduction */

void set_irrigsystem(Stand *stand,          /**< stand pointer */
                     int cft,               /**< CFT index (0..ncft-1) */
                     int npft,              /**< number of natural PFTs */
                     int ncft,              /**< number of crop PFTs */
                     const Config *config   /**< LPJmL configuration */
                    )
{
  int nirrig;
  Irrigation *data;
  Output *output;

  data=stand->data;
  output=&stand->cell->output;

  nirrig=getnirrig(ncft,config);

  switch(stand->type->landusetype)
  {
    case AGRICULTURE:
      if(data->irrigation && data->irrig_system!=stand->cell->ml.irrig_system->crop[cft])
      {
        data->irrig_system=stand->cell->ml.irrig_system->crop[cft]; /* irrig_stor was emptied at harvest */
      }
      if(!data->irrigation)
        data->irrig_system=NOIRRIG;
      break;
    case GRASSLAND:
      if(data->irrigation)
        if(data->irrig_system!=stand->cell->ml.irrig_system->grass[1])
        {
          /* empty irrig_stor and pay back conveyance losses before changing irrigation system */
          stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*stand->cell->coord.area*stand->frac;
          stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*stand->frac;
          getoutput(output,STOR_RETURN,config)+=(data->irrig_stor+data->irrig_amount)*stand->frac;
          stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->coord.area*stand->frac;
          stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
          getoutput(output,CONV_LOSS_EVAP,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
          stand->cell->balance.aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
          getoutput(output,CONV_LOSS_DRAIN,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
          stand->cell->balance.aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
#if defined IMAGE && defined COUPLED
          if(stand->cell->ml.image_data!=NULL)
          {
            stand->cell->ml.image_data->mirrwatdem[0]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
            stand->cell->ml.image_data->mevapotr[0]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
          }
#endif

          if(config->pft_output_scaled)
          {
            getoutputindex(output,CFT_CONV_LOSS_EVAP,rmgrass(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
            getoutputindex(output,CFT_CONV_LOSS_DRAIN,rmgrass(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
          }
          else
          {
            getoutputindex(output,CFT_CONV_LOSS_EVAP,rmgrass(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
            getoutputindex(output,CFT_CONV_LOSS_DRAIN,rmgrass(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
          }

          data->irrig_stor=0;
          data->irrig_amount=0;
          /* change irrig_system */
          data->irrig_system=stand->cell->ml.irrig_system->grass[1];
        }
      if(!data->irrigation)
        data->irrig_system=NOIRRIG;
      break;
    case OTHERS:
      if(data->irrigation)
        if(data->irrig_system!=stand->cell->ml.irrig_system->grass[0])
        {
          if(!config->others_to_crop)
          {
            /* empty irrig_stor and pay back conveyance losses before changing irrigation system */
            stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*stand->cell->coord.area*stand->frac;
            stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*stand->frac;
            getoutput(output,STOR_RETURN,config)+=(data->irrig_stor+data->irrig_amount)*stand->frac;
            stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->coord.area*stand->frac;
            stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
            getoutput(output,CONV_LOSS_EVAP,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
            stand->cell->balance.aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
            getoutput(output,CONV_LOSS_DRAIN,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
            stand->cell->balance.aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
#if defined IMAGE && defined COUPLED
            if(stand->cell->ml.image_data!=NULL)
            {
              stand->cell->ml.image_data->mirrwatdem[0]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
              stand->cell->ml.image_data->mevapotr[0]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
            }
#endif

            if(config->pft_output_scaled)
            {
              getoutputindex(output,CFT_CONV_LOSS_EVAP,rothers(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
              getoutputindex(output,CFT_CONV_LOSS_DRAIN,rothers(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
            }
            else
            {
              getoutputindex(output,CFT_CONV_LOSS_EVAP,rothers(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
              getoutputindex(output,CFT_CONV_LOSS_DRAIN,rothers(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
            }

            data->irrig_stor=0;
            data->irrig_amount=0;
          }
          /* change irrig_system */
          data->irrig_system=stand->cell->ml.irrig_system->grass[0];
        }
      if(!data->irrigation)
        data->irrig_system=NOIRRIG;
      break;
    case AGRICULTURE_TREE: case AGRICULTURE_GRASS:
      if(data->irrigation && data->irrig_system!=stand->cell->ml.irrig_system->ag_tree[data->pft_id-npft+config->nagtree])
      {
        /* empty irrig_stor and pay back conveyance losses before changing irrigation system */
        stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*stand->cell->coord.area*stand->frac;
        stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*stand->frac;
        getoutput(output,STOR_RETURN,config)+=(data->irrig_stor+data->irrig_amount)*stand->frac;
        stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->coord.area*stand->frac;
        stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        getoutput(output,CONV_LOSS_EVAP,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        stand->cell->balance.aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        getoutput(output,CONV_LOSS_DRAIN,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
        stand->cell->balance.aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
#if defined IMAGE && defined COUPLED
        if(stand->cell->ml.image_data!=NULL)
        {
          stand->cell->ml.image_data->mirrwatdem[0]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
          stand->cell->ml.image_data->mevapotr[0]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
        }
#endif
        if(config->pft_output_scaled)
        {
          getoutputindex(output,CFT_CONV_LOSS_EVAP,agtree(ncft,config->nwptype)+data->pft_id-npft+config->nagtree+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
          getoutputindex(output,CFT_CONV_LOSS_DRAIN,agtree(ncft,config->nwptype)+data->pft_id-npft+config->nagtree+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
        }
        else
        {
          getoutputindex(output,CFT_CONV_LOSS_EVAP,agtree(ncft,config->nwptype)+data->pft_id-npft+config->nagtree+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
          getoutputindex(output,CFT_CONV_LOSS_DRAIN,agtree(ncft,config->nwptype)+data->pft_id-npft+config->nagtree+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
        }

        data->irrig_stor=0;
        data->irrig_amount=0;
        /* change irrig_system */
        data->irrig_system=stand->cell->ml.irrig_system->ag_tree[data->pft_id-npft+config->nagtree];
      }
      if(!data->irrigation)
        data->irrig_system=NOIRRIG;
      break;
    case BIOMASS_TREE:
      if(data->irrigation && data->irrig_system!=stand->cell->ml.irrig_system->biomass_tree)
      {
        /* empty irrig_stor and pay back conveyance losses before changing irrigation system */
        stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*stand->cell->coord.area*stand->frac;
        stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*stand->frac;
        getoutput(output,STOR_RETURN,config)+=(data->irrig_stor+data->irrig_amount)*stand->frac;
        stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->coord.area*stand->frac;
        stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        getoutput(output,CONV_LOSS_EVAP,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        stand->cell->balance.aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        getoutput(output,CONV_LOSS_DRAIN,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
        stand->cell->balance.aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
#if defined IMAGE && defined COUPLED
        if(stand->cell->ml.image_data!=NULL)
        {
          stand->cell->ml.image_data->mirrwatdem[0]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
          stand->cell->ml.image_data->mevapotr[0]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
        }
#endif
        if(config->pft_output_scaled)
        {
          getoutputindex(output,CFT_CONV_LOSS_EVAP,rbtree(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
          getoutputindex(output,CFT_CONV_LOSS_DRAIN,rbtree(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
        }
        else
        {
          getoutputindex(output,CFT_CONV_LOSS_EVAP,rbtree(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
          getoutputindex(output,CFT_CONV_LOSS_DRAIN,rbtree(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
        }

        data->irrig_stor=0;
        data->irrig_amount=0;
        /* change irrig_system */
        data->irrig_system=stand->cell->ml.irrig_system->biomass_tree;
      }
      if(!data->irrigation)
        data->irrig_system=NOIRRIG;
      break;
    case BIOMASS_GRASS:
      if(data->irrigation && data->irrig_system!=stand->cell->ml.irrig_system->biomass_grass)
      {
        /* empty irrig_stor and pay back conveyance losses before changing irrigation system */
        stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*stand->cell->coord.area*stand->frac;
        stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*stand->frac;
        getoutput(output,STOR_RETURN,config)+=(data->irrig_stor+data->irrig_amount)*stand->frac;
        stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->coord.area*stand->frac;
        stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        getoutput(output,CONV_LOSS_EVAP,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        stand->cell->balance.aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        getoutput(output,CONV_LOSS_DRAIN,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
        stand->cell->balance.aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
#if defined IMAGE && defined COUPLED
        if(stand->cell->ml.image_data!=NULL)
        {
          stand->cell->ml.image_data->mirrwatdem[0]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
          stand->cell->ml.image_data->mevapotr[0]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
        }
#endif
        if(config->pft_output_scaled)
        {
          getoutputindex(output,CFT_CONV_LOSS_EVAP,rbgrass(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
          getoutputindex(output,CFT_CONV_LOSS_DRAIN,rbgrass(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
        }
        else
        {
          getoutputindex(output,CFT_CONV_LOSS_DRAIN,rbgrass(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
          getoutputindex(output,CFT_CONV_LOSS_DRAIN,rbgrass(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
        }

        data->irrig_stor=0;
        data->irrig_amount=0;
        /* change irrig_system */
        data->irrig_system=stand->cell->ml.irrig_system->biomass_grass;
      }
      if(!data->irrigation)
        data->irrig_system=NOIRRIG;
      break;
    case WOODPLANTATION:
      if (data->irrigation && data->irrig_system != stand->cell->ml.irrig_system->woodplantation)
      {
        /* empty irrig_stor and pay back conveyance losses before changing irrigation system */
        stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*stand->cell->coord.area*stand->frac;
        stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*stand->frac;
        getoutput(output,STOR_RETURN,config)+=(data->irrig_stor+data->irrig_amount)*stand->frac;
        stand->cell->discharge.dmass_lake+=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->cell->coord.area*stand->frac;
        stand->cell->balance.awater_flux-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        getoutput(output,CONV_LOSS_EVAP,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        getoutput(output,CONV_LOSS_DRAIN,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
        stand->cell->balance.aconv_loss_evap-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
        stand->cell->balance.aconv_loss_drain-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
#if defined IMAGE && defined COUPLED
        if(stand->cell->ml.image_data!=NULL)
        {
          stand->cell->ml.image_data->mirrwatdem[0]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
          stand->cell->ml.image_data->mevapotr[0]-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*stand->frac;
        }
#endif
        if (config->pft_output_scaled)
        {
          getoutputindex(output,CFT_CONV_LOSS_EVAP,rwp(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap*stand->frac;
          getoutputindex(output,CFT_CONV_LOSS_DRAIN,rwp(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap)*stand->frac;
        }
        else
        {
          getoutputindex(output,CFT_CONV_LOSS_EVAP,rwp(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*data->conv_evap;
          getoutputindex(output,CFT_CONV_LOSS_DRAIN,rwp(ncft)+nirrig,config)-=(data->irrig_stor+data->irrig_amount)*(1/data->ec-1)*(1-data->conv_evap);
        }

        data->irrig_stor = 0;
        data->irrig_amount = 0;
        /* change irrig_system */
        data->irrig_system = stand->cell->ml.irrig_system->woodplantation;
      }
      if (!data->irrigation)
        data->irrig_system = NOIRRIG;
      break;
    default:
      fail(WRONG_CULTIVATION_TYPE_ERR,TRUE,TRUE,
           "WRONG CULTIVATION TYPE in set_irrigsystem()");
      break;
  } /* of switch */

  /* ec_canal_type=(stand->soil.par->Ks>20) ? 0 : (stand->soil.par->Ks>=10 && stand->soil.par->Ks<=20) ? 1 : 2; */
  /* link ec to soil type: sand 0.7, loam 0.75, clay 0.85 */
  if(data->irrig_system==SURF)
  {
    data->ec=param.ec_canal[(stand->soil.par->Ks>20) ? 0 : (stand->soil.par->Ks>=10 && stand->soil.par->Ks<=20) ? 1 : 2];
    data->conv_evap=((1-param.ec_canal[0])*0.5)/(1-data->ec); /* evaporative share of conveyance loss changes with soil type (sand: 0.5, loam: 0.6, clay: 0.75) */
  }
  else /* set these parameters also in case of NOIRRIG to avoid zero division */
  {
    data->ec=param.ec_pipe;
    data->conv_evap=0.5; /* for sprinkler and drip: half of conv-losses is assumed to evaporate */
  }

  /* - called in landusechange
   * - function annually updates irrigation systems for grassstand, biomass_grass and biomass_tree stand
   * - crop stands are not changed within current growing season, only with cultivate */

} /*of set_irrigsystem*/


void landusechange(Cell *cell,          /**< pointer to cell */
                   int npft,            /**< number of natural PFTs */
                   int ncft,            /**< number of crop PFTs */
                   Bool intercrop,      /**< intercropping possible (TRUE/FALSE) */
                   int year,            /**< simulation year (AD) */
                   const Config *config /**< LPJmL configuration */
                  )
  /* needs to be called before establishment, to ensure that regrowth is possible in the following year*/
{
  Real difffrac,difffrac2,movefrac,difffrac_rice[2];
  Stand *stand, *tempstand, *irrigstand;
  Bool irrigation;
  Irrigation *data;
  Cultivation_type cultivation_type;
  Real grassfrac;
  Real cropfrac;
  Real sum[2],sum_wl[2]; /* rainfed, irrigated */
  int s,s2,pos;
  Bool i,p;
#if defined IMAGE && defined COUPLED
  int nnat;
  Real timberharvest=0;
  int nats[5];
  Real natfrac[5];
#endif
#ifdef CHECK_BALANCE
//anpp, influx and arh do not change
  Stocks start = {0,0};
  Stocks end = {0,0};
  Stocks st;
  Stocks fluxes_fire= {0,0};
  Stocks fluxes_estab= {0,0};
  Stocks fluxes_neg= {0,0};
  Stocks fluxes_prod= {0,0};
  Stocks fluxes_firewood={0,0};
  Stocks balance= {0,0};
  Real start_w = 0;
  Real end_w = 0;
  foreachstand(stand, s, cell->standlist)
  {
    st=standstocks(stand);
    start.carbon+=(st.carbon+ soilmethane(&stand->soil)*WC/WCH4)*stand->frac;//-stand->cell->balance.flux_estab.carbon;
    start.nitrogen+=st.nitrogen*stand->frac;//-stand->cell->balance.flux_estab.nitrogen;
    start_w += soilwater(&stand->soil)*stand->frac;
  }
  start.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  start.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;

  fluxes_fire=cell->balance.fire;
  fluxes_estab=cell->balance.flux_estab;
  fluxes_neg=cell->balance.neg_fluxes;
  fluxes_prod.carbon=(cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon+cell->balance.trad_biofuel.carbon);
  fluxes_prod.nitrogen=(cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen+cell->balance.trad_biofuel.nitrogen);
  fluxes_firewood=cell->balance.flux_firewood;
#endif

   if(cell->ml.dam)
    landusechange_for_reservoir(cell,npft,ncft,intercrop,year,config);
  /* test if land needs to be reallocated between setaside stands */
  difffrac=crop_sum_frac(cell->ml.landfrac,ncft,config->nagtree,cell->ml.reservoirfrac+cell->lakefrac,FALSE)-cell->ml.cropfrac_rf-cell->ml.landfrac[0].crop[RICE]; //cropfrac is without wetland setaside
  difffrac2=crop_sum_frac(cell->ml.landfrac,ncft,config->nagtree,cell->ml.reservoirfrac+cell->lakefrac,TRUE)-cell->ml.cropfrac_ir-cell->ml.landfrac[1].crop[RICE];
  if(difffrac*difffrac2<-epsilon*epsilon) /* if one increases while the other decreases */
  {
    s=findlandusetype(cell->standlist,SETASIDE_RF);
    s2=findlandusetype(cell->standlist,SETASIDE_IR);
    if(s!=NOT_FOUND && s2!=NOT_FOUND)
    {
      stand=getstand(cell->standlist,s);
      irrigstand=getstand(cell->standlist,s2);
      if(year==2003)fprintf(stderr,"irrigstandfrac: %g rainfedfrac: %g\n",irrigstand->frac,stand->frac);
      if(difffrac2<0) /* move irrigated setaside to rainfed setaside */
      {
        movefrac=min(-difffrac2,difffrac);
        if(movefrac+epsilon>=irrigstand->frac)/* move all */
        {
          cutpfts(irrigstand,config);
          mixsetaside(getstand(cell->standlist,s),irrigstand,intercrop,year,npft+ncft,config);
          delstand(cell->standlist,s2);
        }
        else
        {
          pos=addstand(&setaside_rf_stand,cell)-1;
          tempstand=getstand(cell->standlist,pos);
          tempstand->frac=movefrac;
          reclaim_land(irrigstand,tempstand,cell,FALSE,npft+ncft,config);
          irrigstand->frac-=movefrac;
          if(setaside(cell,getstand(cell->standlist,pos),cell->ml.with_tillage,intercrop,npft,ncft,TRUE,FALSE,year,config))
            delstand(cell->standlist,pos);
        }
      }
      else /* move rainfed setaside to irrigated setaside */
      {
        movefrac=min(difffrac2,-difffrac);
        stand=getstand(cell->standlist,s);

        if(movefrac+epsilon>=stand->frac)/* move all */
        {
          cutpfts(stand,config);
          mixsetaside(getstand(cell->standlist,s2),stand,intercrop,year,npft+ncft,config);
          delstand(cell->standlist,s);
        }
        else
        {
          pos=addstand(&setaside_ir_stand,cell)-1;
          tempstand=getstand(cell->standlist,pos);
          tempstand->frac=movefrac;
          reclaim_land(stand,tempstand,cell,FALSE,npft+ncft,config);
          stand->frac-=movefrac;
          if(setaside(cell,getstand(cell->standlist,pos),cell->ml.with_tillage,intercrop,npft,ncft,TRUE,stand->soil.iswetland,year,config))
             delstand(cell->standlist,pos);
        }
      }
    }
  }
  /* update rainfed and irrigated sum of fractions */
  sum[0]=sum[1]=0.0;
  foreachstand(stand,s,cell->standlist)
    if(stand->type->landusetype!=NATURAL && stand->type->landusetype!=WETLAND && stand->type->landusetype!=KILL)
    {
      data=stand->data;
      sum[data->irrigation]+=stand->frac;
    }
  cell->ml.cropfrac_rf=sum[0];
  cell->ml.cropfrac_ir=sum[1];

#ifdef CHECK_BALANCE
  end.carbon=end.nitrogen = end_w=0;
  foreachstand(stand, s, cell->standlist)
  {
    st=standstocks(stand);
    end.carbon+=(st.carbon+ soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
    end.nitrogen+=st.nitrogen*stand->frac;
    end_w+= soilwater(&stand->soil)*stand->frac;
  }
  end.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  end.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;

  balance.carbon=(cell->balance.flux_estab.carbon-fluxes_estab.carbon)-(cell->balance.fire.carbon-fluxes_fire.carbon)-(cell->balance.neg_fluxes.carbon-fluxes_neg.carbon)
      -((cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon+cell->balance.trad_biofuel.carbon)-fluxes_prod.carbon)-(cell->balance.flux_firewood.carbon-fluxes_firewood.carbon);
  balance.nitrogen=(cell->balance.flux_estab.nitrogen-fluxes_estab.nitrogen)-(cell->balance.fire.nitrogen-fluxes_fire.nitrogen)-(cell->balance.neg_fluxes.nitrogen-fluxes_neg.nitrogen)
      -((cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen+cell->balance.trad_biofuel.nitrogen)-fluxes_prod.nitrogen)-(cell->balance.flux_firewood.nitrogen-fluxes_firewood.nitrogen);
  if(fabs(start.carbon-end.carbon+balance.carbon)>0.001)
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid carbon balance in %s after moving setaside year=%d: C_ERROR=%g start : %g end : %g balance.carbon: %g difffrac: %g difffrac2: %g rice_rainfed: %g rice_irr: %g wl_rf: %g wl_irr: %g\n",
         __FUNCTION__,year,start.carbon-end.carbon+balance.carbon,start.carbon,end.carbon,balance.carbon,difffrac,difffrac2,cell->ml.landfrac[0].crop[RICE],cell->ml.landfrac[1].crop[RICE],cell->ml.cropfrac_wl[0],cell->ml.cropfrac_wl[1]);
  if (fabs(start_w - end_w)>0.001)
    fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid water balance in %s after moving setaside: year=%d: W_ERROR=%g start : %g end : %g\n",
         __FUNCTION__,year, start_w - end_w, start_w, end_w);
  if (fabs(start.nitrogen-end.nitrogen+balance.nitrogen)>0.001)
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid nitrogen balance in %s after moving setaside: year=%d: error=%g start : %g end : %g balance.nitrogen: %g\n",
         __FUNCTION__,year, start.nitrogen-end.nitrogen+balance.nitrogen,start.nitrogen,end.nitrogen,balance.nitrogen);
#endif



  for(i=0;i<2;i++)
  {
    cropfrac= i==0 ? cell->ml.cropfrac_rf : cell->ml.cropfrac_ir;

    difffrac=crop_sum_frac(cell->ml.landfrac,ncft,config->nagtree,cell->ml.reservoirfrac+cell->lakefrac,i)-cell->ml.landfrac[i].crop[RICE]-cropfrac;; /*  added the resfrac, see function AND replaced to BEFORE next three lines */
    difffrac_rice[i]=cell->ml.landfrac[i].crop[RICE]-cell->ml.cropfrac_wl[i];

    grassfrac=cell->ml.landfrac[i].grass[0]+cell->ml.landfrac[i].grass[1]; /* pasture + others */

    // FIRST CONVERTION OF WETLANDS BECAUSE RICE SHOULD BE PREFERRED

    if(difffrac_rice[i]<=-epsilon)
      regrowth(cell,difffrac_rice[i],npft,i,TRUE,ncft,year,config);        /*regrowth*/
    else if(difffrac_rice[i]>=epsilon && cell->lakefrac+cell->ml.reservoirfrac+cell->ml.cropfrac_rf+cell->ml.cropfrac_ir+cell->ml.cropfrac_wl[0]+cell->ml.cropfrac_wl[1]<(1-epsilon))
      deforest(cell,difffrac_rice[i],intercrop,npft,FALSE,i,TRUE,ncft,year,minnatfrac_luc,config);  /*deforestation  letzte wieder TRUE"""""""!!!!!!!!!!!!*/

    if(difffrac<=-epsilon)
      regrowth(cell,difffrac,npft,i,FALSE,ncft,year,config);        /*regrowth*/
    else if(difffrac>=epsilon && cell->lakefrac+cell->ml.reservoirfrac+cell->ml.cropfrac_rf+cell->ml.cropfrac_ir+cell->ml.cropfrac_wl[0]+cell->ml.cropfrac_wl[1]<(1-epsilon))
      deforest(cell,difffrac,intercrop,npft,FALSE,i,FALSE,ncft,year,minnatfrac_luc,config);  /*deforestation*/

#ifdef CHECK_BALANCE2
  fprintf(stderr,"LANDUSECHANGE difffrac: %g cropfrac_rf: %g cropfrac_ir: %g cropfrac_wl: %g difffrac_rice: %g grassfrac: %g irrgation:%d \n",
      difffrac,cell->ml.cropfrac_rf,cell->ml.cropfrac_ir,cell->ml.cropfrac_wl[i],difffrac_rice[i],grassfrac, i);
#endif

    /* pasture */
    grassfrac=cell->ml.landfrac[i].grass[1]; /* pasture */
    cultivation_type=PASTURE;
    irrigation=i;
    s=findstand(cell->standlist,GRASSLAND,irrigation);

    if(s!=NOT_FOUND)
    {
      stand=getstand(cell->standlist,s);
      difffrac=stand->frac-grassfrac;
      if(difffrac>epsilon)                                                                            /*grassland will be reduced*/
        grasslandreduction(cell,difffrac,intercrop,npft,s,stand,ncft,year,config);
      else if(difffrac<-epsilon)
        landexpansion(cell,difffrac,npft,stand,irrigation,cultivation_type,0,ncft,year,config);       /*grassland will be expanded*/
#ifdef CHECK_BALANCE2
    fprintf(stderr,"1landexp./grassred difffrac: %g grassfrac: %g s= %d\n",
        difffrac,grassfrac,s);
#endif
    }
    else if(grassfrac>epsilon)                                                                       /*no GRASSLAND changes=-grassfrac */
    {
      difffrac= -grassfrac;
      landexpansion(cell,difffrac,npft,NULL,irrigation,cultivation_type,0,ncft,year,config);
#ifdef CHECK_BALANCE2
    fprintf(stderr,"2landexp./grassred difffrac: %g grassfrac: %g s= %d\n",
        difffrac,grassfrac,s);
#endif
    }
    if(!config->others_to_crop)
    {
      grassfrac=cell->ml.landfrac[i].grass[0]; /* other */
      /* other */
      cultivation_type=OTHER_PASTURE;
      irrigation=i;
      s=findstand(cell->standlist,OTHERS,irrigation);
      if(s!=NOT_FOUND)
      {
        stand=getstand(cell->standlist,s);
        difffrac=stand->frac-grassfrac;
        if(difffrac>epsilon)
          grasslandreduction(cell,difffrac,intercrop,npft,s,stand,ncft,year,config);
        else if(difffrac<-epsilon)
          landexpansion(cell,difffrac,npft,stand,irrigation,cultivation_type,0,ncft,year,config);
#ifdef CHECK_BALANCE2
    fprintf(stderr,"3landexp./grassred difffrac: %g grassfrac: %g s= %d\n",
        difffrac,grassfrac,s);
#endif
      }
      else if(grassfrac>epsilon)
      {
        difffrac= -grassfrac;
        landexpansion(cell,difffrac,npft,NULL,irrigation,cultivation_type,0,ncft,year,config);
#ifdef CHECK_BALANCE2
    fprintf(stderr,"4landexp./grassred difffrac: %g grassfrac: %g s= %d\n",
        difffrac,grassfrac,s);
#endif
      }
    }
    /* Biomass plantations */
    cultivation_type=BIOMASS_TREE_PLANTATION;
    irrigation=i;
    s=findstand(cell->standlist,BIOMASS_TREE,irrigation);
    if(s!=NOT_FOUND)
    {
      stand=getstand(cell->standlist,s);
      difffrac=stand->frac-cell->ml.landfrac[i].biomass_tree;
      if(difffrac>epsilon)
        grasslandreduction(cell,difffrac,intercrop,npft,s,stand,ncft,year,config);
      else if(difffrac<-epsilon)
        landexpansion(cell,difffrac,npft,stand,irrigation,
                      cultivation_type,0,ncft,year,config);
    }
    else if (cell->ml.landfrac[i].biomass_tree>epsilon)
    {
      difffrac= -cell->ml.landfrac[i].biomass_tree;
      landexpansion(cell,difffrac,npft,NULL,
                    irrigation,cultivation_type,0,ncft,year,config);
    }
    /* End biomass plantations */

    /* agriculture tree plantations */
    cultivation_type=AGRICULTURE_TREE_PLANTATION;
    irrigation=i;
    for(p=0;p<config->nagtree;p++)
    {
      if(config->iscotton && !strcmp(config->pftpar[p+npft-config->nagtree].name,"cotton") && cell->ml.sowing_day_cotton[irrigation]==-1)
        continue;
      s=findstandpft(cell->standlist,p+npft-config->nagtree,irrigation);
      if(s!=NOT_FOUND)
      {
        stand=getstand(cell->standlist,s);
        difffrac=stand->frac-cell->ml.landfrac[i].ag_tree[p];
        if(difffrac>epsilon)
          grasslandreduction(cell,difffrac,intercrop,npft,s,stand,ncft,year,config);
        else if(difffrac<-epsilon)
          landexpansion(cell,difffrac,npft,stand,irrigation,
                        cultivation_type,p+npft-config->nagtree,ncft,year,config);
      }
      else if (cell->ml.landfrac[i].ag_tree[p]>epsilon)
      {
        difffrac= -cell->ml.landfrac[i].ag_tree[p];
        landexpansion(cell,difffrac,npft,NULL,
                      irrigation,cultivation_type,p+npft-config->nagtree,ncft,year,config);
      }
    }
    cultivation_type=BIOMASS_GRASS_PLANTATION;
    irrigation=i;
    s=findstand(cell->standlist,BIOMASS_GRASS,irrigation);
    if(s!=NOT_FOUND)
    {
      stand=getstand(cell->standlist,s);
      difffrac=stand->frac-cell->ml.landfrac[i].biomass_grass;
      if(difffrac>epsilon)
        grasslandreduction(cell,difffrac,intercrop,npft,s,stand,ncft,year,config);
      else if(difffrac<-epsilon)
        landexpansion(cell,difffrac,npft,stand,irrigation,
                      cultivation_type,0,ncft,year,config);
    }
    else if (cell->ml.landfrac[i].biomass_grass>epsilon)
    {
      difffrac= -cell->ml.landfrac[i].biomass_grass;
      landexpansion(cell,difffrac,npft,NULL,
                    irrigation,cultivation_type,0,ncft,year,config);
    }

    /* Woodplantations */
    cultivation_type=WOOD_PLANTATION;
    irrigation=i;
    s=findstand(cell->standlist,WOODPLANTATION,irrigation);
    if(s!=NOT_FOUND)
    {
      stand=getstand(cell->standlist,s);
      difffrac=stand->frac-cell->ml.landfrac[i].woodplantation;
      stand->frac_change = -difffrac;
      if(difffrac>epsilon)
        grasslandreduction(cell,difffrac,intercrop,npft,s,stand,ncft,year,config);
      else if(difffrac<-epsilon)
        landexpansion(cell,difffrac,npft,stand,irrigation,
                      cultivation_type,0,ncft,year,config);
    }
    else if (cell->ml.landfrac[i].woodplantation>epsilon)
    {
      difffrac= -cell->ml.landfrac[i].woodplantation;
      landexpansion(cell,difffrac,npft,NULL,
                    irrigation,cultivation_type,0,ncft,year,config);
    }
  }
  check_stand_fracs(cell,cell->lakefrac+cell->ml.reservoirfrac);

  foreachstand(stand,s,cell->standlist)
    if(stand->type->landusetype==GRASSLAND || stand->type->landusetype==BIOMASS_GRASS || stand->type->landusetype==BIOMASS_TREE || stand->type->landusetype==AGRICULTURE_TREE || stand->type->landusetype==AGRICULTURE_GRASS || stand->type->landusetype==WOODPLANTATION || (!config->others_to_crop && stand->type->landusetype==OTHERS)) /* do not update for crops, must be done in sowing functions */
      set_irrigsystem(stand,0,npft,ncft,config); /* no CFT index needed for non-agricultural stands */
#ifdef SAFE
  check_stand_fracs(cell,cell->lakefrac+cell->ml.reservoirfrac);
#endif
  sum[0]=sum[1]=sum_wl[0]=sum_wl[1]=0.0;
  foreachstand(stand,s,cell->standlist)
  {
    if(stand->type->landusetype!=NATURAL && stand->type->landusetype!=WETLAND && stand->type->landusetype!=KILL)
    {
      data=stand->data;
      sum[data->irrigation]+=stand->frac;
      if(stand->soil.iswetland)
        sum_wl[data->irrigation]+=stand->frac;
    }
  }

  cell->ml.cropfrac_rf=sum[0]-sum_wl[0];;
  cell->ml.cropfrac_ir=sum[1]-sum_wl[1];;/* could be different from landusefraction input, due to not harvested winter cereals */
  cell->ml.cropfrac_wl[0]=sum_wl[0];
  cell->ml.cropfrac_wl[1]=sum_wl[1];

#if defined IMAGE && defined COUPLED
    /* if timber harvest not satisfied by agricultural expansion */
    if(config->luc_timber && cell->ml.image_data->timber_frac>epsilon)
    {
      s=findlandusetype(cell->standlist,NATURAL);
      if(s!=NOT_FOUND)
      {
        stand=getstand(cell->standlist,s);
        timberharvest=(stand->frac<=cell->ml.image_data->timber_frac) ? stand->frac : cell->ml.image_data->timber_frac;

        if(timberharvest>epsilon)
        {
          /* deforestation without conversion to agricultural land */
          deforest_for_timber(cell,timberharvest,npft,config->luc_timber,ncft,minnatfrac_luc,year,config);
        }
        cell->ml.image_data->timber_frac=0.0;
      }
      cell->ml.image_data->timber_frac=0.0;
    }
  /* check that sum of fractions is 1.0 */
  check_stand_fracs(cell,cell->lakefrac+cell->ml.reservoirfrac);

  /* check if there is more than 1 natural stand */
  nnat = 0;
  foreachstand(stand,s,cell->standlist)
    if(stand->type->landusetype==NATURAL)
    {
      nnat+=1;
      natfrac[nnat] = stand->frac;
      nats[nnat]    = s;
    }
  if (nnat > 1)
  {
    fprintf(stderr,"ERROR landusechange 1: (%g/%g) more than 1 natural stand in year %d in cell: %i: \n",cell->coord.lon,cell->coord.lat,year,nnat);
    for(i=1;i<nnat+1;i++)
      fprintf(stderr, "i %i no %i frac %g \n",i,nats[i],natfrac[i]);
  }
#endif

#ifdef CHECK_BALANCE
  end.carbon=end.nitrogen = end_w=0;
  foreachstand(stand, s, cell->standlist)
  {
    st=standstocks(stand);
    end.carbon+=(st.carbon+ soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
    end.nitrogen+=st.nitrogen*stand->frac;
    end_w+= soilwater(&stand->soil)*stand->frac;
  }
  end.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  end.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;

  balance.carbon=(cell->balance.flux_estab.carbon-fluxes_estab.carbon)-(cell->balance.fire.carbon-fluxes_fire.carbon)-(cell->balance.neg_fluxes.carbon-fluxes_neg.carbon)
      -((cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon+cell->balance.trad_biofuel.carbon)-fluxes_prod.carbon)-(cell->balance.flux_firewood.carbon-fluxes_firewood.carbon);
  balance.nitrogen=(cell->balance.flux_estab.nitrogen-fluxes_estab.nitrogen)-(cell->balance.fire.nitrogen-fluxes_fire.nitrogen)-(cell->balance.neg_fluxes.nitrogen-fluxes_neg.nitrogen)
      -((cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen+cell->balance.trad_biofuel.nitrogen)-fluxes_prod.nitrogen)-(cell->balance.flux_firewood.nitrogen-fluxes_firewood.nitrogen);
  if(fabs(start.carbon-end.carbon+balance.carbon)>0.001)
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid carbon balance in %s at the end: year=%d: C_ERROR=%g start : %g end : %g balance.carbon: %g",
         __FUNCTION__,year,start.carbon-end.carbon+balance.carbon,start.carbon,end.carbon,balance.carbon);
  if (fabs(start_w - end_w)>0.001)
    fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid water balance in %s at the end: year=%d: W_ERROR=%g start : %g end : %g",
         __FUNCTION__,year, start_w - end_w, start_w, end_w);
  if (fabs(start.nitrogen-end.nitrogen+balance.nitrogen)>0.001)
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid nitrogen balance in %s at the end: year=%d: error=%g start : %g end : %g balance.nitrogen: %g",
         __FUNCTION__,year, start.nitrogen-end.nitrogen+balance.nitrogen,start.nitrogen,end.nitrogen,balance.nitrogen);
#endif
} /* of 'landusechange' */


/*
- called in iterateyear.c
- calls the function vec_sum()
  -> vec_sum() sums the fractions of each crop in the considered cell
     -> share of agricultural land in the cell
  -> calls getcelllanduse() (see landuse.h)
- compares actual share with new share
- if the new crop share is greater than the actual share then calls local function
  deforest()
  -> deforest() checks if a stand with natural vegetation still exist
     (see existlandusetype.c in tools)
  -> if no natural stand exists deforest is not possible
     -> error in input file
  -> if natural stand exists:
     - add new stand to the standlist and save the position of the new stand in
       the standlist (see addstand() in standlist.c)
     - brings new stand to set-aside stand as follows:
       -> calls function reclaim_land() which copies the values of the natural
          stand to the new stand (except the pftlist) and updates the litter pools
          of the new stand
  -> updates the fraction of the natural stand
     -> deletes the natural stand if fraction is zero
  -> calls the function setaside() which adds the new stand to the set-aside stand
     if exist, or sets the new stand to the set-aside stand if no set-aside stand
     still there

- if the new crop share is smaller than actual share then calls local function
  regrowth()
  -> regrowth() checks if set-aside stand exist (see existlandusetype.c in
     tools)
  -> if no set-aside stand exists regrowth is still not possible
     -> regrowth will be next year
  -> if set-aside stand exists:
     -> distinguish if fraction of set-aside stand is smaller equal or greater
    than the determined fraction for reforestation
     -> if the fraction is smaller equal the whole set-aside stand gets natural
    -> update of litter pools for the pfts on the set-aside stand
     -> if the fraction is greater a new stand is added to the standlist
    -> call of function reclaim_land()
    -> update of the fraction of the set-aside stand

     -> distinguish if a natural stand exists
    -> if natural stand exist, mix of soil and vegetation (see mixsoil() in
       setaside.c and specific functions mix_veg_tree/grass.c)
  -> call of function establishmentpft()

- landusechange of managed grassland
  -> distinguish between irrigated and not irrigated managed grassland stand
  -> distinguish if grassland stand exists:
  -> if the new grassland share is smaller than the actual share then calls the
     local function grasslandreduction()
     -> distinguish if fraction of grassland stand is smaller equal or greater
    than the determined fraction for reforestation
     -> if the fraction is smaller equal the whole grassland stand goes to the
        set-aside stand
    -> update of litter pools for the pfts on the grassland stand
        -> call of function setaside()
     -> if the fraction is greater a new stand is added to the standlist
    -> call of function reclaim_land()
    -> update of the fraction of the grassland stand
        -> call of function setaside()

  -> if the new grassland share is greater than the actual share then calls the
     local function landexpansion()
     -> if no set-aside stand exists grassland expansion is still not possible
        -> grassland expansion will be next year
     -> if set-aside stand exists:
        -> distinguish if fraction of set-aside stand is smaller equal or greater
       than the determined fraction for grassland expansion
        -> if the fraction is smaller equal the whole set-aside stand gets
           grassland
       -> update of litter pools for the pfts on the set-aside stand
        -> if the fraction is greater a new stand is added to the standlist
       -> call of function reclaim_land()
       -> update of the fraction of the set-aside stand

     -> distinguish if a grassland stand exists
     -> if grassland stand exist, mix of soil and vegetation (see mixsoil() in
    setaside.c and specific function mix_veg_grass.c)
    -> update of the fraction of the grassland stand
     -> if grassland stand does not exist, the temporary mixstand gets grassland
        stand
        -> call of function establish() and add pft to the pftlist if possible
        -> call of function establishment()
        -> set the variable irrigation

  -> if the grassland stand does not exist and the grassland stand fraction is
     greater 0 then calls the local function landexpansion()

- sets the actual crop share to the new crop share
*/

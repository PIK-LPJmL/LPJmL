/**************************************************************************************/
/**                                                                                \n**/
/**             c  h  e  c  k  _  f  l  u  x  e  s  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function checks for water, nitrogen and carbon balance in a cell           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void check_fluxes(Cell *cell,          /**< cell pointer */
                  int year,            /**< simulation year (AD) */
                  int cellid,          /**< cell index */
                  const Config *config /**< LPJmL configuration */
                 )
{
  Stocks tot={0,0},stocks,delta_tot,balance;
  Real balanceW,totw,awater_flux,fraction;
  Stand *stand;
  String line;
  int s,i,startyear;
#if defined IMAGE && defined COUPLED
  int p;
  Pft *pft;
#endif
  /* carbon balance check */
  awater_flux = cell->balance.awater_flux;
  fraction=0;
/*
  if(year>=1940 && year<=1980)
  {
    foreachstand(stand,s,cell->standlist)
      fprintf(stderr,"\n\nYEAR= %d standfrac: %g standtype: %d iswetland: %d cropfraction_rf: %g cropfraction_irr: %g grasfrac_rf: %g grasfrac_irr: %g\n",year,stand->frac, stand->type->landusetype,stand->soil.iswetland,
              crop_sum_frac(cell->ml.landfrac,12,config->nagtree,cell->ml.reservoirfrac+cell->lakefrac,FALSE),crop_sum_frac(cell->ml.landfrac,12,config->nagtree,cell->ml.reservoirfrac+cell->lakefrac,TRUE),
              cell->ml.landfrac[0].grass[0]+cell->ml.landfrac[0].grass[1],cell->ml.landfrac[1].grass[0]+cell->ml.landfrac[1].grass[1]);
  }
*/


  foreachstand(stand,s,cell->standlist)
  {
    stocks=standstocks(stand);
    tot.carbon+=stocks.carbon*stand->frac;
    tot.nitrogen+=stocks.nitrogen*stand->frac;
    ////// Soil->ch4 must be multiplied by WC/WCH4 as well as cell->balance.aCH4_em
    tot.carbon += soilmethane(&stand->soil)*stand->frac*WC/WCH4;
    fraction+=stand->frac;
  }
  if(cell->ml.dam)
  {
    tot.carbon+=cell->ml.resdata->pool.carbon;
    tot.nitrogen+=cell->ml.resdata->pool.nitrogen;
  }
  tot.carbon+=cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  tot.nitrogen+=cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;
  tot.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon;
  tot.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+cell->NO3_lateral;
  delta_tot.carbon=tot.carbon-cell->balance.tot.carbon;
  delta_tot.nitrogen=tot.nitrogen-cell->balance.tot.nitrogen;
  cell->balance.tot=tot;
////// Soil->ch4 must be multiplied by WC/WCH4 as well as cell->balance.aCH4_em;
  balance.carbon=cell->balance.anpp-cell->balance.arh-cell->balance.fire.carbon+
                 cell->balance.flux_estab.carbon-cell->balance.flux_harvest.carbon-cell->balance.biomass_yield.carbon-delta_tot.carbon-
                 cell->balance.neg_fluxes.carbon+cell->balance.influx.carbon-(cell->balance.aCH4_em+cell->balance.aCH4_sink)*WC/WCH4;
  balance.nitrogen=cell->balance.influx.nitrogen-cell->balance.fire.nitrogen-cell->balance.n_outflux+cell->balance.flux_estab.nitrogen-
                   cell->balance.biomass_yield.nitrogen-cell->balance.flux_harvest.nitrogen-delta_tot.nitrogen-cell->balance.neg_fluxes.nitrogen;

   /* for IMAGE but can also be used without IMAGE */
#ifdef IMAGE
  balance.carbon-=cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon+cell->balance.trad_biofuel.carbon+cell->balance.timber_harvest.carbon;
#else
  balance.carbon-=(cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon+
                   cell->balance.trad_biofuel.carbon);    //+cell->balance.timber_harvest.carbon); //goes in product pool
  balance.nitrogen-=(cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen+
                    cell->balance.trad_biofuel.nitrogen);  //+cell->balance.timber_harvest.nitrogen); //goes in product pool
#endif
  if(config->ischeckpoint)
    startyear=max(config->firstyear-config->nspinup,config->checkpointyear)+1;
  else if (config->withlanduse)
    startyear=config->firstyear-config->nspinup+2;
  else
    startyear=config->firstyear-config->nspinup+param.veg_equil_year+param.equisoil_interval*param.nequilsoil+param.equisoil_fadeout+2;

  if(year>startyear && fabs(balance.carbon)>param.error_limit.stocks.carbon)
  {
#if defined IMAGE && defined COUPLED
    if(config->sim_id==LPJML_IMAGE)
      foreachstand(stand,s,cell->standlist)
      {
        printf("C-balance on stand %d LUT %s, standfrac %g litter %g timber_frac %g\n",
          s,stand->type->name,stand->frac,littercarbon(&stand->soil.litter),cell->ml.image_data->timber_frac);
        fflush(stdout);
        foreachpft(pft,p,&stand->pftlist)
          printf("pft %d, %s, vegc %g\n",
                 p,pft->par->name,vegc_sum(pft));
        fflush(stdout);
      } /* of 'foreachstand' */
#ifdef NO_FAIL_BALANCE
    fprintf(stderr,"ERROR004: "
#else
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,
#endif
         "y: %d c: %d (%s) BALANCE_C-error %.10f nep: %.2f firec: %.2f flux_estab: %.2f flux_harvest: %.2f delta_totc: %.2f\ndeforest_emissions: %.2f product_turnover: %.2f "
         "trad_biofuel: %.2f product pools %.2f %.2f timber_harvest %.2f ftimber %.2f fburn %.2f c_influx %.2f totalfrac: %.5f\n",
         year,cellid+config->startgrid,sprintcoord(line,&cell->coord),balance.carbon,cell->balance.anpp-cell->balance.arh,
         cell->balance.fire.carbon,
         cell->balance.flux_estab.carbon,cell->balance.flux_harvest.carbon,delta_tot.carbon,
         cell->balance.deforest_emissions.carbon,cell->balance.prod_turnover.fast.carbon,cell->balance.trad_biofuel.carbon,
         cell->ml.product.slow.carbon,cell->ml.product.fast.carbon,cell->balance.timber_harvest.carbon,
         cell->ml.image_data->timber_f.fast,cell->ml.image_data->fburnt,cell->balance.influx.carbon,fraction);
#else
#ifdef NO_FAIL_BALANCE
    fprintf(stderr,"ERROR004: "
#else
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,
#endif
         "y: %d c: %d (%s) BALANCE_C-error: %.10f anpp: %.2f rh: %.2f\n"
         "                            firec: %.2f flux_estab: %.2f flux_harvest: %.2f \n"
         "                            delta_totc: %.2f tot.carbon: %.2f biomass_yield: %.2f\n"
         "                            estab_storage_grass: %.2f %.2f estab_storage_tree %.2f %.2f\n"
         "                            deforest_emissions: %.2f product_turnover: %.2f trad_biofuel: %.2f product pools %.2f %.2f timber_harvest.carbon: %.2f c_influx %.2f "
         "                            totalfrac: %.5f reservoirfrac: %.5f reservoiryear: %d\n",
         year,cellid+config->startgrid,sprintcoord(line,&cell->coord),balance.carbon,cell->balance.anpp,cell->balance.arh,cell->balance.fire.carbon,
         cell->balance.flux_estab.carbon,cell->balance.flux_harvest.carbon,delta_tot.carbon,tot.carbon,cell->balance.biomass_yield.carbon,
         cell->balance.estab_storage_grass[0].carbon,cell->balance.estab_storage_grass[1].carbon,cell->balance.estab_storage_tree[0].carbon,cell->balance.estab_storage_tree[1].carbon,
         cell->balance.deforest_emissions.carbon,cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon,cell->balance.trad_biofuel.carbon,
         cell->ml.product.slow.carbon,cell->ml.product.fast.carbon,cell->balance.timber_harvest.carbon,cell->balance.influx.carbon,fraction,cell->ml.reservoirfrac,cell->ml.resdata->reservoir.year);
#endif
  }
#ifdef CHECK_BALANCE2
         foreachstand(stand,s,cell->standlist)
             fprintf(stderr,"standfrac: %g standtype: %s s= %d iswetland: %d cropfraction_rf: %g cropfraction_irr: %g grasfrac_rf: %g grasfrac_irr: %g totalfrac: %.5f \n",
                     stand->frac, stand->type->name,s,stand->soil.iswetland, crop_sum_frac(cell->ml.landfrac,12,config->nagtree,cell->ml.reservoirfrac+cell->lakefrac,FALSE),
                     crop_sum_frac(cell->ml.landfrac,12,config->nagtree,cell->ml.reservoirfrac+cell->lakefrac,TRUE),
                     cell->ml.landfrac[0].grass[0]+cell->ml.landfrac[0].grass[1],cell->ml.landfrac[1].grass[0]+cell->ml.landfrac[1].grass[1],fraction);

#endif
  if(year>startyear && fabs(balance.nitrogen)>param.error_limit.stocks.nitrogen)
  {
#ifdef NO_FAIL_BALANCE
    fprintf(stderr,"ERROR037: "
#else
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,
#endif
      "y: %d c: %d (%s) BALANCE_N-error: %.10f n_influx %g n_outflux %g \n"
      "           delta_tot=%g total nitrogen=%g n_harvest %g n_biomass_yield %g n_estab %g NO3_lateral %g \n"
      "           n_defor_emis %g n_product_turnover %g trad_biofuel: %g timber.harvest: %g \n"
      "           estab_storage grass [0] = %g estab_storage grass [1] = %g  estab_storage tree [0] = %g estab_storage tree [1]= %g neg_fluxes= %g totalfrac: %.5f\n",
      year,cellid+config->startgrid,sprintcoord(line,&cell->coord),balance.nitrogen,cell->balance.influx.nitrogen,cell->balance.n_outflux,
      delta_tot.nitrogen,tot.nitrogen,cell->balance.flux_harvest.nitrogen,cell->balance.biomass_yield.nitrogen,cell->balance.flux_estab.nitrogen,cell->NO3_lateral,
      cell->balance.deforest_emissions.nitrogen, cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen,cell->balance.trad_biofuel.nitrogen,cell->balance.timber_harvest.nitrogen,
      cell->balance.estab_storage_grass[0].nitrogen,cell->balance.estab_storage_grass[1].nitrogen,cell->balance.estab_storage_tree[0].nitrogen,
      cell->balance.estab_storage_tree[1].nitrogen,cell->balance.neg_fluxes.nitrogen,fraction);
/*
      foreachstand(stand,s,cell->standlist){
        foreachpft(pft,p,&stand->pftlist){
          fprintf(stderr,"PFT bm_inc nitrogen %g\n",pft->bm_inc.nitrogen);
        }
      }
*/
  }

  /* water balance check */
#ifdef IMAGE
  totw=(cell->discharge.dmass_lake+cell->discharge.dmass_river+cell->discharge.dmass_gw)/cell->coord.area;            //CAREFULL HERE IT HAS CHANGED
#else
  totw=(cell->discharge.dmass_lake+cell->discharge.dmass_river)/cell->coord.area+cell->ground_st+cell->ground_st_am+cell->lateral_water;
#endif
  foreachstand(stand,s,cell->standlist)
  {
    totw+=soilwater(&stand->soil)*stand->frac;
  }
  if(cell->ml.dam)
  {
    totw+=cell->ml.resdata->dmass/cell->coord.area;/*+cell->resdata->dfout_irrigation/cell->coord.area; */
    for(i=0;i<NIRRIGDAYS;i++)
      totw+=cell->ml.resdata->dfout_irrigation_daily[i]/cell->coord.area;
  }
  cell->balance.awater_flux+=cell->balance.atransp+cell->balance.aevap+cell->balance.ainterc+cell->balance.aevap_lake+cell->balance.aevap_res-cell->balance.airrig;
  balanceW=totw-cell->balance.totw-cell->balance.aprec-cell->balance.aMT_water+cell->balance.awater_flux+cell->balance.excess_water;
  if(year>startyear && fabs(balanceW)>param.error_limit.w_local)
  //if(year>1511 && fabs(balanceW)>1.5)
#ifdef NO_FAIL_BALANCE
      fprintf(stderr,"ERROR005: "
#else
      fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,
#endif
         "y: %d c: %d (%s) BALANCE_W-error %.2f \n "
         "cell->totw: %.2f totw: %.2f lateral_water: %.2f \n "
         "awater_flux:%.2f awater_flux_before:%.2f \n "
         "aprec:%.2f excess water:%.2f \n atransp: %g  aevap %g  ainterc %g \n "
         "runoff %g aevap_lake %g  aevap_res %g  airrig %g  aMT_water %g dmass_lake: %g dmass_river: %g\n",
         year,cellid+config->startgrid,sprintcoord(line,&cell->coord),balanceW,cell->balance.totw,totw,cell->lateral_water,
         cell->balance.awater_flux,awater_flux,cell->balance.aprec,cell->balance.excess_water,cell->balance.atransp,cell->balance.aevap,
         cell->balance.ainterc,cell->discharge.drunoff,cell->balance.aevap_lake,cell->balance.aevap_res,cell->balance.airrig,cell->balance.aMT_water,cell->discharge.dmass_lake,cell->discharge.dmass_river);
  cell->balance.totw=totw;
} /* of 'check_fluxes' */

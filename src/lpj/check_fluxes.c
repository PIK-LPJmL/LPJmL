/**************************************************************************************/
/**                                                                                \n**/
/**             c  h  e  c  k  _  f  l  u  x  e  s  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function checks for water and carbon balance in a cell                     \n**/
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
  Real balanceC,totc,delta_totc;
  Real balanceW,totw;
  Stand *stand;
  String line;
  int s,i,startyear;

#ifdef IMAGE
  int p;
  Pft *pft;
#endif
  /* carbon balance check */
  totc=0;

  foreachstand(stand,s,cell->standlist)
    totc+=standcarbon(stand)*stand->frac;
  if(cell->ml.dam)
    totc+=cell->ml.resdata->c_pool;
  totc+=cell->balance.estab_storage_grass[0]+cell->balance.estab_storage_tree[0]+cell->balance.estab_storage_grass[1]+cell->balance.estab_storage_tree[1];
#ifdef IMAGE
  if(config->sim_id==LPJML_IMAGE)
    totc+=cell->ml.image_data->timber.slow+cell->ml.image_data->timber.fast;
#endif
  delta_totc=totc-cell->balance.totc;
  cell->balance.totc=totc;

  balanceC=cell->balance.nep-cell->output.firec-cell->output.flux_firewood+cell->output.flux_estab-cell->output.flux_harvest-cell->balance.biomass_yield-delta_totc;
  /* for IMAGE but can also be used without IMAGE */
  balanceC-=cell->output.deforest_emissions+cell->output.prod_turnover+cell->output.trad_biofuel;
  if(config->ischeckpoint)
    startyear=max(config->firstyear,config->checkpointyear)+1;
  else
    startyear=config->firstyear+1;
  if(year>startyear && fabs(balanceC)>1)
  {
#ifdef IMAGE
    if(config->sim_id==LPJML_IMAGE)
      foreachstand(stand,s,cell->standlist)
      {
        printf("C-balance on stand %d LUT %s, standfrac %g litter %g timber_frac %g\n",
          s,stand->type->name,stand->frac,littersum(&stand->soil.litter),cell->ml.image_data->timber_frac);
        fflush(stdout);
        foreachpft(pft,p,&stand->pftlist)
          printf("pft %d, %s, vegc %g\n",
                 p,pft->par->name,vegc_sum(pft));
        fflush(stdout);
      } /* of 'foreachstand' */
#ifdef NO_FAIL_BALANCE
    fprintf(stderr,"ERROR004: "
#else
    fail(INVALID_CARBON_BALANCE_ERR,TRUE,
#endif
          "y: %d c: %d (%s) BALANCE_C-error %.10f nep: %.2f firec: %.2f flux_estab: %.2f flux_harvest: %.2f delta_totc: %.2f\ndeforest_emissions: %.2f product_turnover: %.2f trad_biofuel: %.2f product pools %.2f %.2f timber_harvest %.2f ftimber %.2f fburn %.2f\n",
         year,cellid+config->startgrid,sprintcoord(line,&cell->coord),balanceC,cell->balance.nep,
         cell->output.firec,
         cell->output.flux_estab,cell->output.flux_harvest,delta_totc,
         cell->output.deforest_emissions,cell->output.prod_turnover,cell->output.trad_biofuel,
         cell->ml.image_data->timber.slow,cell->ml.image_data->timber.fast,cell->output.timber_harvest,
         cell->ml.image_data->timber_f,cell->ml.image_data->fburnt);
#else
#ifdef NO_FAIL_BALANCE
    fprintf(stderr,"ERROR004: "
#else
    fail(INVALID_CARBON_BALANCE_ERR,TRUE,
#endif
         "y: %d c: %d (%s) BALANCE_C-error %.10f nep: %.2f\n"
         "                            firec: %.2f flux_estab: %.2f \n"
         "                            flux_harvest: %.2f delta_totc: %.2f biomass_yield: %.2f\n"
         "                            estab_storage_grass: %.2f %.2f estab_storage_tree %.2f %.2f\n"
         "                            deforest_emissions: %.2f product_turnover: %.2f\n",
         year,cellid+config->startgrid,sprintcoord(line,&cell->coord),balanceC,cell->balance.nep,cell->output.firec,
         cell->output.flux_estab,cell->output.flux_harvest,delta_totc,cell->balance.biomass_yield,
         cell->balance.estab_storage_grass[0],cell->balance.estab_storage_grass[1],cell->balance.estab_storage_tree[0],cell->balance.estab_storage_tree[1],
         cell->output.deforest_emissions,cell->output.prod_turnover);
#endif
  }
  /* water balance check */
  totw=(cell->discharge.dmass_lake+cell->discharge.dmass_river)/cell->coord.area;
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
  balanceW=totw-cell->balance.totw-cell->balance.aprec+cell->balance.awater_flux;
  if(year>startyear && fabs(balanceW)>1.5)
#ifdef NO_FAIL_BALANCE
    fprintf(stderr,"ERROR005: "
#else
    fail(INVALID_WATER_BALANCE_ERR,TRUE,
#endif
         "y: %d c: %d (%s) BALANCE_W-error %.2f cell->totw:%.2f totw:%.2f awater_flux:%.2f aprec:%.2f\n",
         year,cellid+config->startgrid,sprintcoord(line,&cell->coord),balanceW,cell->balance.totw,totw,
         cell->balance.awater_flux,cell->balance.aprec);
  cell->balance.totw=totw;
} /* of 'check_fluxes' */

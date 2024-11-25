/**************************************************************************************/
/**                                                                                \n**/
/**               u  p  d  a  t  e  _  a  n  n  u  a  l  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function performs necessary updates after iteration over one               \n**/
/**     year                                                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define N 5

void update_annual(Cell *cell,          /**< Pointer to cell */
                   int npft,            /**< number of natural pfts */
                   int ncft,            /**< number of crop pfts */
                   int year,            /**< simulation year (AD) */
                   Bool isdaily,        /**< daily temperature data (TRUE/FALSE) */
                   Bool intercrop,      /**< intercropping (TRUE/FALSE) */
                   const Config *config /**< LPJ configuration */
                  )
{
  int s,m,cft;
  Stand *stand;
  Pftcroppar *croppar;
  Real mintemp[N];
  Stocks litter_neg;
  Real natfrac,wetlandfrac;
#ifdef CHECK_BALANCE
//anpp, influx and arh do not change
  Stocks start = {0,0};
  Stocks end = {0,0};
  Stocks st;
  Stocks fluxes_fire= {0,0};
  Stocks fluxes_estab= {0,0};
  Stocks fluxes_neg= {0,0};
  Stocks fluxes_prod= {0,0};
  Stocks balance= {0,0};
  Real start_w = 0;
  Real end_w = 0;
#endif
  natfrac=wetlandfrac=0;
  if(cell->ml.dam)
    update_reservoir_annual(cell);

  /* Vernalization requirements in case not STATIC_PHU */
  if(config->crop_phu_option!=OLD_CROP_PHU && (config->sdate_option==NO_FIXED_SDATE || year<=config->sdate_fixyear)) /* update only until sdate_fixyear, included NEW_CROP_PHU option for World Modelers */
  {
    getmintemp20_n(&cell->climbuf,mintemp,N);
    for (m=0;m<N;m++)
    {
     for (cft=0;cft<ncft;cft++)
     {
       croppar=config->pftpar[npft+cft].data;
       if (mintemp[m]<=croppar->tv_opt.low && mintemp[m]> -9999)
         cell->climbuf.V_req_a[cft]+=croppar->pvd_max/N; /* maximum number of vernalization days per months */
       else if (mintemp[m]>croppar->tv_opt.low && mintemp[m]<croppar->tv_opt.high)
         cell->climbuf.V_req_a[cft]+=croppar->pvd_max/N*(1-(mintemp[m]-croppar->tv_opt.low)/(croppar->tv_opt.high-croppar->tv_opt.low));
     }
    }
  }

  annual_climbuf(&cell->climbuf,cell->balance.aevap+cell->balance.atransp,ncft,config->crop_phu_option!=OLD_CROP_PHU && (config->sdate_option==NO_FIXED_SDATE || year<=config->sdate_fixyear));
  if(config->sdate_option==NO_FIXED_SDATE ||
    (config->sdate_option==FIXED_SDATE && year<=config->sdate_fixyear)||
    (config->sdate_option>=PRESCRIBED_SDATE && year<=config->sdate_fixyear))
    cell->climbuf.atemp_mean20_fix=cell->climbuf.atemp_mean20;

  /* count number of years without harvest
   * and set 20-year mean to zero if no harvest
   * occurred within the last 10 years
   */
#ifdef CHECK_BALANCE
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
  start_w+=cell->balance.excess_water;
  fluxes_fire=cell->balance.fire;
  fluxes_estab=cell->balance.flux_estab;
  fluxes_neg=cell->balance.neg_fluxes;
  fluxes_prod.carbon=(cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon);
  fluxes_prod.nitrogen=(cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen);
#endif
   if((year<config->firstyear && config->sdate_option<PRESCRIBED_SDATE) || config->sdate_option==NO_FIXED_SDATE)
     update_cropdates(cell->ml.cropdates,ncft);

  foreachstand(stand,s,cell->standlist)
  {
    stand->prescribe_landcover = config->prescribe_landcover;

    stand->soil.mean_maxthaw=(stand->soil.mean_maxthaw-stand->soil.mean_maxthaw/CLIMBUFSIZE)+stand->soil.maxthaw_depth/CLIMBUFSIZE;
    //getoutput(&cell->output,MAXTHAW_DEPTH,config)+=stand->soil.maxthaw_depth*stand->frac*(1.0/(1-stand->cell->lakefrac));
    if(annual_stand(stand,npft,ncft,year,isdaily,intercrop,config))
    {
      /* stand has to be deleted */
      delstand(cell->standlist,s);
      s--; /* adjust stand index */
      continue;
    }
    if(stand->type->landusetype == NATURAL)
      natfrac+=stand->frac;
    if(stand->type->landusetype == WETLAND)
      wetlandfrac+=stand->frac;
  } /* of foreachstand */
#ifdef CHECK_BALANCE
  end_w+=cell->balance.excess_water;
  foreachstand(stand,s,cell->standlist)
  {
    st=standstocks(stand);
    end.carbon+=(st.carbon+soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
    end.nitrogen+=st.nitrogen*stand->frac;
    end_w += soilwater(&stand->soil)*stand->frac;
    //fprintf(stdout,"update_annual: landusetype: %s stand.frac: %g \n\n",stand->type->name, stand->frac);
  }
  end.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  end.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;

  balance.carbon=(cell->balance.flux_estab.carbon-fluxes_estab.carbon)-(cell->balance.fire.carbon-fluxes_fire.carbon)-(cell->balance.neg_fluxes.carbon-fluxes_neg.carbon)
      -((cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon)-fluxes_prod.carbon);
  balance.nitrogen=(cell->balance.flux_estab.nitrogen-fluxes_estab.nitrogen)-(cell->balance.fire.nitrogen-fluxes_fire.nitrogen)-(cell->balance.neg_fluxes.nitrogen-fluxes_neg.nitrogen)
      -((cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen)-fluxes_prod.nitrogen);
  if(fabs(start.carbon-end.carbon+balance.carbon)>0.001) fprintf(stderr,"C_ERROR update annual: year=%d: C_ERROR=%g start : %g end : %g balance.carbon: %g\n",
      year,start.carbon-end.carbon+balance.carbon,start.carbon,end.carbon,balance.carbon);
  if (fabs(start_w - end_w)>0.001) fprintf(stderr, "W_ERROR update annual: year=%d: W_ERROR=%g start : %g end : %g\n",
      year, start_w - end_w, start_w, end_w);
  if (fabs(start.nitrogen - end.nitrogen+balance.nitrogen)>0.001) fprintf(stderr, "N_ERROR update annual: year=%d: error=%g start : %g end : %g balance.nitrogen: %g\n",
      year, start.nitrogen - end.nitrogen+balance.nitrogen, start.nitrogen, end.nitrogen,balance.nitrogen);
#endif
  getoutputindex(&cell->output,FPC,0,config) += natfrac;
  getoutputindex(&cell->output,WPC,0,config) += wetlandfrac;
  cell->hydrotopes.wetland_wtable_mean /= NMONTH;
  cell->hydrotopes.wtable_mean /= NMONTH;
  if(cell->lakefrac<1)
    update_wetland(cell, npft + ncft,ncft,year,config);
#ifdef CHECK_BALANCE
  end.carbon=end.nitrogen = end_w=0;
  end_w+=cell->balance.excess_water;
  foreachstand(stand, s, cell->standlist)
  {
    st=standstocks(stand);
    end.carbon+=(st.carbon+ soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
    end.nitrogen+=st.nitrogen*stand->frac;
    end_w += soilwater(&stand->soil)*stand->frac;
  }
  end.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  end.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;

  balance.carbon=(cell->balance.flux_estab.carbon-fluxes_estab.carbon)-(cell->balance.fire.carbon-fluxes_fire.carbon)-(cell->balance.neg_fluxes.carbon-fluxes_neg.carbon)
      -((cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon)-fluxes_prod.carbon);
  balance.nitrogen=(cell->balance.flux_estab.nitrogen-fluxes_estab.nitrogen)-(cell->balance.fire.nitrogen-fluxes_fire.nitrogen)-(cell->balance.neg_fluxes.nitrogen-fluxes_neg.nitrogen)
      -((cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen)-fluxes_prod.nitrogen);
  if(fabs(start.carbon-end.carbon+balance.carbon)>0.001) fprintf(stderr,"C_ERROR update annual after update_wetland: year=%d: C_ERROR=%g start : %g end : %g balance.carbon: %g\n",
      year,start.carbon-end.carbon+balance.carbon,start.carbon,end.carbon,balance.carbon);
  if (fabs(start_w - end_w)>0.001) fprintf(stderr, "W_ERROR update annual after update_wetland: year=%d: W_ERROR=%g start : %g end : %g\n",
      year, start_w - end_w, start_w, end_w);
  if (fabs(start.nitrogen-end.nitrogen+balance.nitrogen)>0.001) fprintf(stderr,"N_ERROR update annual after update_wetland: year=%d: error=%g start : %g end : %g balance.nitrogen: %g\n",
      year, start.nitrogen-end.nitrogen+balance.nitrogen, start.nitrogen, end.nitrogen,balance.nitrogen);
#endif
  foreachstand(stand,s,cell->standlist)
  {
    if(stand->soil.iswetland==TRUE || stand->type->landusetype==WETLAND)
      getoutput(&cell->output,WETFRAC,config)+=stand->frac;
    litter_neg=checklitter(&stand->soil.litter);
    getoutput(&cell->output,NEGC_FLUXES,config)+=litter_neg.carbon*stand->frac;
    getoutput(&cell->output,NEGN_FLUXES,config)+=litter_neg.nitrogen*stand->frac;
    cell->balance.neg_fluxes.carbon+=litter_neg.carbon*stand->frac;
    cell->balance.neg_fluxes.nitrogen+=litter_neg.nitrogen*stand->frac;
    stand->cell->balance.soil_storage+=soilwater(&stand->soil)*stand->frac*stand->cell->coord.area;
  }
  getoutput(&cell->output,LAND_AREA,config)+=cell->coord.area*(1-cell->lakefrac-cell->ml.reservoirfrac);
  //cell->output.soil_storage+=cell->balance.excess_water*cell->coord.area; /* now tracked in separate flux */
#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
    product_turnover(cell,config);
#else
  /* reset product pools after first year with land use to avoid large peak */
  if(year==config->firstyear-config->nspinup&&!config->landuse_restart)
  {
    cell->ml.product.fast.carbon=cell->ml.product.fast.nitrogen=0;
    cell->ml.product.slow.carbon=cell->ml.product.slow.nitrogen=0;
    cell->balance.deforest_emissions.carbon=cell->balance.deforest_emissions.nitrogen=0;     //this gives trouble for the first year of simulation after restart only for the local balance
  }
  product_turnover(cell,config);
#endif
  if(cell->discharge.dmass_gw<0)
  {
    //fprintf(stderr,"y: %d cell: (%s) negative GW dmass_gw:%g ground_st_am:%g ground_st:%g drunoff:%g \n",year,sprintcoord(line,&cell->coord),cell->discharge.dmass_gw,cell->ground_st_am,cell->ground_st,cell->discharge.drunoff,cell->kbf);
  }
#ifdef CHECK_BALANCE
  end.carbon=end.nitrogen = end_w=0;
  end_w+=cell->balance.excess_water;
  foreachstand(stand, s, cell->standlist)
  {
    st=standstocks(stand);
    end.carbon+=(st.carbon+ soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
    end.nitrogen+=st.nitrogen*stand->frac;
    end_w += soilwater(&stand->soil)*stand->frac;
  }
  end.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  end.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;

  balance.carbon=(cell->balance.flux_estab.carbon-fluxes_estab.carbon)-(cell->balance.fire.carbon-fluxes_fire.carbon)-(cell->balance.neg_fluxes.carbon-fluxes_neg.carbon)
      -((cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon)-fluxes_prod.carbon);
  balance.nitrogen=(cell->balance.flux_estab.nitrogen-fluxes_estab.nitrogen)-(cell->balance.fire.nitrogen-fluxes_fire.nitrogen)-(cell->balance.neg_fluxes.nitrogen-fluxes_neg.nitrogen)
      -((cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen)-fluxes_prod.nitrogen);
  if(fabs(start.carbon-end.carbon+balance.carbon)>0.001) fprintf(stderr,"C_ERROR update annual at the end: year=%d: C_ERROR=%g start : %g end : %g balance.carbon: %g\n",
      year,start.carbon-end.carbon+balance.carbon,start.carbon,end.carbon,balance.carbon);
  if (fabs(start_w - end_w)>epsilon) fprintf(stderr, "W_ERROR update annual at the end: year=%d: W_ERROR=%g start : %g end : %g\n",
      year, start_w - end_w, start_w, end_w);
  if (fabs(start.nitrogen-end.nitrogen+balance.nitrogen)>0.001) fprintf(stderr,"N_ERROR update annual at the end: year=%d: error=%g start : %g end : %g balance.nitrogen: %g\n",
      year, start.nitrogen-end.nitrogen+balance.nitrogen, start.nitrogen, end.nitrogen,balance.nitrogen);
#endif

} /* of 'update_annual' */

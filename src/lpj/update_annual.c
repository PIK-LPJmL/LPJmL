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
                   Real popdens,        /**< population density (capita/km2) */
                   int year,            /**< simulation year (AD) */
                   Bool isdaily,        /**< daily temperature data (TRUE/FALSE) */
                   Bool intercrop,      /**< intercropping (TRUE/FALSE) */
                   const Config *config /**< LPJ configuration */
                  )
{
  int s,p,m,l,cft;
  Pft *pft;
  Stand *stand;
  Pftcroppar *croppar;
  Real mintemp[N];
  Stocks litter_neg;
  Real soilc_agr,litc_agr,stand_fracs;
#ifdef CHECK_BALANCE
  Stocks anfang = {0,0};
  Stocks ende = {0,0};
  Stocks st;
  Real anfang_w = 0;
  Real ende_w = 0;
#endif
  if(cell->ml.dam)
    update_reservoir_annual(cell);

  /* Vernalization requirements in case not STATIC_PHU */
  if(config->crop_phu_option==PRESCRIBED_CROP_PHU && year<=config->sdate_fixyear) /* update only until sdate_fixyear */
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

  annual_climbuf(&cell->climbuf,cell->balance.aevap+cell->balance.atransp,ncft,year,config->crop_phu_option,config->sdate_fixyear);
  if(config->sdate_option==NO_FIXED_SDATE ||
    (config->sdate_option==FIXED_SDATE && year<=config->sdate_fixyear)||
    (config->sdate_option==PRESCRIBED_SDATE && year<=config->sdate_fixyear))
    cell->climbuf.atemp_mean20_fix=cell->climbuf.atemp_mean20;

  /* count number of years without harvest
   * and set 20-year mean to zero if no harvest
   * occurred within the last 10 years
   */
#ifdef CHECK_BALANCE
  foreachstand(stand, s, cell->standlist)
  {
    st=standstocks(stand);
    anfang.carbon+=(st.carbon+ soilmethane(&stand->soil))*stand->frac;
    anfang.nitrogen+=st.nitrogen*stand->frac;
    anfang_w += soilwater(&stand->soil)*stand->frac;
  }
#endif
    if((year<config->firstyear && config->sdate_option!=PRESCRIBED_SDATE) ||
       config->sdate_option==NO_FIXED_SDATE)
     update_cropdates(cell->ml.cropdates,ncft);

  foreachstand(stand,s,cell->standlist)
  {
    stand->prescribe_landcover = config->prescribe_landcover;

    stand->soil.mean_maxthaw=(stand->soil.mean_maxthaw-stand->soil.mean_maxthaw/CLIMBUFSIZE)+stand->soil.maxthaw_depth/CLIMBUFSIZE;
    getoutput(&cell->output,MAXTHAW_DEPTH,config)+=stand->soil.maxthaw_depth*stand->frac*(1.0/(1-stand->cell->lakefrac));
    if(!config->with_nitrogen)
      foreachpft(pft,p,&stand->pftlist)
        pft->vscal=NDAYYEAR;
    if(annual_stand(stand,npft,ncft,popdens,year,isdaily,intercrop,config))
    {
      /* stand has to be deleted */
      delstand(cell->standlist,s);
      s--; /* adjust stand index */
      continue;
    }
    if (stand->type->landusetype == WETLAND)
      foreachpft(pft, p, &stand->pftlist)
        getoutputindex(&cell->output,WPC,getpftpar(pft, id) + 1,config) += pft->fpc;
    else if(stand->type->landusetype == NATURAL)
      foreachpft(pft, p, &stand->pftlist)
        getoutputindex(&cell->output,FPC,getpftpar(pft, id) + 1,config) += pft->fpc;
/*
    cell->output.lit.carbon+=litter_ag_sum(&stand->soil.litter)*stand->frac;
    cell->output.lit.nitrogen+=litter_ag_sum_n(&stand->soil.litter)*stand->frac;
    cell->output.litc_all+=(litter_ag_sum(&stand->soil.litter)+litter_agsub_sum(&stand->soil.litter))*stand->frac;
    for(p=0;p<stand->soil.litter.n;p++)
      cell->output.litc_all+=stand->soil.litter.item[p].bg.carbon*stand->frac;
    cell->output.litc_ag+=litter_ag_sum(&stand->soil.litter)*stand->frac;
*/


  } /* of foreachstand */
#ifdef CHECK_BALANCE
  foreachstand(stand,s,cell->standlist)
  {
    st=standstocks(stand);
    ende.carbon+=(st.carbon+ soilmethane(&stand->soil))*stand->frac;
    ende.nitrogen+=st.nitrogen*stand->frac;
    ende_w += soilwater(&stand->soil)*stand->frac;
    //fprintf(stdout,"update_annual: landusetype: %s stand.frac: %g NEP: %g\n\n",stand->type->name, stand->frac,cell->balance.nep);
  }
  //if(fabs(anfang-ende)>0.001) fprintf(stdout,"C_ERROR update annual - annual stand year=%d: C_ERROR=%g anfang : %g ende : %g\n",year,anfang-ende,anfang,ende);
  if (fabs(anfang_w - ende_w)>0.001) fprintf(stdout, "W_ERROR update annual - annual stand: year=%d: W_ERROR=%g anfang : %g ende : %g\n", year, anfang_w - ende_w, anfang_w, ende_w);
  if (fabs(anfang.nitrogen - ende.nitrogen)>0.001) fprintf(stdout, "N_ERROR update annual - annual stand: year=%d: error=%g anfang : %g ende : %g\n", year, anfang.nitrogen - ende.nitrogen, anfang.nitrogen, ende.nitrogen);
#endif
  getoutputindex(&cell->output,FPC,0,config) += 1 - cell->ml.cropfrac_rf - cell->ml.cropfrac_ir - cell->lakefrac - cell->ml.reservoirfrac - cell->hydrotopes.wetland_area;
  getoutputindex(&cell->output,WPC,0,config) += cell->hydrotopes.wetland_area;
  cell->balance.soil_storage += soilwater(&stand->soil)*stand->frac*stand->cell->coord.area;
  cell->hydrotopes.wetland_wtable_mean /= 12.;
  cell->hydrotopes.wtable_mean /= 12.;

  update_wetland(cell, npft + ncft,year,config);

#ifdef CHECK_BALANCE
  ende.carbon=ende.nitrogen = 0;
  foreachstand(stand, s, cell->standlist)
  {
    st=standstocks(stand);
    ende.carbon+=(st.carbon+ soilmethane(&stand->soil))*stand->frac;
    ende.nitrogen+=st.nitrogen*stand->frac;
    //fprintf(stdout,"update_annual: landusetype: %s stand.frac: %g NEP: %g\n\n",stand->type->name, stand->frac,cell->balance.nep);
  }
  if (fabs(anfang.nitrogen - ende.nitrogen)>0.001) fprintf(stdout, "N_ERROR update annual - annual stand after update_wetland: year=%d: error=%g anfang : %g ende : %g\n", year, anfang.nitrogen - ende.nitrogen, anfang.nitrogen, ende.nitrogen);
#endif
  stand_fracs=0;
  soilc_agr=0;
  litc_agr=0;
  foreachstand(stand,s,cell->standlist)
  {
    if (stand->type->landusetype == SETASIDE_RF || stand->type->landusetype == SETASIDE_IR ||
         stand->type->landusetype == AGRICULTURE)
    {
      stand_fracs += stand->frac;
      for (p = 0; p<stand->soil.litter.n; p++)
        soilc_agr+=stand->soil.litter.item[p].bg.carbon*stand->frac;
      forrootsoillayer(l)
        soilc_agr+=(stand->soil.pool[l].slow.carbon + stand->soil.pool[l].fast.carbon)*stand->frac;
      litc_agr +=(litter_ag_sum(&stand->soil.litter) + litter_agsub_sum(&stand->soil.litter))*stand->frac;
    }
    else if(stand->type->landusetype==WETLAND)
      getoutput(&cell->output,WETFRAC,config)+=stand->frac;
    if(config->with_nitrogen)
    {
      litter_neg=checklitter(&stand->soil.litter);
      getoutput(&cell->output,NEGC_FLUXES,config)+=litter_neg.carbon*stand->frac;
      getoutput(&cell->output,NEGN_FLUXES,config)+=litter_neg.nitrogen*stand->frac;
      cell->balance.neg_fluxes.carbon+=litter_neg.carbon*stand->frac;
      cell->balance.neg_fluxes.nitrogen+=litter_neg.nitrogen*stand->frac;
    }
    stand->cell->balance.soil_storage+=soilwater(&stand->soil)*stand->frac*stand->cell->coord.area;
  }
  //cell->output.soil_storage+=cell->balance.excess_water*cell->coord.area; /* now tracked in separate flux */
  getoutputindex(&cell->output,FPC,0,config) += 1-cell->ml.cropfrac_rf-cell->ml.cropfrac_ir-cell->lakefrac-cell->ml.reservoirfrac;
#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
    product_turnover(cell,config);
#else
  /* reset product pools after first year with land use to avoid large peak */
  if(year==config->firstyear-config->nspinup&&!config->landuse_restart)
  {
    cell->ml.product.fast.carbon=cell->ml.product.fast.nitrogen=0;
    cell->ml.product.slow.carbon=cell->ml.product.slow.nitrogen=0;
    getoutput(&cell->output,TIMBER_HARVESTC,config)=0;
    getoutput(&cell->output,DEFOREST_EMIS,config)=0;
    cell->balance.deforest_emissions.carbon=cell->balance.deforest_emissions.nitrogen=0;
  }
  product_turnover(cell,config);
#endif
} /* of 'update_annual' */

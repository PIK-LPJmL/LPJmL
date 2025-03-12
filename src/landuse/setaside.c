/**************************************************************************************/
/**                                                                                \n**/
/**               s  e  t  a  s  i  d  e  .  c                                     \n**/
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
#include "grass.h"
#include "tree.h"
#include "agriculture.h"

void mixsoilenergy(Stand *,const Stand *,const Config *config);

void mixsoil(Stand *stand1,const Stand *stand2,int year,int ntotpft,const Config *config)
{
  int l,index,i;
  String line;
  Real water1,water2;
  //Real absolute_water1[NSOILLAYER],absolute_water2;
  //Real absolute_ice1[NSOILLAYER],absolute_ice2;
#ifdef CHECK_BALANCE
  Real water_before,water_after;
  Real excess_water=stand1->cell->balance.excess_water;
  Real water_w1,water_w2;
  water_w1=water_w2=0;
  //fprintf(stderr,"soilwater1: %g frac1: %g soilwater2: %g frac2: %g \n",soilwater(&stand1->soil)*stand1->frac,stand1->frac,soilwater(&stand2->soil)*stand2->frac,stand2->frac);
  water_before=soilwater(&stand1->soil)*stand1->frac+soilwater(&stand2->soil)*stand2->frac+stand1->cell->balance.excess_water;
  foreachsoillayer(l)
  {
    water_w1+=(stand1->soil.w[l] * stand1->soil.whcs[l])*stand1->frac;
    water_w2+=(stand2->soil.w[l] * stand2->soil.whcs[l])*stand2->frac;
  }

#endif
  forrootsoillayer(l)
  {
    mixpool(stand1->soil.NH4[l],stand2->soil.NH4[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.NO3[l],stand2->soil.NO3[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.pool[l].fast.carbon,stand2->soil.pool[l].fast.carbon,
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.pool[l].slow.carbon,stand2->soil.pool[l].slow.carbon,
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.pool[l].fast.nitrogen,stand2->soil.pool[l].fast.nitrogen,
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.pool[l].slow.nitrogen,stand2->soil.pool[l].slow.nitrogen,
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.k_mean[l].fast,stand2->soil.k_mean[l].fast,
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.k_mean[l].slow,stand2->soil.k_mean[l].slow,
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.CH4[l],stand2->soil.CH4[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.O2[l],stand2->soil.O2[l],stand1->frac,stand2->frac);
#ifdef SAFE
      if(stand2->soil.NO3[l]<-epsilon || stand1->soil.NO3[l]<-epsilon)
        fail(NEGATIVE_SOIL_NO3_ERR,TRUE,TRUE,"setaside: Negative soil NO3-1=%g NO3-2=%gin layer %d in cell (%s)",
            stand1->soil.NO3[l],stand2->soil.NO3[l],l,sprintcoord(line,&stand2->cell->coord));
      if(stand2->soil.NH4[l]<-epsilon || stand1->soil.NH4[l]<-epsilon)
        fail(NEGATIVE_SOIL_NH4_ERR,TRUE,TRUE,"Negative soil NH4-1=%g  NH4-2=%gin layer %d in cell (%s)",
            stand1->soil.NH4[l],stand2->soil.NH4[l],l,sprintcoord(line,&stand2->cell->coord));
#endif
    mixpool(stand1->soil.decay_rate[l].fast,stand2->soil.decay_rate[l].fast,
            stand1->frac,stand2->frac);
    mixpool(stand1->soil.decay_rate[l].slow,stand2->soil.decay_rate[l].slow,
            stand1->frac,stand2->frac);
  }
  for(l=0;l<stand2->soil.litter.n;l++)
  {
    index=findlitter(&stand1->soil.litter,stand2->soil.litter.item[l].pft);
    if(index==NOT_FOUND)
      index=addlitter(&stand1->soil.litter,stand2->soil.litter.item[l].pft)-1;
    mixpool(stand1->soil.litter.item[index].agtop.leaf.carbon,
            stand2->soil.litter.item[l].agtop.leaf.carbon,stand1->frac,stand2->frac);
    mixpool(stand1->soil.litter.item[index].agtop.leaf.nitrogen,
            stand2->soil.litter.item[l].agtop.leaf.nitrogen,stand1->frac,stand2->frac);
    mixpool(stand1->soil.litter.item[index].agsub.leaf.carbon,
            stand2->soil.litter.item[l].agsub.leaf.carbon,stand1->frac,stand2->frac);
    mixpool(stand1->soil.litter.item[index].agsub.leaf.nitrogen,
            stand2->soil.litter.item[l].agsub.leaf.nitrogen,stand1->frac,stand2->frac);
    mixpool(stand1->soil.litter.item[index].bg.carbon,stand2->soil.litter.item[l].bg.carbon,
          stand1->frac,stand2->frac);
    mixpool(stand1->soil.litter.item[index].bg.nitrogen,stand2->soil.litter.item[l].bg.nitrogen,
          stand1->frac,stand2->frac);
    for(i=0;i<NFUELCLASS;i++)
    {
      mixpool(stand1->soil.litter.item[index].agtop.wood[i].carbon,
              stand2->soil.litter.item[l].agtop.wood[i].carbon,stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.item[index].agtop.wood[i].nitrogen,
              stand2->soil.litter.item[l].agtop.wood[i].nitrogen,stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.item[index].agsub.wood[i].carbon,
              stand2->soil.litter.item[l].agsub.wood[i].carbon,stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.item[index].agsub.wood[i].nitrogen,
              stand2->soil.litter.item[l].agsub.wood[i].nitrogen,stand1->frac,stand2->frac);
    }
  }
  for(l=0;l<stand1->soil.litter.n;l++)
    if(findlitter(&stand2->soil.litter,stand1->soil.litter.item[l].pft)==NOT_FOUND)
    {
      mixpool(stand1->soil.litter.item[l].agtop.leaf.carbon,0,
              stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.item[l].agtop.leaf.nitrogen,0,
              stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.item[l].agsub.leaf.carbon,0,
              stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.item[l].agsub.leaf.nitrogen,0,
              stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.item[l].bg.carbon,0,
              stand1->frac,stand2->frac);
      mixpool(stand1->soil.litter.item[l].bg.nitrogen,0,
              stand1->frac,stand2->frac);
      for(i=0;i<NFUELCLASS;i++)
      {
        mixpool(stand1->soil.litter.item[l].agtop.wood[i].carbon,0,
                stand1->frac,stand2->frac);
        mixpool(stand1->soil.litter.item[l].agtop.wood[i].nitrogen,0,
                stand1->frac,stand2->frac);
        mixpool(stand1->soil.litter.item[l].agsub.wood[i].carbon,0,
                stand1->frac,stand2->frac);
        mixpool(stand1->soil.litter.item[l].agsub.wood[i].nitrogen,0,
                stand1->frac,stand2->frac);
      }
    }
  mixpool(stand1->soil.litter.agtop_moist,stand2->soil.litter.agtop_moist,stand1->frac,stand2->frac);
  mixpool(stand1->soil.litter.agtop_temp,stand2->soil.litter.agtop_temp,stand1->frac,stand2->frac);

  for(i=0;i<=NFUELCLASS;i++)
    mixpool(stand1->soil.litter.avg_fbd[i],stand2->soil.litter.avg_fbd[i],
            stand1->frac,stand2->frac);

  mixpool(stand1->soil.meanw1,stand2->soil.meanw1,
          stand1->frac,stand2->frac);

  /* snowpack is independent of fraction */
  mixpool(stand1->soil.decomp_litter_mean.carbon,stand2->soil.decomp_litter_mean.carbon,
          stand1->frac,stand2->frac);
  mixpool(stand1->soil.decomp_litter_mean.nitrogen,stand2->soil.decomp_litter_mean.nitrogen,
          stand1->frac,stand2->frac);
  mixpool(stand1->soil.fastfrac,stand2->soil.fastfrac,stand1->frac,
          stand2->frac);
  for(i=0;i<ntotpft;i++)
  {
     mixpool(stand1->soil.decomp_litter_pft[i].carbon,stand2->soil.decomp_litter_pft[i].carbon,
             stand1->frac,stand2->frac);
     mixpool(stand1->soil.decomp_litter_pft[i].nitrogen,stand2->soil.decomp_litter_pft[i].nitrogen,
             stand1->frac,stand2->frac);
  }
  mixpool(stand1->soil.snowpack,stand2->soil.snowpack,stand1->frac, stand2->frac);
  mixpool(stand1->soil.snowfraction,stand2->soil.snowfraction,stand1->frac,stand2->frac);
  mixpool(stand1->soil.snowheight,stand2->soil.snowheight,stand1->frac,stand2->frac);
  mixpool(stand1->soil.wa,stand2->soil.wa,stand1->frac,stand2->frac);
  mixpool(stand1->soil.snowdens,stand2->soil.snowdens,stand1->frac,stand2->frac);
  mixpool(stand1->soil.wtable,stand2->soil.wtable,stand1->frac,stand2->frac);
  mixpool(stand1->soil.rw_buffer,stand2->soil.rw_buffer,stand1->frac,stand2->frac);
  mixpool(stand1->soil.mean_maxthaw,stand2->soil.mean_maxthaw,stand1->frac,stand2->frac);
  mixpool(stand1->soil.maxthaw_depth,stand2->soil.maxthaw_depth,stand1->frac,stand2->frac);
  mixpool(stand1->soil.alag,stand2->soil.alag,stand1->frac,stand2->frac);
  mixpool(stand1->soil.amp,stand2->soil.amp,stand1->frac,stand2->frac);
  mixpool(stand1->soil.w_evap,stand2->soil.w_evap,stand1->frac,stand2->frac);


  forrootsoillayer(l)
  {
    water1 = (stand1->soil.w[l] * stand1->soil.whcs[l] + stand1->soil.ice_depth[l] + stand1->soil.w_fw[l] + stand1->soil.ice_fw[l])*stand1->frac;
    water2 = (stand2->soil.w[l] * stand2->soil.whcs[l] + stand2->soil.ice_depth[l] + stand2->soil.w_fw[l] + stand2->soil.ice_fw[l])*stand2->frac;
    if ((water1 + water2)>0)
      mixpool(stand1->frac_g[l], stand2->frac_g[l], water1, water2);
    else
      stand1->frac_g[l]=0;
    //stand1->soil.state[l]=(short)getstate(stand1->soil.temp+l);
    stand1->soil.w[l]=(stand1->soil.w[l]*stand1->soil.whcs[l]*stand1->frac+stand2->soil.w[l]*stand2->soil.whcs[l]*stand2->frac)/(stand1->frac+stand2->frac);
    //mixpool(stand1->soil.whcs[l],stand2->soil.whcs[l],stand1->frac,stand2->frac);
    stand1->soil.w[l]/=stand1->soil.whcs[l];

    mixpool(stand1->soil.w_fw[l],stand2->soil.w_fw[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.wfc[l],stand2->soil.wfc[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.whc[l],stand2->soil.whc[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.wsat[l],stand2->soil.wsat[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.wsats[l],stand2->soil.wsats[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.ice_depth[l],stand2->soil.ice_depth[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.ice_fw[l],stand2->soil.ice_fw[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.freeze_depth[l],stand2->soil.freeze_depth[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.ice_pwp[l],stand2->soil.ice_pwp[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.wpwp[l],stand2->soil.wpwp[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.wpwps[l],stand2->soil.wpwps[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.beta_soil[l], stand2->soil.beta_soil[l], stand1->frac, stand2->frac);
    mixpool(stand1->soil.bulkdens[l], stand2->soil.bulkdens[l], stand1->frac, stand2->frac);
    mixpool(stand1->soil.k_dry[l], stand2->soil.k_dry[l], stand1->frac, stand2->frac);
    mixpool(stand1->soil.Ks[l], stand2->soil.Ks[l], stand1->frac, stand2->frac);
    mixpool(stand1->soil.wi_abs_enth_adj[l],stand2->soil.wi_abs_enth_adj[l],stand1->frac,stand2->frac);
    mixpool(stand1->soil.sol_abs_enth_adj[l],stand2->soil.sol_abs_enth_adj[l],stand1->frac,stand2->frac);
/*
    absolute_water1[l] = stand1->soil.w[l] * stand1->soil.whcs[l] + stand1->soil.w_fw[l] + stand1->soil.wpwps[l] * (1 - stand1->soil.ice_pwp[l]);
    absolute_water2 = stand2->soil.w[l] * stand2->soil.whcs[l] + stand2->soil.w_fw[l] + stand2->soil.wpwps[l] * (1 - stand2->soil.ice_pwp[l]);
    mixpool(absolute_water1[l], absolute_water2, stand1->frac, stand2->frac);

    absolute_ice1[l] = stand1->soil.ice_depth[l] + stand1->soil.ice_fw[l] + stand1->soil.wpwps[l] * stand1->soil.ice_pwp[l];
    absolute_ice2 = stand2->soil.ice_depth[l] + stand2->soil.ice_fw[l] + stand2->soil.wpwps[l] * stand2->soil.ice_pwp[l];
    mixpool(absolute_ice1[l], absolute_ice2, stand1->frac, stand2->frac);
*/
  }

  for(l=0;l<NTILLLAYER;l++)
    mixpool(stand1->soil.df_tillage[l],stand2->soil.df_tillage[l],stand1->frac,stand2->frac);

  updatelitterproperties(stand1,stand1->frac+stand2->frac);

  if(config->soilpar_option==NO_FIXED_SOILPAR || (config->soilpar_option==FIXED_SOILPAR && year<config->soilpar_fixyear))
    pedotransfer(stand1,NULL,NULL,stand1->frac+stand2->frac);

  //pedotransfer(stand1,absolute_water1,absolute_ice1,stand1->frac+stand2->frac);     // DOES NOT WOTK HERE; CAUSES BALANCE ERRORS
  /* bedrock needs to be mixed seperately */

  stand1->soil.w[BOTTOMLAYER]=(stand1->soil.w[BOTTOMLAYER]*stand1->soil.whcs[BOTTOMLAYER]*stand1->frac+stand2->soil.w[BOTTOMLAYER]*stand2->soil.whcs[BOTTOMLAYER]*stand2->frac)/(stand1->frac+stand2->frac);
  //mixpool(stand1->soil.whcs[BOTTOMLAYER],stand2->soil.whcs[BOTTOMLAYER],stand1->frac,stand2->frac);
  stand1->soil.w[BOTTOMLAYER]/=stand1->soil.whcs[BOTTOMLAYER];
  //mixpool(stand1->soil.w[BOTTOMLAYER],stand2->soil.w[BOTTOMLAYER],stand1->frac,stand2->frac);
  mixpool(stand1->soil.w_fw[BOTTOMLAYER],stand2->soil.w_fw[BOTTOMLAYER],stand1->frac,stand2->frac);
  mixpool(stand1->soil.wsat[BOTTOMLAYER],stand2->soil.wsat[BOTTOMLAYER],stand1->frac,stand2->frac);
  mixpool(stand1->soil.wsats[BOTTOMLAYER],stand2->soil.wsats[BOTTOMLAYER],stand1->frac,stand2->frac);
  mixpool(stand1->soil.ice_depth[BOTTOMLAYER],stand2->soil.ice_depth[BOTTOMLAYER],stand1->frac,stand2->frac);
  mixpool(stand1->soil.ice_fw[BOTTOMLAYER],stand2->soil.ice_fw[BOTTOMLAYER],stand1->frac,stand2->frac);
  mixpool(stand1->soil.freeze_depth[BOTTOMLAYER],stand2->soil.freeze_depth[BOTTOMLAYER],stand1->frac,stand2->frac);
  mixpool(stand1->soil.ice_pwp[BOTTOMLAYER],stand2->soil.ice_pwp[BOTTOMLAYER],stand1->frac,stand2->frac);
  mixpool(stand1->soil.wfc[BOTTOMLAYER], stand2->soil.wfc[BOTTOMLAYER], stand1->frac, stand2->frac);
  mixpool(stand1->soil.whc[BOTTOMLAYER], stand2->soil.whc[BOTTOMLAYER], stand1->frac, stand2->frac);
  mixpool(stand1->soil.wpwp[BOTTOMLAYER],stand2->soil.wpwp[BOTTOMLAYER],stand1->frac,stand2->frac);
  mixpool(stand1->soil.wpwps[BOTTOMLAYER], stand2->soil.wpwps[BOTTOMLAYER], stand1->frac, stand2->frac);
  mixpool(stand1->soil.wi_abs_enth_adj[BOTTOMLAYER], stand2->soil.wi_abs_enth_adj[BOTTOMLAYER], stand1->frac, stand2->frac);
  mixpool(stand1->soil.sol_abs_enth_adj[BOTTOMLAYER], stand2->soil.sol_abs_enth_adj[BOTTOMLAYER], stand1->frac, stand2->frac);
  mixpool(stand1->soil.beta_soil[BOTTOMLAYER], stand2->soil.beta_soil[BOTTOMLAYER], stand1->frac, stand2->frac);
  mixpool(stand1->soil.bulkdens[BOTTOMLAYER], stand2->soil.bulkdens[BOTTOMLAYER], stand1->frac, stand2->frac);
  mixpool(stand1->soil.k_dry[BOTTOMLAYER], stand2->soil.k_dry[BOTTOMLAYER], stand1->frac, stand2->frac);
  mixpool(stand1->soil.Ks[BOTTOMLAYER], stand2->soil.Ks[BOTTOMLAYER], stand1->frac, stand2->frac);
  mixpool(stand1->soil.mean_maxthaw,stand2->soil.mean_maxthaw,stand1->frac,stand2->frac);
  mixpool(stand1->soil.alag,stand2->soil.alag,stand1->frac,stand2->frac);
  mixpool(stand1->soil.amp,stand2->soil.amp,stand1->frac,stand2->frac);
  mixpool(stand1->soil.rw_buffer,stand2->soil.rw_buffer,stand1->frac,stand2->frac);
  mixsoilenergy(stand1,stand2,config);
#ifdef CHECK_BALANCE
  Real water_f=0;
  foreachsoillayer(l)
  {
    water_f+=(stand1->soil.w[l] * stand1->soil.whcs[l])*(stand1->frac+stand2->frac);
  }
 water_after=soilwater(&stand1->soil)*(stand1->frac+stand2->frac)+stand1->cell->balance.excess_water;
  if(fabs(water_before-water_after)>0.001)
  {
    fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,
         "Invalid water balance in %s: e=%g water_before=%g water_after=%g balance.excess_water: %g excess_water: %g stand1: %s stand2: %s water_f: %g wa1: %g  wa2: %g  in mixsoil()",
         __FUNCTION__,fabs(water_before-water_after),water_before,water_after,stand1->cell->balance.excess_water,excess_water, stand1->type->name,stand2->type->name,water_f,water_w1,water_w2);
    fflush(stderr);
  }
#endif
} /* of 'mixsoil' */

void mixsoilenergy(Stand *stand1, const Stand *stand2, const Config *config)
{
  int l;
  for(l=0;l<NHEATGRIDP;++l)
    mixpool(stand1->soil.enth[l],stand2->soil.enth[l],stand1->frac,stand2->frac);
  /* update soil temps */
  Soil_thermal_prop therm;
  calc_soil_thermal_props(UNKNOWN, &therm, &(stand1->soil), NULL, NULL ,config->johansen, FALSE);
  compute_mean_layer_temps_from_enth(stand1->soil.temp,stand1->soil.enth,&therm);
} /* of 'mixsoilenergy' */

void mixsetaside(Stand *setasidestand,Stand *cropstand,Bool intercrop,int year,int ntotpft,const Config *config)
{
  /*assumes that all vegetation carbon pools are zero after harvest*/
  int p, p2;
  Pft *pft, *pft2;
  Pftgrass *grass, *grass2;
  Bool found;
#ifdef CHECK_BALANCE
  Stand *stand;
  Stocks start = {0,0};
  Stocks end = {0,0};
  Stocks st;
  Real start_w = cropstand->cell->balance.excess_water;
  Real end_w = 0;
  int s;
  foreachstand(stand, s, cropstand->cell->standlist)
  {
    st=standstocks(stand);
    start.carbon+=(st.carbon+ soilmethane(&stand->soil)*WC/WCH4)*stand->frac;//-stand->cell->balance.flux_estab.carbon;
    start.nitrogen+=st.nitrogen*stand->frac;//-stand->cell->balance.flux_estab.nitrogen;
    start_w += soilwater(&stand->soil)*stand->frac;
  }
#endif
  mixsoil(setasidestand, cropstand, year, ntotpft,config);
  setasidestand->slope_mean=(setasidestand->slope_mean*setasidestand->frac+cropstand->slope_mean*cropstand->frac)/(setasidestand->frac+cropstand->frac);
  setasidestand->Hag_Beta=min(1,(0.06*log(setasidestand->slope_mean+0.1)+0.22)/0.43);

  if(intercrop)
  {
    if (isempty(&cropstand->pftlist)) /* should not happen as establishment of cover crops now happens on all stands after tillage */
    {
    foreachpft(pft,p,&setasidestand->pftlist)
      mix_veg(pft,setasidestand->frac/(setasidestand->frac+cropstand->frac));
  }
    else
    {
      foreachpft(pft, p, &setasidestand->pftlist)
      {
        if(pft->par->type==GRASS)
        {
          found=FALSE;
          grass = pft->data;
          foreachpft(pft2, p2, &cropstand->pftlist)
          {
            if (pft->par->id == pft2->par->id)
            {
              found=TRUE;
              grass2 = pft2->data;
              grass->ind.leaf.carbon = weightedaverage(grass->ind.leaf.carbon, grass2->ind.leaf.carbon, setasidestand->frac, cropstand->frac);
              grass->ind.root.carbon = weightedaverage(grass->ind.root.carbon, grass2->ind.root.carbon, setasidestand->frac, cropstand->frac);
              grass->turn.root.carbon = weightedaverage(grass->turn.root.carbon, grass2->turn.root.carbon, setasidestand->frac, cropstand->frac);
              grass->turn.leaf.carbon = weightedaverage(grass->turn.leaf.carbon, grass2->turn.leaf.carbon, setasidestand->frac, cropstand->frac);
              grass->turn_litt.root.carbon = weightedaverage(grass->turn_litt.root.carbon, grass2->turn_litt.root.carbon, setasidestand->frac, cropstand->frac);
              grass->turn_litt.leaf.carbon = weightedaverage(grass->turn_litt.leaf.carbon, grass2->turn_litt.leaf.carbon, setasidestand->frac, cropstand->frac);
              grass->ind.leaf.nitrogen = weightedaverage(grass->ind.leaf.nitrogen, grass2->ind.leaf.nitrogen, setasidestand->frac, cropstand->frac);
              grass->ind.root.nitrogen = weightedaverage(grass->ind.root.nitrogen, grass2->ind.root.nitrogen, setasidestand->frac, cropstand->frac);
              grass->turn.root.nitrogen = weightedaverage(grass->turn.root.nitrogen, grass2->turn.root.nitrogen, setasidestand->frac, cropstand->frac);
              grass->turn.leaf.nitrogen = weightedaverage(grass->turn.leaf.nitrogen, grass2->turn.leaf.nitrogen, setasidestand->frac, cropstand->frac);
              grass->turn_litt.root.nitrogen = weightedaverage(grass->turn_litt.root.nitrogen, grass2->turn_litt.root.nitrogen, setasidestand->frac, cropstand->frac);
              grass->turn_litt.leaf.nitrogen = weightedaverage(grass->turn_litt.leaf.nitrogen, grass2->turn_litt.leaf.nitrogen, setasidestand->frac, cropstand->frac);
              grass->excess_carbon = weightedaverage(grass->excess_carbon, grass2->excess_carbon, setasidestand->frac, cropstand->frac);
              pft->nleaf = weightedaverage(pft->nleaf, pft2->nleaf, setasidestand->frac, cropstand->frac);
              pft->bm_inc.carbon = weightedaverage(pft->bm_inc.carbon, pft2->bm_inc.carbon, setasidestand->frac, cropstand->frac);
              pft->bm_inc.nitrogen = weightedaverage(pft->bm_inc.nitrogen, pft2->bm_inc.nitrogen, setasidestand->frac, cropstand->frac);
              pft->establish.carbon = weightedaverage(pft->establish.carbon, pft2->establish.carbon, setasidestand->frac, cropstand->frac);
              pft->establish.nitrogen = weightedaverage(pft->establish.nitrogen, pft2->establish.nitrogen, setasidestand->frac, cropstand->frac);
              delpft(&cropstand->pftlist, p2);
              break;
            }
          }
          if(!found)
          {
            grass->ind.leaf.carbon = weightedaverage(grass->ind.leaf.carbon, 0, setasidestand->frac, cropstand->frac);
            grass->ind.root.carbon = weightedaverage(grass->ind.root.carbon, 0, setasidestand->frac, cropstand->frac);
            grass->turn.root.carbon = weightedaverage(grass->turn.root.carbon, 0, setasidestand->frac, cropstand->frac);
            grass->turn.leaf.carbon = weightedaverage(grass->turn.leaf.carbon, 0, setasidestand->frac, cropstand->frac);
            grass->turn_litt.root.carbon = weightedaverage(grass->turn_litt.root.carbon, 0, setasidestand->frac, cropstand->frac);
            grass->turn_litt.leaf.carbon = weightedaverage(grass->turn_litt.leaf.carbon, 0, setasidestand->frac, cropstand->frac);
            grass->ind.leaf.nitrogen = weightedaverage(grass->ind.leaf.nitrogen, 0, setasidestand->frac, cropstand->frac);
            grass->ind.root.nitrogen = weightedaverage(grass->ind.root.nitrogen, 0, setasidestand->frac, cropstand->frac);
            grass->turn.root.nitrogen = weightedaverage(grass->turn.root.nitrogen, 0, setasidestand->frac, cropstand->frac);
            grass->turn.leaf.nitrogen = weightedaverage(grass->turn.leaf.nitrogen, 0, setasidestand->frac, cropstand->frac);
            grass->turn_litt.root.nitrogen = weightedaverage(grass->turn_litt.root.nitrogen, 0, setasidestand->frac, cropstand->frac);
            grass->turn_litt.leaf.nitrogen = weightedaverage(grass->turn_litt.leaf.nitrogen, 0, setasidestand->frac, cropstand->frac);
            grass->excess_carbon = weightedaverage(grass->excess_carbon, 0, setasidestand->frac, cropstand->frac);
            pft->nleaf = weightedaverage(pft->nleaf, 0, setasidestand->frac, cropstand->frac);
            pft->bm_inc.carbon = weightedaverage(pft->bm_inc.carbon, 0, setasidestand->frac, cropstand->frac);
            pft->bm_inc.nitrogen = weightedaverage(pft->bm_inc.nitrogen, 0, setasidestand->frac, cropstand->frac);
            pft->establish.carbon = weightedaverage(pft->establish.carbon, 0, setasidestand->frac, cropstand->frac);
            pft->establish.nitrogen = weightedaverage(pft->establish.nitrogen, 0, setasidestand->frac, cropstand->frac);
          }
        }
      }
      foreachpft(pft, p, &cropstand->pftlist)
      {
        if(pft->par->type==GRASS)
        {
          grass = pft->data;
          pft2=addpft(setasidestand, pft->par, year, 0, config);
          grass2=pft2->data;
          grass2->ind.leaf.carbon = weightedaverage(grass->ind.leaf.carbon, 0, cropstand->frac, setasidestand->frac);
          grass2->ind.root.carbon = weightedaverage(grass->ind.root.carbon, 0, cropstand->frac, setasidestand->frac);
          grass2->turn.root.carbon = weightedaverage(grass->turn.root.carbon, 0, cropstand->frac, setasidestand->frac);
          grass2->turn.leaf.carbon = weightedaverage(grass->turn.leaf.carbon, 0, cropstand->frac, setasidestand->frac);
          grass2->turn_litt.root.carbon = weightedaverage(grass->turn_litt.root.carbon, 0, cropstand->frac, setasidestand->frac);
          grass2->turn_litt.leaf.carbon = weightedaverage(grass->turn_litt.leaf.carbon, 0, cropstand->frac, setasidestand->frac);
          grass2->ind.leaf.nitrogen = weightedaverage(grass->ind.leaf.nitrogen, 0, cropstand->frac, setasidestand->frac);
          grass2->ind.root.nitrogen = weightedaverage(grass->ind.root.nitrogen, 0, cropstand->frac, setasidestand->frac);
          grass2->turn.root.nitrogen = weightedaverage(grass->turn.root.nitrogen, 0, cropstand->frac, setasidestand->frac);
          grass2->turn.leaf.nitrogen = weightedaverage(grass->turn.leaf.nitrogen, 0, cropstand->frac, setasidestand->frac);
          grass2->turn_litt.root.nitrogen = weightedaverage(grass->turn_litt.root.nitrogen, 0, cropstand->frac, setasidestand->frac);
          grass2->turn_litt.leaf.nitrogen = weightedaverage(grass->turn_litt.leaf.nitrogen, 0, cropstand->frac, setasidestand->frac);
          grass2->excess_carbon = weightedaverage(grass->excess_carbon, 0, cropstand->frac,setasidestand->frac);
          pft2->nleaf = weightedaverage(pft->nleaf, 0, cropstand->frac, setasidestand->frac);
          pft2->bm_inc.carbon = weightedaverage(pft->bm_inc.carbon, 0, cropstand->frac, setasidestand->frac);
          pft2->establish.carbon = weightedaverage(pft->establish.carbon, 0, cropstand->frac, setasidestand->frac);
          pft2->bm_inc.nitrogen = weightedaverage(pft->bm_inc.nitrogen, 0, cropstand->frac, setasidestand->frac);
          pft2->establish.nitrogen = weightedaverage(pft->establish.nitrogen, 0, cropstand->frac, setasidestand->frac);
          delpft(&cropstand->pftlist, p);
          p--; /* adjust loop variable */
        }
      }

    }
  }
  setasidestand->frac+=cropstand->frac;
  cropstand->frac=0;
#ifdef CHECK_BALANCE
  end.carbon=end.nitrogen = 0;
  end_w=cropstand->cell->balance.excess_water;
  foreachstand(stand, s, cropstand->cell->standlist)
  {
    st=standstocks(stand);
    end.carbon+=(st.carbon+ soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
    end.nitrogen+=st.nitrogen*stand->frac;
    end_w+= soilwater(&stand->soil)*stand->frac;
  }
  if(fabs(start.carbon-end.carbon)>0.001)
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid carbon balance in %s mixsetaside at the end: C_ERROR=%g start : %g end : %g",
         __FUNCTION__,start.carbon-end.carbon,start.carbon,end.carbon);
  if (fabs(start_w - end_w)>0.001)
    fail(INVALID_WATER_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid water balance in %s mixsetaside at the end: W_ERROR=%g start : %g end : %g",
         __FUNCTION__,start_w - end_w, start_w, end_w);
  if (fabs(start.nitrogen-end.nitrogen)>0.001)
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE,"Invalid nitrogen balance in %s mixsetaside at the end: error=%g start : %g end : %g",
         __FUNCTION__, start.nitrogen-end.nitrogen,start.nitrogen,end.nitrogen);
#endif
} /* of 'mixsetaside' */

Bool setaside(Cell *cell,          /**< Pointer to LPJ cell */
              Stand *cropstand,    /**< crop stand */
              Bool with_tillage,   /**< tillage (TRUE/FALSE) */
              Bool intercrop,      /**< intercropping possible (TRUE/FALSE) */
              int npft,            /**< number of natural PFTs */
              int ncft,              /**< number of crop PFTs */
              Bool irrig,          /**< irrigated stand (TRUE/FALSE) */
              Bool iswetland,
              int year,            /**< simulation year */
              const Config *config /**< LPJmL configuration */
             )                     /** \return stand has to be killed (TRUE/FALSE) */
{
  int s,p,n_est;
  Pft *pft;
  Stocks flux_estab,stocks;
  Irrigation *data;
  String line;

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
#endif
  if(cropstand<0)
    fail(NEGATIVE_STAND_FRAC_ERR,TRUE,TRUE,"setaside: Negative crop stand frac =%g in cell (%s) before update",cropstand->frac,sprintcoord(line,&cell->coord));
  /* call tillage before */
  if(with_tillage)
    tillage(&cropstand->soil,param.residue_frac);

#ifdef SAFE
  if (!isempty(&cropstand->pftlist))
    fail(LIST_NOT_EMPTY_ERR,TRUE, TRUE, "Pftlist is not empty in setaside().");
#endif
  if (intercrop)
  {
    n_est = 0;
    for (p = 0; p < npft; p++)
    {
      if(establish(cell->gdd[p],config->pftpar+p,&cell->climbuf,cropstand->type->landusetype==WETLAND || cropstand->type->landusetype==SETASIDE_WETLAND) &&
         config->pftpar[p].type==GRASS && config->pftpar[p].cultivation_type==NONE && p!=Sphagnum_moss)
      {
        addpft(cropstand,config->pftpar+p,year,0,config);
        n_est++;
      }
    }
    flux_estab.carbon = flux_estab.nitrogen = 0.0;
    foreachpft(pft, p, &cropstand->pftlist)
    {
      stocks = establishment(pft, 0, 0, n_est);
      //flux_estab.carbon += stocks.carbon;
      //flux_estab.nitrogen += stocks.nitrogen;
      /* to avoid artificial fertilization of setaside stands with small grass saplings planted as cover crops
         instead of sowing seeds, we take the biomass for the cover crop sapling from the litter pools */
      
      cropstand->soil.litter.item->agtop.leaf.carbon -= stocks.carbon;
      if (cropstand->soil.litter.item->agtop.leaf.carbon < 0)
      {
        cropstand->soil.litter.item->agsub.leaf.carbon += cropstand->soil.litter.item->agtop.leaf.carbon;
        cropstand->soil.litter.item->agtop.leaf.carbon = 0;
        if (cropstand->soil.litter.item->agsub.leaf.carbon < 0)
        {
          flux_estab.carbon -= cropstand->soil.litter.item->agsub.leaf.carbon;
          cropstand->soil.litter.item->agsub.leaf.carbon = 0;
        }
      }
      cropstand->soil.litter.item->agtop.leaf.nitrogen -= stocks.nitrogen;
      if (cropstand->soil.litter.item->agtop.leaf.nitrogen < 0)
      {
        cropstand->soil.litter.item->agsub.leaf.nitrogen += cropstand->soil.litter.item->agtop.leaf.nitrogen;
        cropstand->soil.litter.item->agtop.leaf.nitrogen = 0;
        if (cropstand->soil.litter.item->agsub.leaf.nitrogen < 0)
        {
          flux_estab.nitrogen -= cropstand->soil.litter.item->agsub.leaf.nitrogen;
          cropstand->soil.litter.item->agsub.leaf.nitrogen = 0;
        }
      }
    }
    getoutput(&cell->output,FLUX_ESTABC,config)+=flux_estab.carbon*cropstand->frac;
    getoutput(&cell->output,FLUX_ESTABN,config)+=flux_estab.nitrogen*cropstand->frac;
    getoutput(&cell->output,FLUX_ESTABN_MG,config)+=flux_estab.nitrogen*cropstand->frac;
    cell->balance.flux_estab.carbon+=flux_estab.carbon*cropstand->frac;
    cell->balance.flux_estab.nitrogen+=flux_estab.nitrogen*cropstand->frac;

  }

  if(iswetland)
    s=findlandusetype(cell->standlist,SETASIDE_WETLAND);
  else
    s=findlandusetype(cell->standlist,irrig? SETASIDE_IR : SETASIDE_RF);

  if(s!=NOT_FOUND)
  {
    mixsetaside(getstand(cell->standlist,s),cropstand,intercrop,year,npft+ncft,config);
    return TRUE;
  }
  else
  {
    cropstand->type->freestand(cropstand);
    if(iswetland)
      cropstand->type=&setaside_wetland_stand;
    else
      cropstand->type= irrig? &setaside_ir_stand : &setaside_rf_stand;
    cropstand->type->newstand(cropstand);
    data=cropstand->data;
    data->irrigation= irrig;
    cropstand->soil.iswetland=iswetland;
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
      -((cell->balance.deforest_emissions.carbon+cell->balance.prod_turnover.fast.carbon+cell->balance.prod_turnover.slow.carbon+cell->balance.trad_biofuel.carbon)-fluxes_prod.carbon);
  balance.nitrogen=(cell->balance.flux_estab.nitrogen-fluxes_estab.nitrogen)-(cell->balance.fire.nitrogen-fluxes_fire.nitrogen)-(cell->balance.neg_fluxes.nitrogen-fluxes_neg.nitrogen)
      -((cell->balance.deforest_emissions.nitrogen+cell->balance.prod_turnover.fast.nitrogen+cell->balance.prod_turnover.slow.nitrogen+cell->balance.trad_biofuel.nitrogen)-fluxes_prod.nitrogen);
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
  return FALSE;
} /* of 'setaside' */

/*
- called in landusechange(), update_daily()
- checks if a set-aside stand already exists
  -> if TRUE: call of local function mixsetaside()
  -> if FALSE: set stand variable landusetype to SETASIDE
       if intercropping is possible:
       -> check if pft of type GRASS could be establish (see
          establish.c)
          -> if TRUE: add pft to the pftlist and call specific
             function establishment() (see pft.h)

mixsetaside()

- mixes certain variables of structure soil of the set-aside stand with the
  considered crop stand
  -> calls local function mixpool() (see landuse.h)
- if already intercropping on set-aside stand reduce the leafs & roots dependent
  on the fraction of the crop stand
  -> calls specific function mix_veg() (see pft.h,mix_veg_grass.c,mix_veg_tree.c)
- adds the land fractions of the crop stand to the set-aside stand
- deletes considered crop stand (see delstand() in standlist.c)
*/

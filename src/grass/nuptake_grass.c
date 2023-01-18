/**************************************************************************************/
/**                                                                                \n**/
/**             n  u  p  t  a  k  e  _  g  r  a  s  s  .  c                        \n**/
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
#include "agriculture.h"

Real nuptake_grass(Pft *pft,             /**< pointer to PFT data */
                   Real *n_plant_demand, /**< total N plant demand */
                   Real *ndemand_leaf,   /**< N demand of leafs */
                   int npft,             /**< number of natural PFTs */
                   int ncft,             /**< number of crop PFTs */
                   const Config *config  /**< LPJmL configurtation */
                  )                      /** \return nitrogen uptake (gN/m2/day) */
{
  Soil *soil;
  Pftgrass *grass;
  Pftgrasspar *grasspar;
  Real NO3_up=0;
  Real NCplant,ndemand_leaf_opt,NC_actual,NC_leaf,ndemand_all;
  Real f_NCplant;
  Real up_temp_f;
  Real totn,nsum;
  Real wscaler;
  Real autofert_n;
  Real n_uptake=0;
  Real n_upfail=0; /**< track n_uptake that is not available from soil for output reporting */
  Real rootdist_n[LASTLAYER];
  Real n_deficit=0.0;
  Real n_fixed=0.0;
  Irrigation *data;
  int l,nnat,nirrig;
  ndemand_all=*n_plant_demand;
  soil=&pft->stand->soil;
  if(config->permafrost)
    getrootdist(rootdist_n,pft->par->rootdist,soil->mean_maxthaw);
  else
    forrootsoillayer(l)
      rootdist_n[l]=pft->par->rootdist[l];
  data=pft->stand->data;
  grass=pft->data;
  grasspar=pft->par->data;
  nnat=getnnat(npft,config);
  nirrig=getnirrig(ncft,config);

  ndemand_leaf_opt=*ndemand_leaf;

  NCplant = (grass->ind.leaf.nitrogen+ grass->ind.root.nitrogen) / (grass->ind.leaf.carbon+ grass->ind.root.carbon); /* Plant's mobile nitrogen concentration, Eq.9, Zaehle&Friend 2010 Supplementary */
  NC_leaf=(grass->ind.leaf.nitrogen-grass->turn.leaf.nitrogen+pft->bm_inc.nitrogen*grass->falloc.leaf)/(grass->ind.leaf.carbon-grass->turn.leaf.carbon+pft->bm_inc.carbon*grass->falloc.leaf);

  f_NCplant = min(max(((NC_leaf-pft->par->ncleaf.high)/(pft->par->ncleaf.low-pft->par->ncleaf.high)),0),1);
  /* reducing uptake according to availability */
  nsum=0;
  if(NC_leaf<(pft->par->ncleaf.high*(1+pft->par->knstore)))
    forrootsoillayer(l)
    {
      wscaler=soil->w[l]>epsilon ? 1 : 0;
      totn=(soil->NO3[l]+soil->NH4[l])*wscaler;
      if(totn>0)
      {
        /*Thornley 1991*/
        up_temp_f = nuptake_temp_fcn(soil->temp[l]);
        NO3_up = 2*pft->par->vmax_up*(pft->par->kNmin+totn/(totn+pft->par->KNmin*soil->wsat[l]*soildepth[l]/1000))* up_temp_f*
            f_NCplant * (grass->ind.root.carbon*pft->nind+pft->bm_inc.carbon*grass->falloc.root-grass->turn_litt.root.nitrogen)*rootdist_n[l]/1000;
        /* reducing uptake according to availability */
        if(NO3_up>totn)
          NO3_up=totn;
        n_uptake+=NO3_up;
        nsum+=totn*rootdist_n[l];
      }
    }
  if(nsum<epsilon)  //nsum==0
    n_uptake=0;
  else
  {
    if (n_uptake>*n_plant_demand-(vegn_sum_grass(pft)-grass->turn_litt.root.nitrogen-grass->turn_litt.leaf.nitrogen+pft->bm_inc.nitrogen))
      n_uptake=*n_plant_demand-(vegn_sum_grass(pft)-grass->turn_litt.root.nitrogen-grass->turn_litt.leaf.nitrogen+pft->bm_inc.nitrogen);
    if(n_uptake<=0)
      n_uptake=0;
    else
    {
      pft->bm_inc.nitrogen+=n_uptake;
      forrootsoillayer(l)
      {
        wscaler=soil->w[l]>epsilon ? 1 : 0;
        soil->NO3[l]-=soil->NO3[l]*wscaler*rootdist_n[l]*n_uptake/nsum;
        if(soil->NO3[l]<0)
        {
          pft->bm_inc.nitrogen+=soil->NO3[l];
          n_upfail+=soil->NO3[l];
          soil->NO3[l]=0;
        }

        soil->NH4[l]-=soil->NH4[l]*wscaler*rootdist_n[l]*n_uptake/nsum;
        if(soil->NH4[l]<0)
        {
          pft->bm_inc.nitrogen+=soil->NH4[l];
          n_upfail+=soil->NH4[l];
          soil->NH4[l]=0;
        }
      }
    }
  }
  if(config->fertilizer_input==AUTO_FERTILIZER && (pft->stand->type->landusetype==GRASSLAND || pft->stand->type->landusetype==BIOMASS_GRASS || pft->stand->type->landusetype==AGRICULTURE_GRASS))
  {
    data=pft->stand->data;
    autofert_n=*n_plant_demand-(vegn_sum_grass(pft)+pft->bm_inc.nitrogen);
    n_uptake += autofert_n;
    pft->bm_inc.nitrogen += autofert_n;
    pft->vscal+=1;
    pft->stand->cell->balance.n_influx += autofert_n*pft->stand->frac;
    getoutput(&pft->stand->cell->output,FLUX_AUTOFERT,config)+=autofert_n*pft->stand->frac;
    switch(pft->stand->type->landusetype)
    {
    case GRASSLAND:
      getoutputindex(&pft->stand->cell->output,CFT_NFERT,rothers(ncft)+data->irrigation*nirrig,config)+=autofert_n;
      getoutputindex(&pft->stand->cell->output,CFT_NFERT,rmgrass(ncft)+data->irrigation*nirrig,config)+=autofert_n;
      break;
    case BIOMASS_GRASS:
      getoutputindex(&pft->stand->cell->output,CFT_NFERT,rbgrass(ncft)+data->irrigation*nirrig,config)+=autofert_n;
      break;
    case AGRICULTURE_GRASS:
      getoutputindex(&pft->stand->cell->output,CFT_NFERT,data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=autofert_n;
      break;
    }
  }
  else
  {
    n_deficit = *n_plant_demand/(1+pft->par->knstore)-(vegn_sum_grass(pft)-grass->turn_litt.root.nitrogen-grass->turn_litt.leaf.nitrogen+pft->bm_inc.nitrogen);
    if(n_deficit>0 && pft->npp_bnf>0)
    {
       n_fixed=ma_biological_n_fixation(pft, soil, n_deficit, config);
       pft->bm_inc.nitrogen+=n_fixed;
       getoutput(&pft->stand->cell->output,BNF,config)+=n_fixed*pft->stand->frac;
       pft->stand->cell->balance.n_influx+=n_fixed*pft->stand->frac;
    }
    else
      pft->npp_bnf=0.0;
    if(*n_plant_demand/(1+pft->par->knstore)>(vegn_sum_grass(pft)-grass->turn_litt.root.nitrogen-grass->turn_litt.leaf.nitrogen+pft->bm_inc.nitrogen))
    {
      NC_actual=(vegn_sum_grass(pft)+pft->bm_inc.nitrogen)/(vegc_sum_grass(pft)+pft->bm_inc.carbon);
      NC_leaf=(grass->ind.leaf.nitrogen-grass->turn.leaf.nitrogen+pft->bm_inc.nitrogen*grass->falloc.leaf/pft->nind)/(grass->ind.leaf.carbon-grass->turn.leaf.carbon+pft->bm_inc.carbon*grass->falloc.leaf/pft->nind);
      if(NC_leaf< pft->par->ncleaf.low)
        NC_leaf=pft->par->ncleaf.low;
      else if (NC_leaf>pft->par->ncleaf.high)
        NC_leaf=pft->par->ncleaf.high;
      *ndemand_leaf=grass->ind.leaf.nitrogen*pft->nind+pft->bm_inc.nitrogen*grass->falloc.leaf-grass->turn_litt.leaf.nitrogen;
      *ndemand_leaf=max(grass->ind.leaf.nitrogen*pft->nind-grass->turn_litt.leaf.nitrogen,*ndemand_leaf);
      *n_plant_demand=*ndemand_leaf+(grass->ind.root.nitrogen-grass->turn.root.nitrogen)*pft->nind+NC_leaf*(grass->excess_carbon*pft->nind+pft->bm_inc.carbon)*(grass->falloc.root/grasspar->ratio);
   }
  }
  if(ndemand_leaf_opt<epsilon)
    pft->vscal+=1;
  else
    pft->vscal+=min(1,*ndemand_leaf/(ndemand_leaf_opt/(1+pft->par->knstore)));
  /* correcting for failed uptake from depleted soils in outputs */
  n_uptake+=n_upfail;
  switch(pft->stand->type->landusetype)
  {
  case NATURAL: case SETASIDE_RF: case SETASIDE_IR: case WETLAND: case SETASIDE_WETLAND:
    getoutputindex(&pft->stand->cell->output,PFT_NUPTAKE,pft->par->id,config)+=n_uptake;
    getoutputindex(&pft->stand->cell->output,PFT_NDEMAND,pft->par->id,config)+=max(0,ndemand_all-(vegn_sum_grass(pft)+pft->bm_inc.carbon))/365;
    break;
  case BIOMASS_GRASS:
    getoutputindex(&pft->stand->cell->output,PFT_NUPTAKE,nnat+rbgrass(ncft)+data->irrigation*nirrig,config)+=n_uptake; /* *stand->cell->ml.landfrac[data->irrigation].biomass_grass; */
    getoutputindex(&pft->stand->cell->output,PFT_NDEMAND,nnat+rbgrass(ncft)+data->irrigation*nirrig,config)+=max(0,ndemand_all-(vegn_sum_grass(pft)-grass->turn_litt.root.nitrogen-grass->turn_litt.leaf.nitrogen+pft->bm_inc.carbon))/365; /* *stand->cell->ml.landfrac[data->irrigation].biomass_grass; */
    break;
  case AGRICULTURE_GRASS:
    getoutputindex(&pft->stand->cell->output,PFT_NUPTAKE,nnat+data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=n_uptake; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
    getoutputindex(&pft->stand->cell->output,PFT_NDEMAND,nnat+data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=max(0,ndemand_all-(vegn_sum_grass(pft)-grass->turn_litt.root.nitrogen-grass->turn_litt.leaf.nitrogen+pft->bm_inc.carbon))/365; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
    break;
  case GRASSLAND:
    getoutputindex(&pft->stand->cell->output,PFT_NUPTAKE,nnat+rothers(ncft)+data->irrigation*nirrig,config)+=n_uptake;/*pft->stand->cell->ml.landfrac[data->irrigation].grass[0];*/
    getoutputindex(&pft->stand->cell->output,PFT_NUPTAKE,nnat+rmgrass(ncft)+data->irrigation*nirrig,config)+=n_uptake;/*pft->stand->cell->ml.landfrac[data->irrigation].grass[1];*/
    getoutputindex(&pft->stand->cell->output,PFT_NDEMAND,nnat+rothers(ncft)+data->irrigation*nirrig,config)+=max(0,ndemand_all-(vegn_sum_grass(pft)+pft->bm_inc.carbon))/365;/*pft->stand->cell->ml.landfrac[data->irrigation].grass[0];*/
    getoutputindex(&pft->stand->cell->output,PFT_NDEMAND,nnat+rmgrass(ncft)+data->irrigation*nirrig,config)+=max(0,ndemand_all-(vegn_sum_grass(pft)+pft->bm_inc.carbon))/365;/*pft->stand->cell->ml.landfrac[data->irrigation].grass[1];*/
    break;
  } /* of 'switch' */
  pft->stand->cell->balance.n_uptake+=n_uptake*pft->stand->frac;
  pft->stand->cell->balance.n_demand+=max(0,(ndemand_all-(vegn_sum_grass(pft)-grass->turn_litt.root.nitrogen-grass->turn_litt.leaf.nitrogen+pft->bm_inc.nitrogen)))*pft->stand->frac/365;
  return n_uptake;
} /* of 'nuptake_grass' */

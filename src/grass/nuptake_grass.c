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
  //Pftgrasspar *grasspar;
  Real NO3_up[LASTLAYER], NH4_up[LASTLAYER];
  Real NCplant,ndemand_leaf_opt,NC_leaf,ndemand_all,ndemand_actual,ndemand_nh4,ndemand_no3;
  Real f_NCplant=0;
  Real up_temp_f[LASTLAYER];
  Real totn=0,totnh4=0,totno3=0,nh4sum=0,no3sum=0;
  Real wscaler;
  Real autofert_n;
  Real nh4_uptake=0, no3_uptake=0;
  Real nh4_upfail=0, no3_upfail=0; /**< track n_uptake that is not available from soil for output reporting */
  Real rootdist_n[LASTLAYER];
  Real rootdist_total=0.0;
  Real n_deficit=0.0;
  Real n_fixed=0.0;
  Irrigation *data;
  int l,nnat,nirrig;
  Real nh4_update_reduction=0.0, no3_update_reduction=0.0; /* scaling update down if demand is lower than uptake potential */
#ifdef DEBUG_N_UPTAKE
  Real NO3_before[LASTLAYER], NO3_after[LASTLAYER], NH4_before[LASTLAYER], NH4_after[LASTLAYER];
  Real vegn_before, vegn_after, nh4before=0, nh4after=0, no3before=0, no3after=0;
  Real bm_incn_before, bm_incn_after;
#endif
  ndemand_all=*n_plant_demand;
  soil=&pft->stand->soil;
  if(config->permafrost)
    getrootdist(rootdist_n,pft->par->rootdist,soil->mean_maxthaw);
  else
    forrootsoillayer(l)
      rootdist_n[l]=pft->par->rootdist[l];
  /* compute root distribution that is relevant for N uptake by ignoring frozen/totally dry layers */
  forrootsoillayer(l){
    wscaler=soil->w[l]>epsilon ? 1 : 0;
    rootdist_total+=rootdist_n[l]*wscaler;
  }
  if(rootdist_total>epsilon){
    forrootsoillayer(l){
      rootdist_n[l]/=rootdist_total;
    }
  }
  else{
    forrootsoillayer(l)
      rootdist_n[l]=0.0;
  }
  data=pft->stand->data;
  grass=pft->data;
  //grasspar=pft->par->data;
  nnat=getnnat(npft,config);
  nirrig=getnirrig(ncft,config);

  ndemand_leaf_opt=*ndemand_leaf;


  /* compute N demand by the plant */
  ndemand_actual=*n_plant_demand-(vegn_sum_grass(pft)+pft->bm_inc.nitrogen);/* *n_plant_demand includes extra demand knstore */
  if(ndemand_actual<0)
    return(0.0); /* no N demand, return 0 */
  if((grass->ind.leaf.carbon-grass->turn.leaf.carbon+pft->bm_inc.carbon*grass->falloc.leaf/pft->nind)==0)
    NC_leaf=pft->par->ncleaf.low;
  else
    NC_leaf=(grass->ind.leaf.nitrogen-grass->turn.leaf.nitrogen+pft->bm_inc.nitrogen*grass->falloc.leaf/pft->nind)/(grass->ind.leaf.carbon-grass->turn.leaf.carbon+pft->bm_inc.carbon*grass->falloc.leaf/pft->nind);
  if(NC_leaf>=(pft->par->ncleaf.high*(1+pft->par->knstore)))
    return(0.0); /* no N demand, return 0 */

  /* compute total mineral N availability */
  forrootsoillayer(l)
  {
      wscaler = soil->w[l] > epsilon ? 1 : 0;
      up_temp_f[l] = nuptake_temp_fcn(soil->temp[l]);
      totnh4 += (soil->NH4[l]) * wscaler;
      totno3 += (soil->NO3[l]) * wscaler;
  }
  totn = totnh4 + totno3; /* total mineral N available in soil */
  if(totn < epsilon)
  {
    /* no N available, return 0 */
    return(0.0);
  }
  /* assign NO3 and NH4 specific uptake shares */
  /* TO DO: This should be weighted by N-form specific uptake rates */
  ndemand_nh4 = ndemand_actual * totnh4 / totn; /* N demand from NH4 */
  ndemand_no3 = ndemand_actual * totno3 / totn; /* N demand from NO3 */

  /* compute potential uptake across layers and N-forms */
  forrootsoillayer(l){
    /* TO DO: make pft->par->KNmin N-form specific */
    /* TO DO: make pft->par->vmax_up N-form specific */
    NH4_up[l] = pft->par->vmax_up*(pft->par->kNmin +totnh4/(totnh4+pft->par->KNmin*soil->wsat[l]*soildepth[l]/1000))* up_temp_f[l]*
      f_NCplant * (grass->ind.root.carbon*pft->nind+pft->bm_inc.carbon*grass->falloc.root-grass->turn_litt.root.carbon)*rootdist_n[l]/1000;  //Smith et al. Eq. C14-C15, Navail=totn
    nh4sum+=NH4_up[l];
    NO3_up[l] = pft->par->vmax_up*(pft->par->kNmin +totno3/(totno3+pft->par->KNmin*soil->wsat[l]*soildepth[l]/1000))* up_temp_f[l]*
      f_NCplant * (grass->ind.root.carbon*pft->nind+pft->bm_inc.carbon*grass->falloc.root-grass->turn_litt.root.carbon)*rootdist_n[l]/1000;
    no3sum+=NO3_up[l];
  }
  if((nh4sum + no3sum) < epsilon)
  {
    /* no N available for uptake, return 0 */
    return(0.0);
  }
  
  /* actually take up N and remove from layers */
  if(no3sum>0) no3_update_reduction = ndemand_no3 / no3sum; /* scaling update down if demand is lower than uptake potential */
  if(nh4sum>0) nh4_update_reduction = ndemand_nh4 / nh4sum; /* scaling update down if demand is lower than uptake potential */
  forrootsoillayer(l){
    no3_uptake += NO3_up[l] * no3_update_reduction;
    soil->NO3[l] -= NO3_up[l] * no3_update_reduction;
#ifdef SAFE
    if(soil->NO3[l]<0)
    {
      no3_upfail += soil->NO3[l];
      soil->NO3[l]=0.0;
    }
#endif
    nh4_uptake += NH4_up[l] * nh4_update_reduction;
    soil->NH4[l] -= NH4_up[l] * nh4_update_reduction;
#ifdef SAFE
    if(soil->NH4[l]<0)
    {
      nh4_upfail += soil->NH4[l];
      soil->NH4[l]=0.0;
    }
#endif
  }
  /* correcting for failed uptake from depleted soils in outputs */
  nh4_uptake+=nh4_upfail;
  no3_uptake+=no3_upfail;


  /* add autofert input */
  if(config->fertilizer_input==AUTO_FERTILIZER && pft->stand->type->landusetype!=NATURAL)
  {
    data=pft->stand->data;
    autofert_n=*n_plant_demand-(vegn_sum_grass(pft)+pft->bm_inc.nitrogen);
    if(autofert_n>0)
    {
      no3_uptake += autofert_n/2.0;
      nh4_uptake += autofert_n/2.0;
      pft->bm_inc.nitrogen += autofert_n;
      pft->stand->cell->balance.influx.nitrogen += autofert_n*pft->stand->frac;
      getoutput(&pft->stand->cell->output,FLUX_AUTOFERT,config)+=autofert_n*pft->stand->frac;
      switch(pft->stand->type->landusetype)
      {
        case BIOMASS_TREE:
          getoutputindex(&pft->stand->cell->output,CFT_NFERT,rbtree(ncft)+data->irrigation*nirrig,config)+=autofert_n;
          break;
        case WOODPLANTATION:
          getoutputindex(&pft->stand->cell->output,CFT_NFERT,rwp(ncft)+data->irrigation*nirrig,config)+=autofert_n;
          break;
        case AGRICULTURE_TREE:
          getoutputindex(&pft->stand->cell->output,CFT_NFERT,data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=autofert_n;
          break;
        default:
          /* do nothing */
          break;
      }
    }
    pft->vscal+=1;
    pft->npp_bnf=0.0;
  }
  else  /* add BNF */
  {
    n_deficit = *n_plant_demand/(1+pft->par->knstore)-(vegn_sum_tree(pft)+pft->bm_inc.nitrogen);
    if(n_deficit>0 && pft->npp_bnf>0)
    {
       n_fixed=npp_contr_biol_n_fixation(pft, soil, n_deficit, config);
       pft->bm_inc.nitrogen+=n_fixed;
       getoutput(&pft->stand->cell->output,BNF,config)+=n_fixed*pft->stand->frac;
       if(pft->stand->type->landusetype!=NATURAL && pft->stand->type->landusetype!=WOODPLANTATION)
         getoutput(&pft->stand->cell->output,BNF_MG,config)+=n_fixed*pft->stand->frac;
       pft->stand->cell->balance.influx.nitrogen+=n_fixed*pft->stand->frac;
    }
    else
      pft->npp_bnf=0.0;
  }
  /* compute vscal */
  if(ndemand_leaf_opt<epsilon)
    pft->vscal+=1;
  else
    pft->vscal+=min(1,*ndemand_leaf/(ndemand_leaf_opt/(1+pft->par->knstore))); /*eq. C20 in Smith et al. 2014, Biogeosciences */

 


#ifdef SCRAPYARD
  NCplant = (grass->ind.leaf.nitrogen+ grass->ind.root.nitrogen) / (grass->ind.leaf.carbon+ grass->ind.root.carbon); /* Plant's mobile nitrogen concentration, Eq.9, Zaehle&Friend 2010 Supplementary */

  f_NCplant = min(max(((NCplant-pft->par->ncleaf.high)/(2.0/(1.0/pft->par->ncleaf.low+1.0/pft->par->ncleaf.high)-pft->par->ncleaf.high)),0),1); /* consistent with Smith et al. 2014 */
  /* reducing uptake according to availability */
  nh4sum=no3sum=0;
#ifdef DEBUG_N_UPTAKE
  forrootsoillayer(l)
  {
    NO3_before[l]=soil->NO3[l];
    NH4_before[l]=soil->NH4[l];
    nh4before+=soil->NH4[l];
    no3before+=soil->NO3[l];
  }
  vegn_before=vegn_sum_grass(pft);
  bm_incn_before=pft->bm_inc.nitrogen;
#endif
  if((grass->ind.leaf.carbon-grass->turn.leaf.carbon+pft->bm_inc.carbon*grass->falloc.leaf/pft->nind)==0)
    NC_leaf=pft->par->ncleaf.low;
  else
    NC_leaf=(grass->ind.leaf.nitrogen-grass->turn.leaf.nitrogen+pft->bm_inc.nitrogen*grass->falloc.leaf/pft->nind)/(grass->ind.leaf.carbon-grass->turn.leaf.carbon+pft->bm_inc.carbon*grass->falloc.leaf/pft->nind);
  if(NC_leaf<(pft->par->ncleaf.high*(1+pft->par->knstore)))
    forrootsoillayer(l)
    {
      wscaler=soil->w[l]>epsilon ? 1 : 0;
      up_temp_f = nuptake_temp_fcn(soil->temp[l]);
      totn=soil->NH4[l]*wscaler;
      //fprintf(stdout,"layer %d wscaler %g minN %g %g up_temp_f %g \n",l,wscaler,soil->NH4[l], soil->NO3[l],up_temp_f);
      //fflush(stdout);
      if(totn>0)
      {
        /*Thornley 1991*/
        N_up = 5*pft->par->vmax_up*(pft->par->kNmin+totn/(totn+pft->par->KNmin*soil->wsat[l]*soildepth[l]/1000))* up_temp_f*
            f_NCplant * (grass->ind.root.carbon*pft->nind+pft->bm_inc.carbon*grass->falloc.root-grass->turn_litt.root.carbon)*rootdist_n[l]/1000;
        /* reducing uptake according to availability */
        //fprintf(stdout,"N_up1: %g ",N_up);
        //fflush(stdout);
        if(N_up>totn)
          N_up=totn;
        //fprintf(stdout,"N_up2 %g ",N_up);
        //fflush(stdout);
        nh4_uptake+=N_up;
        nh4sum+=N_up; //totn*rootdist_n[l];
        //fprintf(stdout,"nh4_uptake %g nh4sum %g ",nh4_uptake,nh4sum);
        //fflush(stdout);
      }
      totn=soil->NO3[l]*wscaler;
      if(totn>0)
      {
        /*Thornley 1991*/
        N_up = pft->par->vmax_up*(pft->par->kNmin+totn/(totn+pft->par->KNmin*soil->wsat[l]*soildepth[l]/1000))* up_temp_f*
            f_NCplant * (grass->ind.root.carbon*pft->nind+pft->bm_inc.carbon*grass->falloc.root-grass->turn_litt.root.carbon)*rootdist_n[l]/1000;
        /* reducing uptake according to availability */
        //fprintf(stdout,"N_up3: %g ",N_up);
        //fflush(stdout);
        if(N_up>totn)
          N_up=totn;
        //fprintf(stdout,"N_up4 %g ",N_up);
        //fflush(stdout);
        no3_uptake+=N_up;
        no3sum+=N_up; //totn*rootdist_n[l];
        //fprintf(stdout,"no3_uptake %g no3sum %g \n",no3_uptake,no3sum);
        //fflush(stdout);
      }
    }
  if(nh4sum==0)
    nh4_uptake=0;
  else
  {
    if (nh4_uptake>*n_plant_demand-(vegn_sum_grass(pft)+pft->bm_inc.nitrogen))
      nh4_uptake=*n_plant_demand-(vegn_sum_grass(pft)+pft->bm_inc.nitrogen);
    if(nh4_uptake<=0)
      nh4_uptake=0;
    else
    {
      pft->bm_inc.nitrogen+=nh4_uptake;
      forrootsoillayer(l)
      {
        wscaler=soil->w[l]>epsilon ? 1 : 0;
        //soil->NH4[l]-=soil->NH4[l]*wscaler*rootdist_n[l]*nh4_uptake/nh4sum;
        soil->NH4[l]-=wscaler*rootdist_n[l]*nh4_uptake;
        if(soil->NH4[l]<0)
        {
          pft->bm_inc.nitrogen+=soil->NH4[l];
          nh4_upfail+=soil->NH4[l];
          soil->NH4[l]=0;
        }
      }
    }
  }
  if(no3sum==0)
    no3_uptake=0;
  else
  {
    if (no3_uptake>*n_plant_demand-(vegn_sum_grass(pft)+pft->bm_inc.nitrogen))
      no3_uptake=*n_plant_demand-(vegn_sum_grass(pft)+pft->bm_inc.nitrogen);
    if(no3_uptake<=0)
      no3_uptake=0;
    else
    {
      pft->bm_inc.nitrogen+=no3_uptake;
      forrootsoillayer(l)
      {
        wscaler=soil->w[l]>epsilon ? 1 : 0;
        //soil->NO3[l]-=soil->NO3[l]*wscaler*rootdist_n[l]*no3_uptake/no3sum;
        soil->NO3[l]-=wscaler*rootdist_n[l]*no3_uptake;
        if(soil->NO3[l]<0)
        {
          pft->bm_inc.nitrogen+=soil->NO3[l];
          no3_upfail+=soil->NO3[l];
          soil->NO3[l]=0;
        }
      }
    }
  }
  if(config->fertilizer_input==AUTO_FERTILIZER
    && (pft->stand->type->landusetype==GRASSLAND || pft->stand->type->landusetype==OTHERS || pft->stand->type->landusetype==BIOMASS_GRASS || pft->stand->type->landusetype==AGRICULTURE_GRASS || pft->stand->type->landusetype == SETASIDE_RF || pft->stand->type->landusetype == SETASIDE_IR))
  {
    data=pft->stand->data;
    autofert_n=*n_plant_demand-(vegn_sum_grass(pft)+pft->bm_inc.nitrogen);
    if(autofert_n>0)
    {
      nh4_uptake += autofert_n/2.0;
      no3_uptake += autofert_n/2.0;
      pft->bm_inc.nitrogen += autofert_n;
      pft->stand->cell->balance.influx.nitrogen += autofert_n*pft->stand->frac;
      getoutput(&pft->stand->cell->output,FLUX_AUTOFERT,config)+=autofert_n*pft->stand->frac;
      switch(pft->stand->type->landusetype)
      {
        case GRASSLAND:
          getoutputindex(&pft->stand->cell->output,CFT_NFERT,rmgrass(ncft)+data->irrigation*nirrig,config)+=autofert_n;
          break;
        case OTHERS:
          getoutputindex(&pft->stand->cell->output,CFT_NFERT,rothers(ncft)+data->irrigation*nirrig,config)+=autofert_n;
          break;
        case BIOMASS_GRASS:
          getoutputindex(&pft->stand->cell->output,CFT_NFERT,rbgrass(ncft)+data->irrigation*nirrig,config)+=autofert_n;
          break;
        case AGRICULTURE_GRASS:
          getoutputindex(&pft->stand->cell->output,CFT_NFERT,data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=autofert_n;
          break;
        default:
          /* do nothing */
          break;
      }
    }
    pft->vscal+=1;
    pft->npp_bnf=0.0;
  }
  else
  {
    n_deficit = *n_plant_demand/(1+pft->par->knstore)-(vegn_sum_grass(pft)+pft->bm_inc.nitrogen);
    if(n_deficit>0 && pft->npp_bnf>0)
    {
       n_fixed=npp_contr_biol_n_fixation(pft, soil, n_deficit, config);
       pft->bm_inc.nitrogen+=n_fixed;
       getoutput(&pft->stand->cell->output,BNF,config)+=n_fixed*pft->stand->frac;
       if(pft->stand->type->landusetype!=NATURAL)
         getoutput(&pft->stand->cell->output,BNF_MG,config)+=n_fixed*pft->stand->frac;
       pft->stand->cell->balance.influx.nitrogen+=n_fixed*pft->stand->frac;
    }
    else
      pft->npp_bnf=0.0;
    if(*n_plant_demand/(1+pft->par->knstore)>(vegn_sum_grass(pft)+pft->bm_inc.nitrogen))
    {
      if((grass->ind.leaf.carbon-grass->turn.leaf.carbon+pft->bm_inc.carbon*grass->falloc.leaf/pft->nind)==0)
        NC_leaf=pft->par->ncleaf.low;
      else
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
  nh4_uptake+=nh4_upfail;
  no3_uptake+=no3_upfail;

#endif
  /* write outputs */
  switch(pft->stand->type->landusetype)
  {
    case NATURAL: case SETASIDE_RF: case SETASIDE_IR:
      if(config->pft_output_scaled)
        getoutputindex(&pft->stand->cell->output,PFT_BNF,pft->par->id,config)+=n_fixed*pft->stand->frac;
      else
        getoutputindex(&pft->stand->cell->output,PFT_BNF,pft->par->id,config)+=n_fixed;
      getoutputindex(&pft->stand->cell->output,PFT_NUPTAKE,pft->par->id,config)+=nh4_uptake+no3_uptake;
      getoutputindex(&pft->stand->cell->output,PFT_NDEMAND,pft->par->id,config)+=max(0,ndemand_all-(vegn_sum_grass(pft)+pft->bm_inc.carbon))/365;
      break;
    case BIOMASS_GRASS:
      if(config->pft_output_scaled)
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rbgrass(ncft)+data->irrigation*nirrig,config)+=n_fixed*pft->stand->frac;
      else
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rbgrass(ncft)+data->irrigation*nirrig,config)+=n_fixed; /* *stand->cell->ml.landfrac[data->irrigation].biomass_grass; */
      getoutputindex(&pft->stand->cell->output,PFT_NUPTAKE,nnat+rbgrass(ncft)+data->irrigation*nirrig,config)+=nh4_uptake+no3_uptake; /* *stand->cell->ml.landfrac[data->irrigation].biomass_grass; */
      getoutputindex(&pft->stand->cell->output,PFT_NDEMAND,nnat+rbgrass(ncft)+data->irrigation*nirrig,config)+=max(0,ndemand_all-(vegn_sum_grass(pft)+pft->bm_inc.carbon))/365; /* *stand->cell->ml.landfrac[data->irrigation].biomass_grass; */
      break;
    case AGRICULTURE_GRASS:
      if(config->pft_output_scaled)
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=n_fixed*pft->stand->frac;
      else
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=n_fixed; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
      getoutputindex(&pft->stand->cell->output,PFT_NUPTAKE,nnat+data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=nh4_uptake+no3_uptake; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
      getoutputindex(&pft->stand->cell->output,PFT_NDEMAND,nnat+data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=max(0,ndemand_all-(vegn_sum_grass(pft)+pft->bm_inc.carbon))/365; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
      break;
    case GRASSLAND:
      if(config->pft_output_scaled)
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rmgrass(ncft)+data->irrigation*nirrig,config)+=n_fixed*pft->stand->frac;
      else
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rmgrass(ncft)+data->irrigation*nirrig,config)+=n_fixed;/*pft->stand->cell->ml.landfrac[data->irrigation].grass[1];*/
      getoutputindex(&pft->stand->cell->output,PFT_NUPTAKE,nnat+rmgrass(ncft)+data->irrigation*nirrig,config)+=nh4_uptake+no3_uptake;/*pft->stand->cell->ml.landfrac[data->irrigation].grass[1];*/
      getoutputindex(&pft->stand->cell->output,PFT_NDEMAND,nnat+rmgrass(ncft)+data->irrigation*nirrig,config)+=max(0,ndemand_all-(vegn_sum_grass(pft)+pft->bm_inc.carbon))/365;/*pft->stand->cell->ml.landfrac[data->irrigation].grass[1];*/
      break;
    case OTHERS:
      if(config->pft_output_scaled)
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rothers(ncft)+data->irrigation*nirrig,config)+=n_fixed*pft->stand->frac;
      else
        getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rothers(ncft)+data->irrigation*nirrig,config)+=n_fixed;/*pft->stand->cell->ml.landfrac[data->irrigation].grass[0];*/
      getoutputindex(&pft->stand->cell->output,PFT_NUPTAKE,nnat+rothers(ncft)+data->irrigation*nirrig,config)+=nh4_uptake+no3_uptake;/*pft->stand->cell->ml.landfrac[data->irrigation].grass[0];*/
      getoutputindex(&pft->stand->cell->output,PFT_NDEMAND,nnat+rothers(ncft)+data->irrigation*nirrig,config)+=max(0,ndemand_all-(vegn_sum_grass(pft)+pft->bm_inc.carbon))/365;/*pft->stand->cell->ml.landfrac[data->irrigation].grass[0];*/
      break;
    default:
      /* do nothing */
      break;
  } /* of 'switch' */
  pft->stand->cell->balance.n_uptake+=(nh4_uptake+no3_uptake)*pft->stand->frac;
  pft->stand->cell->balance.n_demand+=max(0,(ndemand_all-(vegn_sum_grass(pft)+pft->bm_inc.nitrogen)))*pft->stand->frac/365;
#ifdef DEBUG_N_UPTAKE
  forrootsoillayer(l)
  {
    NO3_after[l]=soil->NO3[l];
    NH4_after[l]=soil->NH4[l];
    nh4after+=soil->NH4[l];
    no3after+=soil->NO3[l];
  }
  vegn_after=vegn_sum_grass(pft);
  bm_incn_after=pft->bm_inc.nitrogen;
if(fabs((no3before+nh4before+vegn_before/pft->nind+bm_incn_before-(no3after+nh4after+vegn_after/pft->nind+bm_incn_after))-
         (nh4_uptake+no3_uptake+n_fixed))> epsilon){
  fprintf(stdout,"\n\n\npool changes:\n");
  forrootsoillayer(l){
    fprintf(stdout,"NO3_before[%d] %g after %g diff %g\n",l,NO3_before[l],NO3_after[l],NO3_after[l]-NO3_before[l]);
    fprintf(stdout,"NH4_before[%d] %g after %g diff %g\n",l,NH4_before[l],NH4_after[l],NH4_after[l]-NH4_before[l]);
  }
  fprintf(stdout,"nh4before %g after %g diff %g\n",nh4before,nh4after,nh4after-nh4before);
  fprintf(stdout,"no3before %g after %g diff %g\n",no3before,no3after,no3after-no3before);
  fprintf(stdout,"veg nitrogen before %g after %g diff %g\n",vegn_before,vegn_after,vegn_after-vegn_before);
  fprintf(stdout,"veg nitrogen before %g after %g diff %g\n",vegn_before/pft->nind,vegn_after/pft->nind,(vegn_after-vegn_before)/pft->nind);
  fprintf(stdout,"bm_inc nitrogen before %g after %g diff %g\n",bm_incn_before,bm_incn_after,bm_incn_after-bm_incn_before);
  fprintf(stdout,"nh4_uptake %g no3_uptake %g n_fixed %g\n",nh4_uptake,no3_uptake,n_fixed);
  fprintf(stdout,"nh4_upfail %g no3_upfail %g\n",nh4_upfail,no3_upfail);
  fprintf(stdout,"delta pools %g\n",no3before+nh4before+vegn_before/pft->nind+bm_incn_before-(no3after+nh4after+vegn_after/pft->nind+bm_incn_after));
  fprintf(stdout,"sum fluxes %g\n",nh4_uptake+no3_uptake+n_fixed);
  fprintf(stdout,"n_deficit %g\n",n_deficit);  
  fprintf(stdout,"balance %g\n\n\n\n",(no3before+nh4before+vegn_before/pft->nind+bm_incn_before-(no3after+nh4after+vegn_after/pft->nind+bm_incn_after))-
         (nh4_uptake+no3_uptake+n_fixed));
  fflush(stdout);
  }
#endif

  return nh4_uptake+no3_uptake;
} /* of 'nuptake_grass' */

/**************************************************************************************/
/**                                                                                \n**/
/**               n  u  p  t  a  k  e  _  t  r  e  e  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates nitrogen uptake of trees                               \n**/
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
#include "agriculture.h"

Real nuptake_tree(Pft *pft,             /**< pointer to PFT data */
                  Real *n_plant_demand, /**< total N plant demand */
                  Real *ndemand_leaf,   /**< N demand of leafs */
                  int npft,             /**< number of natural PFTs */
                  int ncft,             /**< number of crop PFTs */
                  const Config *config  /**< LPJmL configuration */
                 )                      /** \return nitrogen uptake (gN/m2/day) */
{

  Pfttree *tree;
  Soil *soil;
  const Pfttreepar *treepar;
  Real NO3_up=0;
  Real NCplant,ndemand_leaf_opt,NC_actual,NC_leaf,ndemand_all;
  Real f_NCplant=0;
  Real up_temp_f;
  Real totn,nsum;
  Real wscaler;
  Real autofert_n;
  Real n_uptake=0;
  Real n_upfail=0; /**< track n_uptake that is not available from soil for output reporting */
  int l,nnat,nirrig;
  Irrigation *data;
  Real rootdist_n[LASTLAYER];
  Real n_deficit=0.0;
  Real n_fixed=0.0;
  soil=&pft->stand->soil;
  ndemand_all=*n_plant_demand;
  if(config->permafrost)
    getrootdist(rootdist_n,pft->par->rootdist,soil->mean_maxthaw);
  else
    forrootsoillayer(l)
      rootdist_n[l]=pft->par->rootdist[l];
  tree=pft->data;
  treepar=pft->par->data;
  nnat=getnnat(npft,config);
  nirrig=getnirrig(ncft,config);
 // NCplant = (vegn_sum_tree(pft)-(tree->ind.sapwood.nitrogen+tree->ind.heartwood.nitrogen)*pft->nind+pft->bm_inc.nitrogen*(tree->falloc.leaf+tree->falloc.leaf))/(vegc_sum_tree(pft)-(tree->ind.sapwood.carbon+tree->ind.heartwood.carbon)*pft->nind+pft->bm_inc.carbon*(tree->falloc.leaf+tree->falloc.leaf)); /* Plant's mobile nitrogen concentration, Eq.9, Zaehle&Friend 2010 Supplementary */
  NC_leaf=(tree->ind.leaf.nitrogen-tree->turn.leaf.nitrogen+pft->bm_inc.nitrogen*tree->falloc.leaf/pft->nind)/(tree->ind.leaf.carbon-tree->turn.leaf.carbon+pft->bm_inc.carbon*tree->falloc.leaf/pft->nind);
  NCplant = (tree->ind.leaf.nitrogen+ tree->ind.root.nitrogen) / (tree->ind.leaf.carbon+ tree->ind.root.carbon); /* Plant's mobile nitrogen concentration, Eq.9, Zaehle&Friend 2010 Supplementary */
  f_NCplant = min(max(((NC_leaf-pft->par->ncleaf.high)/(pft->par->ncleaf.low-pft->par->ncleaf.high)),0),1); /*zaehle supple 10*/
  ndemand_leaf_opt=*ndemand_leaf;
  nsum=0;
  if(NC_leaf<(pft->par->ncleaf.high*(1+pft->par->knstore)))
    forrootsoillayer(l)
    {
      wscaler=soil->w[l]>epsilon ? 1 : 0;
      totn=(soil->NO3[l]+soil->NH4[l])*wscaler;
      if(totn>0)
      {
        up_temp_f = nuptake_temp_fcn(soil->temp[l]);
        //up_temp_f=1;
        NO3_up = 2*pft->par->vmax_up*(pft->par->kNmin +totn/(totn+pft->par->KNmin*soil->wsat[l]*soildepth[l]/1000))* up_temp_f*
            f_NCplant * (tree->ind.root.carbon*pft->nind+pft->bm_inc.carbon*tree->falloc.root-tree->turn_litt.root.nitrogen)*rootdist_n[l]/1000;  //Smith et al. Eq. C14-C15, Navail=totn
        /* reducing uptake according to availability */
        if(NO3_up>totn)
          NO3_up=totn;
        n_uptake+=NO3_up;
        nsum+=totn*rootdist_n[l];
      }
    }
#ifdef DEBUG_N
  printf("TREE n_uptake=%g, nplant_demand=%g vegn=%g\n",n_uptake,*n_plant_demand,(vegn_sum_tree(pft)-tree->ind.heartwood.nitrogen*pft->nind));
#endif
  if(nsum<epsilon)     //nsum==0
    n_uptake=0;
  else
  {
    if (n_uptake>*n_plant_demand-(vegn_sum_tree(pft)+pft->bm_inc.nitrogen-tree->ind.heartwood.nitrogen*pft->nind))
      n_uptake=*n_plant_demand-(vegn_sum_tree(pft)+pft->bm_inc.nitrogen-tree->ind.heartwood.nitrogen*pft->nind);
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
          //pft->bm_inc.nitrogen+=soil->NO3[l];
          n_upfail+=soil->NO3[l];
          soil->NO3[l]=0;
        }
        soil->NH4[l]-=soil->NH4[l]*wscaler*rootdist_n[l]*n_uptake/nsum-n_upfail*wscaler;
        if(wscaler>0)
         n_upfail=0;
        if(soil->NH4[l]<0)
        {
          //pft->bm_inc.nitrogen+=soil->NH4[l];
          n_upfail+=soil->NH4[l];
          soil->NH4[l]=0;
        }
      }
      pft->bm_inc.nitrogen+=n_upfail;
    }
  }
  if(config->fertilizer_input==AUTO_FERTILIZER && pft->stand->type->landusetype!=NATURAL && pft->stand->type->landusetype!=WETLAND)
  {
    data=pft->stand->data;
    autofert_n=*n_plant_demand-(vegn_sum_tree(pft)+pft->bm_inc.nitrogen-tree->ind.heartwood.nitrogen*pft->nind);
    n_uptake += autofert_n;
    pft->bm_inc.nitrogen += autofert_n;
    pft->vscal+=1;
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
    }
  }
  else
  {
    n_deficit = *n_plant_demand/(1+pft->par->knstore)-(vegn_sum_tree(pft)+pft->bm_inc.nitrogen-tree->ind.heartwood.nitrogen*pft->nind);
    if(n_deficit>0 && pft->npp_bnf>0)
    {
       n_fixed=npp_contr_biol_n_fixation(pft, soil, n_deficit, config);
       pft->bm_inc.nitrogen+=n_fixed;
       getoutput(&pft->stand->cell->output,BNF,config)+=n_fixed*pft->stand->frac;
       pft->stand->cell->balance.influx.nitrogen+=n_fixed*pft->stand->frac;
    }
    else
      pft->npp_bnf=0.0;
    if(*n_plant_demand/(1+pft->par->knstore)>(vegn_sum_tree(pft)+pft->bm_inc.nitrogen-tree->ind.heartwood.nitrogen*pft->nind))   /*HERE RECALCULATION OF N-demand TO N-supply*/
    {
      NC_actual=(vegn_sum_tree(pft)+pft->bm_inc.nitrogen-tree->ind.heartwood.nitrogen*pft->nind)/(vegc_sum_tree(pft)+pft->bm_inc.carbon-tree->ind.heartwood.carbon*pft->nind);
      NC_leaf=(tree->ind.leaf.nitrogen-tree->turn.leaf.nitrogen+pft->bm_inc.nitrogen*tree->falloc.leaf/pft->nind)/(tree->ind.leaf.carbon-tree->turn.leaf.carbon+pft->bm_inc.carbon*tree->falloc.leaf/pft->nind);
      if(NC_leaf< pft->par->ncleaf.low)
        NC_leaf=pft->par->ncleaf.low;
      else if (NC_leaf>pft->par->ncleaf.high)
        NC_leaf=pft->par->ncleaf.high;
      *ndemand_leaf=tree->ind.leaf.nitrogen*pft->nind+pft->bm_inc.nitrogen*tree->falloc.leaf-tree->turn_litt.leaf.nitrogen;
      *ndemand_leaf=max(tree->ind.leaf.nitrogen*pft->nind-tree->turn_litt.leaf.nitrogen,*ndemand_leaf);
      *n_plant_demand=*ndemand_leaf+(tree->ind.root.nitrogen+tree->ind.sapwood.nitrogen-tree->turn.root.nitrogen)*pft->nind+NC_leaf*(tree->excess_carbon*pft->nind+pft->bm_inc.carbon)*(tree->falloc.root/treepar->ratio.root+tree->falloc.sapwood/treepar->ratio.sapwood);
    }

    if(ndemand_leaf_opt<epsilon)
      pft->vscal+=1;
    else
      pft->vscal+=min(1,*ndemand_leaf/(ndemand_leaf_opt/(1+pft->par->knstore))); /*eq. C20 in Smith et al. 2014, Biogeosciences */
    /* correcting for failed uptake from depleted soils in outputs */
    n_uptake+=n_upfail;
  }

  switch(pft->stand->type->landusetype)
  {
  case BIOMASS_TREE:
    data=pft->stand->data;
    if(config->pft_output_scaled)
      getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rbtree(ncft)+data->irrigation*nirrig,config)+=n_fixed*pft->stand->frac;
    else
      getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rbtree(ncft)+data->irrigation*nirrig,config)+=n_fixed; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
    getoutputindex(&pft->stand->cell->output,PFT_NUPTAKE,nnat+rbtree(ncft)+data->irrigation*nirrig,config)+=n_uptake; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
    getoutputindex(&pft->stand->cell->output,PFT_NDEMAND,nnat+rbtree(ncft)+data->irrigation*nirrig,config)+=max(0,ndemand_all-(vegn_sum_tree(pft)+pft->bm_inc.nitrogen-tree->ind.heartwood.nitrogen*pft->nind))/365; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
    break;
  case AGRICULTURE_TREE:
    data=pft->stand->data;
    if(config->pft_output_scaled)
      getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=n_fixed*pft->stand->frac;
    else
      getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=n_fixed; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
    getoutputindex(&pft->stand->cell->output,PFT_NUPTAKE,nnat+data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=n_uptake; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
    getoutputindex(&pft->stand->cell->output,PFT_NDEMAND,nnat+data->pft_id-npft+config->nagtree+agtree(ncft,config->nwptype)+data->irrigation*nirrig,config)+=max(0,ndemand_all-(vegn_sum_tree(pft)+pft->bm_inc.nitrogen-tree->ind.heartwood.nitrogen*pft->nind))/365; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
    break;
  case WOODPLANTATION:
    data=pft->stand->data;
    if(config->pft_output_scaled)
      getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rwp(ncft)+data->irrigation*nirrig,config)+=n_fixed*pft->stand->frac; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
    else
      getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+rwp(ncft)+data->irrigation*nirrig,config)+=n_fixed; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
    getoutputindex(&pft->stand->cell->output,PFT_NUPTAKE,nnat+rwp(ncft)+data->irrigation*nirrig,config)+=n_uptake; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
    getoutputindex(&pft->stand->cell->output,PFT_NDEMAND,nnat+rwp(ncft)+data->irrigation*nirrig,config)+=max(0,ndemand_all-(vegn_sum_tree(pft)+pft->bm_inc.nitrogen-tree->ind.heartwood.nitrogen*pft->nind))/365; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
    break;
  default:
    if(config->pft_output_scaled)
      getoutputindex(&pft->stand->cell->output,PFT_BNF,pft->par->id,config)+=n_fixed*pft->stand->frac;
    else
      getoutputindex(&pft->stand->cell->output,PFT_BNF,pft->par->id,config)+=n_fixed;
    getoutputindex(&pft->stand->cell->output,PFT_NUPTAKE,pft->par->id,config)+=n_uptake;
    getoutputindex(&pft->stand->cell->output,PFT_NDEMAND,pft->par->id,config)+=max(0,ndemand_all-(vegn_sum_tree(pft)+pft->bm_inc.nitrogen-tree->ind.heartwood.nitrogen*pft->nind))/365;
  } /* of 'switch' */
  pft->stand->cell->balance.n_uptake+=n_uptake*pft->stand->frac;
  pft->stand->cell->balance.n_demand+=max(0,(ndemand_all-(vegn_sum_tree(pft)+pft->bm_inc.nitrogen-tree->ind.heartwood.nitrogen*pft->nind)))*pft->stand->frac/365;
  return n_uptake;
} /* of 'nuptake_tree' */

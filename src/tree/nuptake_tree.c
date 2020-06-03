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
                  int nbiomass,         /**< number of biomass PFTs */
                  int ncft              /**< number of crop PFTs */
                 )                      /** \return nitrogen uptake (gN/m2/day) */
{

  Pfttree *tree;
  Soil *soil;
  const Pfttreepar *treepar;
  Real NO3_up=0;
  Real NCplant,ndemand_leaf_opt,NC_actual,NC_leaf;
  Real f_NCplant=0;
  Real up_temp_f;
  Real totn,nsum;
  Real wscaler;
  Real n_uptake=0;
  Real n_upfail=0; /**< track n_uptake that is not available from soil for output reporting */
  int l;
  Irrigation *data;
  Real rootdist_n[LASTLAYER];
  soil=&pft->stand->soil;
  getrootdist(rootdist_n,pft->par->rootdist,soil->mean_maxthaw);
  tree=pft->data;
  treepar=pft->par->data;
  NCplant = (tree->ind.leaf.nitrogen+ tree->ind.root.nitrogen) / (tree->ind.leaf.carbon+ tree->ind.root.carbon); /* Plant's mobile nitrogen concentration, Eq.9, Zaehle&Friend 2010 Supplementary */
  f_NCplant = min(max(((NCplant-pft->par->ncleaf.high)/(pft->par->ncleaf.low-pft->par->ncleaf.high)),0),1); /*zaehle supple 10*/
  ndemand_leaf_opt=*ndemand_leaf;
  nsum=0;
  forrootsoillayer(l)
  {
    wscaler=(soil->w[l]+soil->ice_depth[l]/soil->whcs[l]>0) ? (soil->w[l]/(soil->w[l]+soil->ice_depth[l]/soil->whcs[l])) : 0;
    totn=(soil->NO3[l]+soil->NH4[l])*wscaler;
    if(totn>0)
    {
      up_temp_f = nuptake_temp_fcn(soil->temp[l]);
      //up_temp_f=1;
      NO3_up = 2*pft->par->vmax_up*(pft->par->kNmin +totn/(totn+pft->par->KNmin*soil->wsat[l]*soildepth[l]/1000))* up_temp_f * f_NCplant * (tree->ind.root.carbon*pft->nind+pft->bm_inc.carbon*tree->falloc.root)*rootdist_n[l]/1000; //Smith et al. Eq. C14-C15, Navail=totn

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
  if(nsum==0)
    n_uptake=0;
  else
  {
  if (n_uptake>*n_plant_demand-(vegn_sum_tree(pft)-tree->ind.heartwood.nitrogen*pft->nind))
    n_uptake=*n_plant_demand-(vegn_sum_tree(pft)-tree->ind.heartwood.nitrogen*pft->nind);
  if(n_uptake<=0)
    n_uptake=0;
  else
  {
    pft->bm_inc.nitrogen+=n_uptake;
    forrootsoillayer(l)
      if(soil->temp[l]>0 && nsum > 0.0)
      {
        wscaler=(soil->w[l]+soil->ice_depth[l]/soil->whcs[l]>0) ? (soil->w[l]/(soil->w[l]+soil->ice_depth[l]/soil->whcs[l])) : 0;
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
  if(*n_plant_demand/(1+pft->par->knstore)>(vegn_sum_tree(pft)-tree->ind.heartwood.nitrogen*pft->nind))   /*HERE RECALCULATION OF N-demand TO N-supply*/
  {
    *n_plant_demand=(vegn_sum_tree(pft)-tree->ind.heartwood.nitrogen*pft->nind);
    NC_actual=(vegn_sum_tree(pft)-tree->ind.heartwood.nitrogen*pft->nind)/(vegc_sum_tree(pft)-tree->ind.heartwood.carbon*pft->nind);
    NC_leaf=NC_actual/(tree->falloc.sapwood/treepar->ratio.sapwood+tree->falloc.root/treepar->ratio.root+tree->falloc.leaf);
    if(NC_leaf< pft->par->ncleaf.low)
        NC_leaf=pft->par->ncleaf.low;
    else if (NC_leaf>pft->par->ncleaf.high)
        NC_leaf=pft->par->ncleaf.high;
//    *ndemand_leaf=(tree->ind.leaf.carbon*pft->nind+pft->bm_inc.carbon*tree->falloc.leaf)*NC_leaf;
    *ndemand_leaf=(tree->ind.leaf.carbon*pft->nind)*NC_leaf;
  }

  if(ndemand_leaf_opt<epsilon)
    pft->vscal+=1;
  else
   pft->vscal+=min(1,*ndemand_leaf/(ndemand_leaf_opt/(1+pft->par->knstore))); /*eq. C20 in Smith et al. 2014, Biogeosciences */
  /* correcting for failed uptake from depleted soils in outputs */
  n_uptake+=n_upfail;
  if(pft->stand->type->landusetype==BIOMASS_TREE)
  {
    data=pft->stand->data;
    pft->stand->cell->output.pft_nuptake[(npft-nbiomass)+rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=n_uptake; /* stand->cell->ml.landfrac[data->irrigation].biomass_tree; */
  }
  else
    pft->stand->cell->output.pft_nuptake[pft->par->id]+=n_uptake;
  pft->stand->cell->balance.n_uptake+=n_uptake*pft->stand->frac;
  return n_uptake;
} /* of 'nuptake_tree' */

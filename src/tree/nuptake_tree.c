/**************************************************************************************/
/**                                                                                \n**/
/**               n  u  p  t  a  k  e  _  t  r  e  e  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml/lpjml                             \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "tree.h"
#include "agriculture.h"

Real nuptake_tree(Pft *pft,
                  Real *n_plant_demand,
                  Real *ndemand_leaf,
                  int npft,
                  int nbiomass,
                  int ncft
                 )
{

  Pfttree *tree;
  Soil *soil;
  const Pfttreepar *treepar;
  Real NO3_up=0;
  Real NCplant,ndemand_leaf_opt,NC_actual,NC_leaf;
  Real f_NCplant=0;
  Real up_temp_f;
  Real totn,nsum;
  Real n_uptake=0;
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
    totn=(soil->NO3[l]+soil->NH4[l]);
    if(totn>0 && soil->temp[l]>0)
    //if(totn>0)
    {
      //up_temp_f = max((0.0326 + 0.0035 * pow(soil->temp[l],1.652) - pow((soil->temp[l]/41.748),7.19)),0); /*Eq. C5 in Smith et al. 2014*/
      up_temp_f = nuptake_temp_fcn(soil->temp[l]);
      //up_temp_f=1;
      NO3_up = 2*pft->par->vmax_up*(pft->par->kNmin +totn/(totn+pft->par->KNmin*soil->par->wsat*soildepth[l]/1000))* up_temp_f * f_NCplant * (tree->ind.root.carbon*pft->nind+pft->bm_inc.carbon*tree->falloc.root)/1000; //Smith et al. Eq. C14-C15, Navail=totn
        /* reducing uptake according to availability */
      if(NO3_up>totn)
        NO3_up=totn;
      n_uptake+=NO3_up*rootdist_n[l];
      nsum+=totn*rootdist_n[l];
    }
  }
#ifdef DEBUG_N
  printf("TREE n_uptake=%g, nplant_demand=%g vegn=%g\n",n_uptake,*n_plant_demand,(vegn_sum_tree(pft)-tree->ind.heartwood.nitrogen*pft->nind));
#endif

  if (n_uptake>*n_plant_demand-(vegn_sum_tree(pft)-tree->ind.heartwood.nitrogen*pft->nind))
    n_uptake=*n_plant_demand-(vegn_sum_tree(pft)-tree->ind.heartwood.nitrogen*pft->nind);
  if(n_uptake<=0)
    n_uptake=0;
  else
  {
    pft->bm_inc.nitrogen+=n_uptake;
    forrootsoillayer(l)
      if(soil->temp[l]>0)
      {
        soil->NO3[l]-=soil->NO3[l]*rootdist_n[l]*n_uptake/nsum;
        if(soil->NO3[l]<0)
        {
           pft->bm_inc.nitrogen+=soil->NO3[l];
           soil->NO3[l]=0;
        }
        soil->NH4[l]-=soil->NH4[l]*rootdist_n[l]*n_uptake/nsum;
        if(soil->NH4[l]<0)
        {
           pft->bm_inc.nitrogen+=soil->NH4[l];
           soil->NH4[l]=0;
        }
      }
  }
  if(*n_plant_demand*(1-pft->par->knstore)>(vegn_sum_tree(pft)-tree->ind.heartwood.nitrogen*pft->nind))   /*HERE RECALCULATION OF N-demand TO N-supply*/
  {
    *n_plant_demand=(vegn_sum_tree(pft)-tree->ind.heartwood.nitrogen*pft->nind);
    NC_actual=(vegn_sum_tree(pft)-tree->ind.heartwood.nitrogen*pft->nind)/(vegc_sum_tree(pft)-tree->ind.heartwood.carbon*pft->nind);
    NC_leaf=NC_actual/(tree->falloc.sapwood/treepar->ratio.sapwood+tree->falloc.root/treepar->ratio.root+tree->falloc.leaf);
    if(NC_leaf< pft->par->ncleaf.low)
        NC_leaf=pft->par->ncleaf.low;
    else if (NC_leaf>pft->par->ncleaf.high)
        NC_leaf=pft->par->ncleaf.high;
    *ndemand_leaf=(tree->ind.leaf.carbon*pft->nind+pft->bm_inc.carbon*tree->falloc.leaf)*NC_leaf;
  }

  if(ndemand_leaf_opt<epsilon)
    pft->vscal+=1;
  else
   pft->vscal+=min(1,*ndemand_leaf/(ndemand_leaf_opt/(1+pft->par->knstore))); /*eq. C20 in Smith et al. 2014, Biogeosciences */
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

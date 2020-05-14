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
                   int nbiomass,         /**< number of biomass PFTs */
                   int ncft              /**< number of crop PFTs */
                  )                      /** \return nitrogen uptake (gN/m2/day) */
{
  Soil *soil;
  Pftgrass *grass;
  Pftgrasspar *grasspar;
  Real NO3_up=0;
  Real NCplant,ndemand_leaf_opt,NC_actual,NC_leaf;
  Real f_NCplant;
  Real up_temp_f;
  Real totn,nsum;
  Real wscaler;
  Real n_uptake=0;
  Real n_upfail=0; /**< track n_uptake that is not available from soil for output reporting */
  Real rootdist_n[LASTLAYER];
  Irrigation *data;
  int l;
  soil=&pft->stand->soil;
  getrootdist(rootdist_n,pft->par->rootdist,soil->mean_maxthaw);
  data=pft->stand->data;
  grass=pft->data;
  grasspar=pft->par->data;
  ndemand_leaf_opt=*ndemand_leaf;

  NCplant = (grass->ind.leaf.nitrogen+ grass->ind.root.nitrogen) / (grass->ind.leaf.carbon+ grass->ind.root.carbon); /* Plant's mobile nitrogen concentration, Eq.9, Zaehle&Friend 2010 Supplementary */

  f_NCplant = min(max(((NCplant-pft->par->ncleaf.high)/(pft->par->ncleaf.low-pft->par->ncleaf.high)),0),1);
  /* reducing uptake according to availability */
  nsum=0;
  forrootsoillayer(l)
  {
    wscaler=(soil->w[l]+soil->ice_depth[l]/soil->par->whcs[l]>0) ? (soil->w[l]/(soil->w[l]+soil->ice_depth[l]/soil->par->whcs[l])) : 0;
    //wscaler=1;
    totn=(soil->NO3[l]+soil->NH4[l])*wscaler;
    if(totn>0)
    {
      /*Thornley 1991*/
      up_temp_f = nuptake_temp_fcn(soil->temp[l]);
      NO3_up = 2*pft->par->vmax_up*(pft->par->kNmin+totn/(totn+pft->par->KNmin*soil->par->wsat*soildepth[l]/1000))* up_temp_f * f_NCplant * grass->ind.root.carbon*pft->nind/1000;
      /* reducing uptake according to availability */
     if(NO3_up>totn)
        NO3_up=totn;
      n_uptake+=NO3_up*rootdist_n[l];
      nsum+=totn*rootdist_n[l];
    }
  }
  if(nsum==0)
    n_uptake=0;
  if (n_uptake>*n_plant_demand-vegn_sum_grass(pft))
    n_uptake=*n_plant_demand-vegn_sum_grass(pft);
  if(n_uptake<=0)
    n_uptake=0;
  else
  {
    pft->bm_inc.nitrogen+=n_uptake;
    forrootsoillayer(l)
    {
      wscaler=(soil->w[l]+soil->ice_depth[l]/soil->par->whcs[l]>0) ? (soil->w[l]/(soil->w[l]+soil->ice_depth[l]/soil->par->whcs[l])) : 0;
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
  if(*n_plant_demand/(1+pft->par->knstore)>vegn_sum_grass(pft))
  {
    *n_plant_demand=vegn_sum_grass(pft);
    NC_actual=vegn_sum_grass(pft)/vegc_sum_grass(pft);
    NC_leaf=NC_actual/(grass->falloc.root/grasspar->ratio+grass->falloc.leaf);
    if(NC_leaf< pft->par->ncleaf.low)
      NC_leaf=pft->par->ncleaf.low;
    else if (NC_leaf>pft->par->ncleaf.high)
      NC_leaf=pft->par->ncleaf.high;
//    *ndemand_leaf=(grass->ind.leaf.carbon*pft->nind+pft->bm_inc.carbon*grass->falloc.leaf)*NC_leaf;
    *ndemand_leaf=(grass->ind.leaf.carbon*pft->nind)*NC_leaf;
  }
  if(ndemand_leaf_opt<epsilon)
    pft->vscal+=1;
  else
    pft->vscal+=min(1,*ndemand_leaf/(ndemand_leaf_opt/(1+pft->par->knstore)));
  /* correcting for failed uptake from depleted soils in outputs */
  n_uptake+=n_upfail;
  if(pft->stand->type->landusetype==NATURAL || pft->stand->type->landusetype==SETASIDE_RF || pft->stand->type->landusetype==SETASIDE_IR)
  {
    pft->stand->cell->output.pft_nuptake[pft->par->id]+=n_uptake;
  }
  else if(pft->stand->type->landusetype==BIOMASS_GRASS)
  {
    pft->stand->cell->output.pft_nuptake[(npft-nbiomass)+rbgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=n_uptake; /* *stand->cell->ml.landfrac[data->irrigation].biomass_grass; */
  }
  else
  {
    pft->stand->cell->output.pft_nuptake[(npft-nbiomass)+rothers(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=n_uptake;/*pft->stand->cell->ml.landfrac[data->irrigation].grass[0];*/
    pft->stand->cell->output.pft_nuptake[(npft-nbiomass)+rmgrass(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=n_uptake;/*pft->stand->cell->ml.landfrac[data->irrigation].grass[1];*/
  }
  pft->stand->cell->balance.n_uptake+=n_uptake*pft->stand->frac;
  return n_uptake;
} /* of 'nuptake_grass' */

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

Real nuptake_grass(Pft *pft,
                   Real *n_plant_demand,
                   Real *ndemand_leaf,
                   int npft,
                   int nbiomass,
                   int ncft
                  )
{
  Soil *soil;
  Pftgrass *grass;
  Pftgrasspar *grasspar;
  Real NO3_up=0;
  Real NCplant,ndemand_leaf_opt,NC_actual,NC_leaf;
  Real f_NCplant;
  Real up_temp_f;
  Real totn,nsum;
  Real n_uptake=0;
  Real rootdist_n[LASTLAYER];
  Irrigation *data;
  int l;
  soil=&pft->stand->soil;
  getrootdist(rootdist_n,pft->par->rootdist,soil->mean_maxthaw);
  data=pft->stand->data;
  grass=pft->data;
  grasspar=pft->par->data;
  ndemand_leaf_opt=*ndemand_leaf;

 // NCplant = (grass->ind.leaf.nitrogen + grass->ind.root.nitrogen + pft->bm_inc.nitrogen*(1-pft->par->knstore)*grass->falloc.root + pft->bm_inc.nitrogen*(1-pft->par->knstore)*grass->falloc.leaf) / (grass->ind.leaf.carbon+ grass->ind.root.carbon + pft->bm_inc.carbon*grass->falloc.leaf + pft->bm_inc.carbon*grass->falloc.root);         ; /* Plant's mobile nitrogen concentration */
  NCplant = (grass->ind.leaf.nitrogen+ grass->ind.root.nitrogen) / (grass->ind.leaf.carbon+ grass->ind.root.carbon); /* Plant's mobile nitrogen concentration, Eq.9, Zaehle&Friend 2010 Supplementary */
  //NCplant = (pft->bm_inc.nitrogen*(1-pft->par->knstore)*grass->falloc.root + pft->bm_inc.nitrogen*(1-pft->par->knstore)*grass->falloc.leaf) / (pft->bm_inc.carbon*grass->falloc.leaf + pft->bm_inc.carbon*grass->falloc.root);         ; /* Plant's mobile nitrogen concentration */

  f_NCplant = min(max(((NCplant-pft->par->ncleaf.high)/(pft->par->ncleaf.low-pft->par->ncleaf.high)),0),1);
  //printf("low,high,%g %g\n",pft->par->ncleaf.low,pft->par->ncleaf.high);
  //printf("f_NCplant=%g, NCplant=%g\n",f_NCplant,NCplant);
  /* reducing uptake according to availability */
  nsum=0;
  forrootsoillayer(l)
  {
    totn=(soil->NO3[l]+soil->NH4[l]);
    if(totn>0 && soil->temp[l]>0)
    //if(totn>0)
    {
      //up_temp_f = max((0.0326 + 0.0035 * pow(soil->temp[l],1.652) - pow((soil->temp[l]/41.748),7.19)),0); /*Eq. C5 in Smith et al. 2014*/
      /*Thornley 1991*/
      up_temp_f = nuptake_temp_fcn(soil->temp[l]);
      //up_temp_f=1;
      //NO3_up2 = pft->par->vmax_up*1.0e-6 * totn * (pft->par->kNmin + (1/(totn*pft->par->KNmin))) * up_temp_f * f_NCplant * grass->ind.root.carbon; /*this formulation leeds to wrong uptake*/
      NO3_up = 2*pft->par->vmax_up*(pft->par->kNmin+totn/(totn+pft->par->KNmin*soil->par->wsat*soildepth[l]/1000))* up_temp_f * f_NCplant * grass->ind.root.carbon*pft->nind/1000;
      /* reducing uptake according to availability */
      //printf("fNO3_p=%g 2:%g totn %g u_temp= %g carbon_root: %g vmax: %g  %g\n",NO3_up,NO3_up2 ,totn,up_temp_f,grass->ind.root.carbon ,pft->par->vmax_up,(pft->par->kNmin+totn/(totn+pft->par->KNmin*allwater(soil,l)*soildepth[l]/1000)));
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
  if(*n_plant_demand*(1-pft->par->knstore)>vegn_sum_grass(pft))
  {
    *n_plant_demand=vegn_sum_grass(pft);
    NC_actual=vegn_sum_grass(pft)/vegc_sum_grass(pft);
    NC_leaf=NC_actual/(grass->falloc.root/grasspar->ratio+grass->falloc.leaf);
    if(NC_leaf< pft->par->ncleaf.low)
        NC_leaf=pft->par->ncleaf.low;
    else if (NC_leaf>pft->par->ncleaf.high)
        NC_leaf=pft->par->ncleaf.high;
    *ndemand_leaf=(grass->ind.leaf.carbon*pft->nind+pft->bm_inc.carbon*grass->falloc.leaf)*NC_leaf;
  }
  if(ndemand_leaf_opt<epsilon)
    pft->vscal+=1;
  else
    pft->vscal+=min(1,*ndemand_leaf/(ndemand_leaf_opt/(1+pft->par->knstore)));
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

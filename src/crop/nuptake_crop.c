/**************************************************************************************/
/**                                                                                \n**/
/**               n  u  p  t  a  k  e  _  c  r  o  p  .  c                         \n**/
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
#include "crop.h"
#include "agriculture.h"

Real nuptake_crop(Pft *pft,             /**< pointer to PFT data */
                  Real *n_plant_demand, /**< total N plant demand */
                  Real *ndemand_leaf,   /**< N demand of leafs */
                  int npft,             /**< number of natural PFTs */
                  int ncft,             /**< number of crop PFTs */
                  const Config *config  /**< LPJmL configuration */
                 )                      /** \return nitrogen uptake (gN/m2/day) */
{
#ifdef SAFE
  String line;
#endif
  Soil *soil;
  Pftcrop *crop;
  Irrigation *data;
  Real NO3_up[LASTLAYER],NH4_up[LASTLAYER];
  Real ndemand_leaf_opt;
  Real NCplant=0;
  Real f_NCplant=0;
  Real up_temp_f=0;
  Real wscaler;
  Real n_uptake=0;
  Real nupsum=0;
  Real fixed_n=0;
  Real n_deficit=0.0;
  Real autofert_n=0;
  Real rootdist_n[LASTLAYER];
  Real nc_ratio;
  int l,nirrig,nnat,index;
  soil=&pft->stand->soil;
  if(config->permafrost)
    getrootdist(rootdist_n,pft->par->rootdist,soil->mean_maxthaw);
  else
    forrootsoillayer(l)
      rootdist_n[l]=pft->par->rootdist[l];
  nnat=getnnat(npft,config);
  nirrig=getnirrig(ncft,config);
  index=(pft->stand->type->landusetype==AGRICULTURE) ? pft->par->id-npft : rothers(ncft);

  crop=pft->data;
  data=pft->stand->data;
  if(crop->ind.leaf.carbon+crop->ind.root.carbon==0)
    return 0;
  //fprintcropphys2(stdout,crop->ind,pft->nind);

  NCplant = (crop->ind.leaf.nitrogen + crop->ind.root.nitrogen) / (crop->ind.leaf.carbon + crop->ind.root.carbon); /* Plant's mobile nitrogen concentration, Eq.9, Zaehle&Friend 2010 Supplementary */
  f_NCplant = min(max(((NCplant-pft->par->ncleaf.high)/(2.0/(1.0/pft->par->ncleaf.low+1.0/pft->par->ncleaf.high)-pft->par->ncleaf.high)),0),1); /* consistent with Smith et al. 2014 */
#ifdef DEBUG_N
  printf("f_NCplant=%g\n",f_NCplant);
#endif
  ndemand_leaf_opt=*ndemand_leaf;
  if(crop->ind.leaf.carbon==0)
    nc_ratio = pft->par->ncleaf.low;
  else
    nc_ratio = crop->ind.leaf.nitrogen/crop->ind.leaf.carbon;
  if(nc_ratio<(pft->par->ncleaf.high*(1+pft->par->knstore)))
  {
    forrootsoillayer(l)
    {
      wscaler=soil->w[l]>epsilon ? 1 : 0;
      up_temp_f=nuptake_temp_fcn(soil->temp[l]);
      if(soil->NO3[l]>0)
      {
        NO3_up[l]=pft->par->vmax_up*(pft->par->kNmin+soil->NO3[l]*wscaler/(soil->NO3[l]*wscaler+pft->par->KNmin*soil->wsat[l]*soildepth[l]/1000))*up_temp_f*
            f_NCplant*(crop->ind.root.carbon*pft->nind)*rootdist_n[l]/1000; //Smith et al. Eq. C14-C15, Navail=totn
        /* reducing uptake according to availability */
        if(NO3_up[l]>soil->NO3[l])
          NO3_up[l]=soil->NO3[l];
        nupsum+=NO3_up[l];
#ifdef DEBUG_N
        printf("layer %d NO3_up=%g\n",l,NO3_up);
#endif
      }
      else
        NO3_up[l]=0;
      if(soil->NH4[l]>0)
      {
        NH4_up[l]=pft->par->vmax_up*(pft->par->kNmin+soil->NH4[l]*wscaler/(soil->NH4[l]*wscaler+pft->par->KNmin*soil->wsat[l]*soildepth[l]/1000))*up_temp_f*
            f_NCplant*(crop->ind.root.carbon*pft->nind)*rootdist_n[l]/1000;
        /* reducing uptake according to availability */
        if(NH4_up[l]>soil->NH4[l])
          NH4_up[l]=soil->NH4[l];
        nupsum+=NH4_up[l];
      }
      else
        NH4_up[l]=0;
    }
    if(nupsum==0)
      n_uptake=0;
    else
    {
      if(n_uptake>*n_plant_demand-pft->bm_inc.nitrogen)
        n_uptake=*n_plant_demand-pft->bm_inc.nitrogen;
      else
        n_uptake=nupsum;
      pft->bm_inc.nitrogen+=n_uptake;
      forrootsoillayer(l)
      {
        soil->NO3[l]-=NO3_up[l]*n_uptake/nupsum;
        soil->NH4[l]-=NH4_up[l]*n_uptake/nupsum;      
#ifdef SAFE
        if (soil->NO3[l]<-epsilon)
          fail(NEGATIVE_SOIL_NO3_ERR,TRUE,TRUE,"Cell (%s) NO3=%g<0 in layer %d, NO3_up=%g, nuptake=%g, nupsum=%g",
                sprintcoord(line,&pft->stand->cell->coord),soil->NO3[l],l,NO3_up[l],n_uptake,nupsum);
        if (soil->NH4[l]<-epsilon)
          fail(NEGATIVE_SOIL_NO3_ERR,TRUE,TRUE,"Cell (%s) NH4=%g<0 in layer %d, NH4_up=%g, nuptake=%g, nupsum=%g",
                sprintcoord(line,&pft->stand->cell->coord),soil->NH4[l],l,NH4_up[l],n_uptake,nupsum);
#endif
      }
    }
  }
  crop->ndemandsum += max(0, *n_plant_demand - pft->bm_inc.nitrogen);
  if(*n_plant_demand > pft->bm_inc.nitrogen)
  {
    /* no N limitation for N-fixing crops */
    if (pft->par->nfixing)
    {
      if(!config->npp_controlled_bnf)
      {
        fixed_n = *n_plant_demand - pft->bm_inc.nitrogen;
        n_uptake += fixed_n;
        pft->bm_inc.nitrogen = *n_plant_demand;
        getoutput(&pft->stand->cell->output,BNF,config) += fixed_n*pft->stand->frac;
        pft->stand->cell->balance.influx.nitrogen += fixed_n*pft->stand->frac;
        getoutput(&pft->stand->cell->output,BNF_AGR,config) += fixed_n*pft->stand->frac;
        getoutput(&pft->stand->cell->output,BNF_MG,config) += fixed_n*pft->stand->frac;
        pft->vscal = 1;
      }
      else
      {
        n_deficit = *n_plant_demand-pft->bm_inc.nitrogen;
        if(n_deficit>0 && pft->npp_bnf>0)
        {
          fixed_n=npp_contr_biol_n_fixation(pft, soil, n_deficit, config);
          pft->bm_inc.nitrogen+=fixed_n;
          getoutput(&pft->stand->cell->output,BNF,config)+=fixed_n*pft->stand->frac;
          pft->stand->cell->balance.influx.nitrogen+=fixed_n*pft->stand->frac;
          getoutput(&pft->stand->cell->output,BNF_AGR,config) += fixed_n*pft->stand->frac;
          getoutput(&pft->stand->cell->output,BNF_MG,config) += fixed_n*pft->stand->frac;
        }
        else
          pft->npp_bnf=0.0;

      }
    }
    else
    {
      if(config->fertilizer_input==AUTO_FERTILIZER)
      {
        autofert_n = *n_plant_demand - pft->bm_inc.nitrogen;
        if(autofert_n>0)
        {
          n_uptake += autofert_n;
          pft->bm_inc.nitrogen = *n_plant_demand;
          if(crop->sh!=NULL)
            crop->sh->nfertsum+=autofert_n*pft->stand->frac;
          else
            getoutputindex(&pft->stand->cell->output,CFT_NFERT,index+data->irrigation*nirrig,config)+=autofert_n;
          pft->stand->cell->balance.influx.nitrogen += autofert_n*pft->stand->frac;
          getoutput(&pft->stand->cell->output,FLUX_AUTOFERT,config)+=autofert_n*pft->stand->frac;
        }
        pft->vscal = 1;
        pft->npp_bnf=0.0;
      }
      else
      {
        //*n_plant_demand=pft->bm_inc.nitrogen;
        //*ndemand_leaf=pft->bm_inc.nitrogen*crop->ind.leaf.carbon/(crop->ind.leaf.carbon+(crop->ind.root.carbon/croppar->ratio.root+crop->ind.pool.carbon/croppar->ratio.pool+crop->ind.so.carbon/croppar->ratio.so)); /*these parameters need to be in pft.par and need to be checked as well)*/
        if(crop->ind.leaf.carbon==0)
          *ndemand_leaf=0;
        else
          *ndemand_leaf=(crop->ind.leaf.carbon*pft->nind)*(crop->ind.leaf.nitrogen)/(crop->ind.leaf.carbon);
        if(ndemand_leaf_opt<epsilon)
          pft->vscal=1;
        else
          pft->vscal=min(1,(*ndemand_leaf/(ndemand_leaf_opt/(1+pft->par->knstore)))); /*eq. C20 in Smith et al. 2014, Biogeosciences */
      }
    }
  }
  else
    pft->vscal = 1;
#ifdef DEBUG_N
  printf("ndemand=%g,ndemand_opt=%g\n",*ndemand_leaf,ndemand_leaf_opt);
#endif
  if(crop->sh!=NULL)
    crop->nuptakesum += n_uptake;
  else
    getoutputindex(&pft->stand->cell->output,PFT_NUPTAKE,nnat+index+data->irrigation*nirrig,config)+=n_uptake;
  if(config->pft_output_scaled)
    getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+index+data->irrigation*nirrig,config)+=fixed_n*pft->stand->frac;
  else
    getoutputindex(&pft->stand->cell->output,PFT_BNF,nnat+index+data->irrigation*nirrig,config)+=fixed_n;
  getoutputindex(&pft->stand->cell->output,PFT_NDEMAND,nnat+index+data->irrigation*nirrig,config)+=max(0,*n_plant_demand-pft->bm_inc.nitrogen)/365;
  pft->stand->cell->balance.n_uptake+=n_uptake*pft->stand->frac;
  pft->stand->cell->balance.n_demand+=max(0,(*n_plant_demand-pft->bm_inc.nitrogen))*pft->stand->frac/365;
  return n_uptake;
} /* of 'nuptake_crop' */

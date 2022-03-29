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
  String line;
  Soil *soil;
  Pftcrop *crop;
  const Pftcroppar *croppar;
  Irrigation *data;
  Real ndemand_leaf_opt,NO3_up=0;
  Real NCplant=0;
  Real f_NCplant=0;
  Real up_temp_f=0;
  Real totn,nsum;
  Real wscaler;
  Real n_uptake=0;
  Real n_upfail=0; /**< track n_uptake that is not available from soil for output reporting */
  Real fixed_n=0;
  Real n_deficit=0.0;
  Real autofert_n=0;
  Real rootdist_n[LASTLAYER];
  int l,nirrig;
  soil=&pft->stand->soil;
  if(config->permafrost)
    getrootdist(rootdist_n,pft->par->rootdist,soil->mean_maxthaw);
  else
    forrootsoillayer(l)
      rootdist_n[l]=pft->par->rootdist[l];
  nirrig=getnirrig(ncft,config);

  crop=pft->data;
  croppar=pft->par->data;
  data=pft->stand->data;
  if(crop->ind.leaf.carbon+crop->ind.root.carbon==0)
    return 0;
  //fprintcropphys2(stdout,crop->ind,pft->nind);

  NCplant = (crop->ind.leaf.nitrogen + crop->ind.root.nitrogen) / (crop->ind.leaf.carbon + crop->ind.root.carbon); /* Plant's mobile nitrogen concentration, Eq.9, Zaehle&Friend 2010 Supplementary */
  f_NCplant = min(max(((NCplant-pft->par->ncleaf.high)/(pft->par->ncleaf.low-pft->par->ncleaf.high)),0),1); /*Eq.10, Zaehle&Friend 2010 Supplementary*/
  /* disabling N uptake response to plant C:N ratio status */
  f_NCplant=1;
#ifdef DEBUG_N
  printf("f_NCplant=%g\n",f_NCplant);
#endif
  ndemand_leaf_opt=*ndemand_leaf;
  nsum=0;
  if((crop->ind.leaf.nitrogen/crop->ind.leaf.carbon)<(pft->par->ncleaf.high*(1+pft->par->knstore)))
    forrootsoillayer(l)
    {
      wscaler=(soil->w[l]+soil->ice_depth[l]/soil->whcs[l]>0) ? (soil->w[l]/(soil->w[l]+soil->ice_depth[l]/soil->whcs[l])) : 0;
      totn=(soil->NO3[l]+soil->NH4[l])*wscaler;
      if(totn > 0)
      {
        up_temp_f = nuptake_temp_fcn(soil->temp[l]);
        NO3_up = 2*pft->par->vmax_up*(pft->par->kNmin+totn/(totn+pft->par->KNmin*soil->wsat[l]*soildepth[l]/1000))*up_temp_f*f_NCplant*(crop->ind.root.carbon*pft->nind)*rootdist_n[l]/1000; //Smith et al. Eq. C14-C15, Navail=totn
#ifdef DEBUG_N
      printf("layer %d NO3_up=%g\n",l,NO3_up);
#endif
        /* reducing uptake according to availability */
        if(NO3_up>totn)
          NO3_up=totn;
        n_uptake+=NO3_up;
        nsum+=totn*rootdist_n[l];
      }
    }
  if(nsum==0)
    n_uptake=0;
  else
  {
    if (n_uptake>*n_plant_demand-pft->bm_inc.nitrogen)
      n_uptake=*n_plant_demand-pft->bm_inc.nitrogen;
    if(n_uptake<=0)
      n_uptake=0;
    else
    {
      pft->bm_inc.nitrogen+=n_uptake;
      forrootsoillayer(l)
      {

        wscaler=(soil->w[l]+soil->ice_depth[l]/soil->whcs[l]>0) ? (soil->w[l]/(soil->w[l]+soil->ice_depth[l]/soil->whcs[l])) : 0;
        soil->NO3[l]-=(soil->NO3[l]*wscaler*rootdist_n[l]*n_uptake)/nsum;
        soil->NH4[l]-=soil->NH4[l]*wscaler*rootdist_n[l]*n_uptake/nsum;
        if(soil->NO3[l]<0)
        {
          pft->bm_inc.nitrogen+=soil->NO3[l];
          n_upfail+=soil->NO3[l];
          soil->NO3[l]=0;
        }
        if(soil->NH4[l]<0)
        {
          pft->bm_inc.nitrogen+=soil->NH4[l];
          n_upfail+=soil->NH4[l];
          soil->NH4[l]=0;
        }
#ifdef SAFE
        if (soil->NO3[l]<-epsilon)
          fail(NEGATIVE_SOIL_NO3_ERR,TRUE,"Cell (%s) NO3=%g<0 in layer %d, nuptake=%g, nsum=%g",
               sprintcoord(line,&pft->stand->cell->coord),soil->NO3[l],l,n_uptake,nsum);
        if (soil->NH4[l]<-epsilon)
          fail(NEGATIVE_SOIL_NO3_ERR,TRUE,"Cell (%s) NH4=%g<0 in layer %d, nuptake=%g, nsum=%g",
               sprintcoord(line,&pft->stand->cell->coord),soil->NH4[l],l,n_uptake,nsum);

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
      if(!config->ma_bnf)
      {
        fixed_n = *n_plant_demand - pft->bm_inc.nitrogen;
        n_uptake += fixed_n;
        pft->bm_inc.nitrogen = *n_plant_demand;
        getoutput(&pft->stand->cell->output,BNF,config) += fixed_n*pft->stand->frac;
        pft->stand->cell->balance.n_influx += fixed_n*pft->stand->frac;
        getoutput(&pft->stand->cell->output,BNF_AGR,config) += fixed_n*pft->stand->frac;
        pft->vscal = 1;
      }
      if(config->ma_bnf)
      {
        n_deficit = *n_plant_demand-pft->bm_inc.nitrogen;
        if(n_deficit>0 && pft->npp_bnf>0)
        {
          fixed_n=ma_biological_n_fixation(pft, soil, n_deficit, config);
          pft->bm_inc.nitrogen+=fixed_n;
          getoutput(&pft->stand->cell->output,BNF,config)+=fixed_n*pft->stand->frac;
          pft->stand->cell->balance.n_influx+=fixed_n*pft->stand->frac;
          getoutput(&pft->stand->cell->output,BNF_AGR,config) += fixed_n*pft->stand->frac;
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
        n_uptake += autofert_n;
        pft->bm_inc.nitrogen = *n_plant_demand;
        pft->vscal = 1;
        if(config->double_harvest)
          crop->dh->nfertsum+=autofert_n*pft->stand->frac;
        else
          getoutputindex(&pft->stand->cell->output,CFT_NFERT,pft->par->id-npft+data->irrigation*nirrig,config)+=autofert_n;
        pft->stand->cell->balance.n_influx += autofert_n*pft->stand->frac;
        getoutput(&pft->stand->cell->output,FLUX_AUTOFERT,config)+=autofert_n*pft->stand->frac;
      }
      else
      {
        //*n_plant_demand=pft->bm_inc.nitrogen;
        //*ndemand_leaf=pft->bm_inc.nitrogen*crop->ind.leaf.carbon/(crop->ind.leaf.carbon+(crop->ind.root.carbon/croppar->ratio.root+crop->ind.pool.carbon/croppar->ratio.pool+crop->ind.so.carbon/croppar->ratio.so)); /*these parameters need to be in pft.par and need to be checked as well)*/
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
  /* correcting for failed uptake from depleted soils in outputs */
  n_uptake+=n_upfail;
#ifdef DEBUG_N
  printf("ndemand=%g,ndemand_opt=%g\n",*ndemand_leaf,ndemand_leaf_opt);
#endif

  if(crop->dh!=NULL)
    crop->nuptakesum += n_uptake;
  else
    getoutputindex(&pft->stand->cell->output,PFT_NUPTAKE,(pft->par->id-config->nbiomass-config->nagtree-config->nwft)+data->irrigation*nirrig,config)+=n_uptake;
  getoutputindex(&pft->stand->cell->output,PFT_NDEMAND,(pft->par->id-config->nbiomass-config->nagtree-config->nwft)+data->irrigation*nirrig,config)+=max(0,*n_plant_demand-pft->bm_inc.nitrogen)/365;
  pft->stand->cell->balance.n_uptake+=n_uptake*pft->stand->frac;
  pft->stand->cell->balance.n_demand+=max(0,(*n_plant_demand-pft->bm_inc.nitrogen))*pft->stand->frac/365;
  if(pft->par->id==config->crop_index && data->irrigation==config->crop_irrigation)
  {
    getoutput(&pft->stand->cell->output,D_NUPTAKE,config)+=n_uptake;
    getoutput(&pft->stand->cell->output,D_VSCAL,config)+=pft->vscal;
  }
   return n_uptake;
} /* of 'nuptake_crop' */

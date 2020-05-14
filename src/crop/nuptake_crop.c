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
                  int UNUSED(npft),     /**< number of natural PFTs */
                  int nbiomass,         /**< number of biomass PFTs */
                  int ncft              /**< number of crop PFTs */
                 )                      /** \return nitrogen uptake (gN/m2/day) */
{
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
  Real rootdist_n[LASTLAYER];
  int l;
  soil=&pft->stand->soil;
  getrootdist(rootdist_n,pft->par->rootdist,soil->mean_maxthaw);
  crop=pft->data;
  croppar=pft->par->data;
  data=pft->stand->data;
  if(crop->ind.leaf.carbon+crop->ind.root.carbon==0)
    return 0;
  //fprintcropphys2(stdout,crop->ind,pft->nind);

  NCplant = (crop->ind.leaf.nitrogen + crop->ind.root.nitrogen) / (crop->ind.leaf.carbon + crop->ind.root.carbon); /* Plant's mobile nitrogen concentration, Eq.9, Zaehle&Friend 2010 Supplementary */
  f_NCplant = min(max(((NCplant-pft->par->ncleaf.high)/(pft->par->ncleaf.low-pft->par->ncleaf.high)),0),1); /*Eq.10, Zaehle&Friend 2010 Supplementary*/
#ifdef DEBUG_N
  printf("f_NCplant=%g\n",f_NCplant);
#endif
  ndemand_leaf_opt=*ndemand_leaf;
  nsum=0;
  forrootsoillayer(l)
  {
    wscaler=(soil->w[l]+soil->ice_depth[l]/soil->par->whcs[l]>0) ? (soil->w[l]/(soil->w[l]+soil->ice_depth[l]/soil->par->whcs[l])) : 0;
    totn=(soil->NO3[l]+soil->NH4[l])*wscaler;
    if(totn > 0)
    {
      up_temp_f = nuptake_temp_fcn(soil->temp[l]);
      NO3_up = 2*pft->par->vmax_up*(pft->par->kNmin +totn/(totn+pft->par->KNmin*soil->par->wsat*soildepth[l]/1000))* up_temp_f * f_NCplant * (crop->ind.root.carbon*pft->nind)/1000; //Smith et al. Eq. C14-C15, Navail=totn
#ifdef DEBUG_N
      printf("layer %d NO3_up=%g\n",l,NO3_up);
#endif
      /* reducing uptake according to availability */
      if(NO3_up>totn)
        NO3_up=totn;
      n_uptake+=NO3_up*rootdist_n[l];
      nsum+=totn*rootdist_n[l];
    }
  }
  if(nsum==0)
    n_uptake=0;
  if (n_uptake>*n_plant_demand-pft->bm_inc.nitrogen)
    n_uptake=*n_plant_demand-pft->bm_inc.nitrogen;
  if(n_uptake<=0)
    n_uptake=0;
  else
  {
    pft->bm_inc.nitrogen+=n_uptake;
    forrootsoillayer(l)
    {
      wscaler=(soil->w[l]+soil->ice_depth[l]/soil->par->whcs[l]>0) ? (soil->w[l]/(soil->w[l]+soil->ice_depth[l]/soil->par->whcs[l])) : 0;
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
        fail(NEGATIVE_SOIL_NO3_ERR,TRUE,"Pixel: %.2f %.2f NO3=%g<0 in layer %d, nuptake=%g, nsum=%g",
             pft->stand->cell->coord.lat,pft->stand->cell->coord.lon,soil->NO3[l],l,n_uptake,nsum);
      if (soil->NH4[l]<-epsilon)
        fail(NEGATIVE_SOIL_NO3_ERR,TRUE,"Pixel: %.2f %.2f NH4=%g<0 in layer %d, nuptake=%g, nsum=%g",
             pft->stand->cell->coord.lat,pft->stand->cell->coord.lon,soil->NH4[l],l,n_uptake,nsum);

#endif
    }
  }
  crop->ndemandsum += max(0, *n_plant_demand - pft->bm_inc.nitrogen);
  if (*n_plant_demand > pft->bm_inc.nitrogen)
  {
    /* no N limitation for N-fixing crops */
    if (pft->par->id == OIL_CROPS_SOYBEAN || pft->par->id == PULSES)
    {
      fixed_n = *n_plant_demand - pft->bm_inc.nitrogen;
      n_uptake += fixed_n;
      pft->bm_inc.nitrogen = *n_plant_demand;
      pft->stand->cell->output.mbnf += fixed_n*pft->stand->frac;
      pft->vscal = 1;
    }
    else
    {
      *n_plant_demand = pft->bm_inc.nitrogen;
      *ndemand_leaf = *n_plant_demand*crop->ind.leaf.carbon / (crop->ind.leaf.carbon + (crop->ind.root.carbon / croppar->ratio.root + crop->ind.pool.carbon / croppar->ratio.pool + crop->ind.so.carbon / croppar->ratio.so)); /*these parameters need to be in pft.par and need to be checked as well)*/
      if (ndemand_leaf_opt < epsilon)
        pft->vscal = 1;
      else
        pft->vscal = min(1, (*ndemand_leaf / (ndemand_leaf_opt / (1 + pft->par->knstore)))); /*eq. C20 in Smith et al. 2014, Biogeosciences */
    }
  }
  else
    pft->vscal = 1;
  /* correcting for failed uptake from depleted soils in outputs */
  n_uptake+=n_upfail;
  crop->nuptakesum += n_uptake;
#ifdef DEBUG_N
  printf("ndemand=%g,ndemand_opt=%g\n",*ndemand_leaf,ndemand_leaf_opt);
#endif

   pft->stand->cell->output.pft_nuptake[(pft->par->id-nbiomass)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=n_uptake;
   pft->stand->cell->balance.n_uptake+=n_uptake*pft->stand->frac;
   if(pft->par->id==pft->stand->cell->output.daily.cft && data->irrigation==pft->stand->cell->output.daily.irrigation)
   {
     pft->stand->cell->output.daily.nuptake=n_uptake;
     pft->stand->cell->output.daily.vscal=pft->vscal;
   }
   return n_uptake;
} /* of 'nuptake_crop' */

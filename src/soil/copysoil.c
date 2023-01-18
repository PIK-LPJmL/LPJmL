/**************************************************************************************/
/**                                                                                \n**/
/**                c  o  p  y  s  o  i  l  .  c                                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function copies soil variables                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void copysoil(Soil *dst,       /**< destination */
              const Soil *src, /**< source */
              int ntotpft      /**< total number of PFTs */
              )                /** \return void */
{
  int i,l,p;
  dst->par=src->par;
  forrootsoillayer(l)
  {
    dst->pool[l]=src->pool[l];
    dst->k_mean[l].slow=src->k_mean[l].slow;
    dst->k_mean[l].fast=src->k_mean[l].fast;
    dst->NO3[l]=src->NO3[l];
    dst->NH4[l]=src->NH4[l];
    dst->CH4[l] = src->CH4[l];
    dst->O2[l] = src->O2[l];
    dst->layer_exists[l] = src->layer_exists[l];
    for(p=0;p<ntotpft;p++)
      dst->c_shift[l][p]=src->c_shift[l][p];
  }
  dst->YEDOMA=src->YEDOMA;
#ifdef MICRO_HEATING
  dst->litter.decomC=src->litter.decomC;
#endif
  dst->alag=src->alag;
  dst->amp=src->amp;
  dst->meanw1=src->meanw1;
  dst->snowpack=src->snowpack;
  dst->snowdens = src->snowdens;
  dst->snowheight = src->snowheight;
  dst->snowfraction = src->snowfraction;
  dst->icefrac=src->icefrac;
  dst->fastfrac=src->fastfrac;
  dst->maxthaw_depth=src->maxthaw_depth;
  dst->mean_maxthaw=src->mean_maxthaw;
  dst->decomp_litter_mean=src->decomp_litter_mean;
  dst->whcs_all=src->whcs_all;
  dst->litter.agtop_wcap=src->litter.agtop_wcap;
  dst->litter.agtop_moist=src->litter.agtop_moist;
  dst->litter.agtop_cover=src->litter.agtop_cover;
  dst->litter.agtop_temp=src->litter.agtop_temp;
  dst->count=src->count;
  freelitter(&dst->litter);
  dst->litter.n=src->litter.n;
  if(src->litter.n>0)
  {
    dst->litter.item=newvec(Litteritem,src->litter.n);
    check(dst->litter.item);
    for(i=0;i<src->litter.n;i++)
      dst->litter.item[i]=src->litter.item[i];
  }
  for(i=0;i<NFUELCLASS+1;i++)
    dst->litter.avg_fbd[i]=src->litter.avg_fbd[i];
  for(i=0;i<NSOILLAYER;i++)
  {
    dst->w[i]=src->w[i];
    dst->w_fw[i]=src->w_fw[i];
    dst->ice_fw[i]=src->ice_fw[i];
    dst->ice_depth[i]=src->ice_depth[i];
    dst->freeze_depth[i]=src->freeze_depth[i];
    dst->ice_pwp[i]=src->ice_pwp[i];
    dst->state[i]=src->state[i];
    dst->perc_energy[i]=src->perc_energy[i];
    dst->wpwp[i]=src->wpwp[i];
    dst->wfc[i]=src->wfc[i];
    dst->wsat[i]=src->wsat[i];
    dst->whc[i]=src->whc[i];
    dst->wsats[i]=src->wsats[i];
    dst->whcs[i]=src->whcs[i];
    dst->wpwps[i]=src->wpwps[i];
    dst->k_dry[i]=src->k_dry[i];
    dst->Ks[i]=src->Ks[i];
    dst->bulkdens[i]=src->bulkdens[i];
    dst->beta_soil[i]=src->beta_soil[i];
#ifdef MICRO_HEATING
    dst->micro_heating[i]=src->micro_heating[i];
    dst->decomC[i]=src->decomC[i];
#endif
  }
  dst->w_evap=src->w_evap;
  for(i=0;i<NSOILLAYER+1;i++)
  {
    dst->temp[i]=src->temp[i];
    dst->amean_temp[i] = src->amean_temp[i];
  }
  dst->rw_buffer=src->rw_buffer;
  dst->wa = src->wa;
  dst->wtable = src->wtable;
  dst->iswetland = src->iswetland;
  for(i=0;i<NTILLLAYER;i++)
    dst->df_tillage[i]=src->df_tillage[i];
} /* of 'copysoil' */

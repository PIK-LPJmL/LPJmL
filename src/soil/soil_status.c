/**************************************************************************************/
/**                                                                                \n**/
/**                      s o i l _ s t a t u s .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**           save and copy soil status variables changed in littersom             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void soil_status(Soil *dst,       /**< destination */
              const Soil *src, /**< source */
              int ntotpft      /**< total number of PFTs */
              )                /** \return void */
{
  int i,l,p;
  dst->par=src->par;
  forrootsoillayer(l)
  {
    dst->pool[l]=src->pool[l];
    dst->NO3[l]=src->NO3[l];
    dst->NH4[l]=src->NH4[l];
    dst->CH4[l] = src->CH4[l];
    dst->O2[l] = src->O2[l];
    dst->decay_rate[l].slow=src->decay_rate[l].slow;
    dst->decay_rate[l].fast=src->decay_rate[l].fast;
  }
  for(i=0;i<NSOILLAYER;i++)
  {
    dst->w[i]=src->w[i];
    dst->w_fw[i]=src->w_fw[i];
    dst->ice_fw[i]=src->ice_fw[i];
    dst->ice_depth[i]=src->ice_depth[i];
  }
  for(p=0;p<ntotpft;p++)
  {
    dst->decomp_litter_pft[p].carbon=src->decomp_litter_pft[p].carbon;
    dst->decomp_litter_pft[p].nitrogen=src->decomp_litter_pft[p].nitrogen;
  }
  dst->fastfrac=src->fastfrac;
  dst->YEDOMA=src->YEDOMA;
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
}

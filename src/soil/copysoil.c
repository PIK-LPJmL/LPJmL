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
    dst->cpool[l].fast=src->cpool[l].fast;
    dst->cpool[l].slow=src->cpool[l].slow;
    dst->k_mean[l].slow=src->k_mean[l].slow;
    dst->k_mean[l].fast=src->k_mean[l].fast;
    for(p=0;p<ntotpft;p++)
    {
      dst->c_shift_fast[l][p]=src->c_shift_fast[l][p];
      dst->c_shift_slow[l][p]=src->c_shift_slow[l][p];
    }
  }
  dst->YEDOMA=src->YEDOMA;
#ifdef MICRO_HEATING
  dst->litter.decomC=src->litter.decomC;
#endif
  dst->alag=src->alag;
  dst->amp=src->amp;
  dst->meanw1=src->meanw1;
  dst->snowpack=src->snowpack;
  dst->maxthaw_depth=src->maxthaw_depth;
  dst->mean_maxthaw=src->mean_maxthaw;
  dst->decomp_litter_mean=src->decomp_litter_mean;
  freelitter(&dst->litter);
  dst->litter.n=src->litter.n;
  if(src->litter.n>0)
  {
    dst->litter.ag=newvec(Litteritem,src->litter.n);
    check(dst->litter.ag);
    dst->litter.bg=newvec(Real,src->litter.n);
    check(dst->litter.bg);
    for(i=0;i<src->litter.n;i++)
    {
      dst->litter.ag[i]=src->litter.ag[i];
      dst->litter.bg[i]=src->litter.bg[i];
    }
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
#ifdef MICRO_HEATING
    dst->micro_heating[i]=src->micro_heating[i];
    dst->decomC[i]=src->decomC[i];
#endif
  }
  dst->w_evap=src->w_evap;
  for(i=0;i<NSOILLAYER+1;i++)
    dst->temp[i]=src->temp[i];
  dst->rw_buffer=src->rw_buffer;
} /* of 'copysoil' */

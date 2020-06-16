/**************************************************************************************/
/**                                                                                \n**/
/**                     e  q  u  i  l  s  o  i  l  . c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     SOIL DECOMPOSITION EQUILIBRIUM CALCULATION                                 \n**/
/**                                                                                \n**/
/**     Analytical solution of differential flux equations for fast and            \n**/
/**     slow soil carbon pools.  Implemented after (soil_equil_year)               \n**/
/**     simulation years, when annual litter inputs should be close to             \n**/
/**     equilibrium.  Assumes average climate (temperature and soil                \n**/
/**     moisture) from all years up to soil_equil_year.                            \n**/
/**                                                                                \n**/
/**       Rate of change of soil pool size = litter input - decomposition          \n**/
/**         (5) dc/dt = litter_decom - kc                                          \n**/
/**       At equilibrium,                                                          \n**/
/**         (6) dc/dt = 0                                                          \n**/
/**       From (5) & (6),                                                          \n**/
/**         (7) c = litter_decom / k                                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void equilsoil(Soil *soil,            /**< pointer to soil data */
               int ntotpft,           /**< total number of PFTs */
               const Pftpar pftpar[]  /**< PFT parameter array */
              )                       /** \return void */
{
  int l,p,f;
  Real sumlitter,pftlitter,wood=0,socfraction;
  Pool *k_mean,*c0;
  Pool *sum;

  k_mean=newvec(Pool,ntotpft);
  check(k_mean);
  c0=newvec(Pool,ntotpft);
  check(c0);
  sum=newvec(Pool,ntotpft);
  check(sum);
  
  sumlitter=littersum(&soil->litter);
  soil->decomp_litter_mean/=(soil_equil_year-veg_equil_year);
  forrootsoillayer(l)
  {
    soil->k_mean[l].fast/=(soil_equil_year-veg_equil_year);
    soil->k_mean[l].slow/=(soil_equil_year-veg_equil_year);
  }
  for(p=0;p<ntotpft;p++)
    k_mean[p].fast=k_mean[p].slow=sum[p].fast=sum[p].slow=0.0;
  forrootsoillayer(l)
    for(p=0;p<ntotpft;p++)
    {
      socfraction=pow(10,pftpar[p].soc_k*logmidlayer[l])
                  - (l>0 ? pow(10,pftpar[p].soc_k*logmidlayer[l-1]): 0);
      k_mean[p].fast+=soil->k_mean[l].fast*socfraction;
      k_mean[p].slow+=soil->k_mean[l].slow*socfraction;
    }
  for(p=0;p<ntotpft;p++)
  {
    c0[p].fast=k_mean[p].fast>epsilon ? (1-param.atmfrac)*param.fastfrac*(soil->decomp_litter_mean)/k_mean[p].fast : 0;
    c0[p].slow=k_mean[p].slow>epsilon ? (1-param.atmfrac)*(1.0-param.fastfrac)*(soil->decomp_litter_mean)/k_mean[p].slow : 0;
  }

  if(soil->decomp_litter_mean>epsilon)
  {
    forrootsoillayer(l)
      soil->cpool[l].slow=soil->cpool[l].fast=0;
    forrootsoillayer(l)
    {
      for(p=0;p<soil->litter.n;p++)
      {
        wood=0;
        for(f=0;f<NFUELCLASS;f++)
          wood+=soil->litter.ag[p].trait.wood[f];
        pftlitter=soil->litter.bg[p]+soil->litter.ag[p].trait.leaf+wood;
        socfraction=pow(10,soil->litter.ag[p].pft->soc_k*logmidlayer[l])
                    - (l>0 ? pow(10,soil->litter.ag[p].pft->soc_k*logmidlayer[l-1]): 0);
        soil->cpool[l].slow+=c0[soil->litter.ag[p].pft->id].slow*socfraction*pftlitter/sumlitter;
        soil->cpool[l].fast+=c0[soil->litter.ag[p].pft->id].fast*socfraction*pftlitter/sumlitter;
      }
    }
    forrootsoillayer(l)
    {
      for(p=0;p<ntotpft;p++)
      {
        socfraction=pow(10,pftpar[p].soc_k*logmidlayer[l])
                    - (l>0 ? pow(10,pftpar[p].soc_k*logmidlayer[l-1]): 0);
        soil->c_shift_fast[l][p]=(c0[p].fast*socfraction-c0[p].fast*socfraction*(1-soil->k_mean[l].fast))/((1-param.atmfrac)*param.fastfrac*(soil->decomp_litter_mean));
        soil->c_shift_slow[l][p]=(c0[p].slow*socfraction-c0[p].slow*socfraction*(1-soil->k_mean[l].slow))/((1-param.atmfrac)*(1.0-param.fastfrac)*(soil->decomp_litter_mean));
        sum[p].fast+=soil->c_shift_fast[l][p];
        sum[p].slow+=soil->c_shift_slow[l][p];
      }

      soil->k_mean[l].slow=soil->k_mean[l].fast=0.0;
    }
    for(p=0;p<ntotpft;p++)
    {
      if(sum[p].fast<1.0 && sum[p].fast>0.0)
        for (l=0;l<LASTLAYER;l++)
          soil->c_shift_fast[l][p]=soil->c_shift_fast[l][p]/sum[p].fast;
      else if (sum[p].fast<epsilon)
      {
        soil->c_shift_fast[0][p]=1.0;
        for (l=1;l<LASTLAYER;l++)
          soil->c_shift_fast[l][p]=0;
      }
      if(sum[p].slow<1.0 && sum[p].slow>0.0)
        for (l=0;l<LASTLAYER;l++)
          soil->c_shift_slow[l][p]=soil->c_shift_slow[l][p]/sum[p].slow;
      else if (sum[p].slow<epsilon)
      {
        soil->c_shift_slow[0][p]=1.0;
        for (l=1;l<LASTLAYER;l++)
          soil->c_shift_slow[l][p]=0;
      }
    }
    soil->count=0;
    soil->decomp_litter_mean=0.0;
  }
  /* freeing memory */
  
  free(k_mean);
  free(c0);
  free(sum);
 
} /* of 'equilsoil' */

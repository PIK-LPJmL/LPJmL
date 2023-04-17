/**************************************************************************************/
/**                                                                                \n**/
/**                     e  q  u  i  l  s  o  i  l  . c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     SOIL DECOMPOSITION EQUILIBRIUM CALCULATION                                 \n**/
/**                                                                                \n**/
/**     Analytical solution of differential flux equations for fast and            \n**/
/**     slow soil carbon pools.  Needs be to called several times to reach         \n**/
/**     equilibrium.                                                               \n**/
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
               const Pftpar pftpar[], /**< PFT parameter array */
               Bool nremove           /**< remove mineral N */
              )                       /** \return void         */
{
  int l,p;
  Real socfraction;
  Poolpar *sum;
  sum=newvec(Poolpar,ntotpft);
  check(sum);
  
  for(p=0;p<ntotpft;p++)
    sum[p].fast=sum[p].slow=0.0;

  /* calculate c_shift */
  forrootsoillayer(l)
  {
    soil->decay_rate[l].fast/=soil->count;
    soil->decay_rate[l].slow/=soil->count;
    for(p=0;p<ntotpft;p++)
    {
      socfraction=pow(10,pftpar[p].soc_k*logmidlayer[l])
                  - (l>0 ? pow(10,pftpar[p].soc_k*logmidlayer[l-1]): 0);
      soil->c_shift[l][p].fast=soil->decay_rate[l].fast*socfraction;
      soil->c_shift[l][p].slow=soil->decay_rate[l].slow*socfraction;
      sum[p].fast+=soil->c_shift[l][p].fast;
      sum[p].slow+=soil->c_shift[l][p].slow;
    }
  }
  for(p=0;p<ntotpft;p++)
  {
    if(sum[p].fast>0)
      for (l=0;l<LASTLAYER;l++) 
        soil->c_shift[l][p].fast/=sum[p].fast;
    else
    {
      soil->c_shift[0][p].fast=1.0;
      for (l=1;l<LASTLAYER;l++) 
        soil->c_shift[l][p].fast=0;
    }
    if(sum[p].slow>0)
      for (l=0;l<LASTLAYER;l++) 
        soil->c_shift[l][p].slow/=sum[p].slow;
    else
    {
      soil->c_shift[0][p].slow=1.0;
      for (l=1;l<LASTLAYER;l++) 
        soil->c_shift[l][p].slow=0;
    }
  }

  /* resest soil C and N pools*/
  forrootsoillayer(l)
  {
    soil->pool[l].slow.carbon=soil->pool[l].fast.carbon=0;
    soil->pool[l].slow.nitrogen=soil->pool[l].fast.nitrogen=0;
  }
     
  /* caluclate equilibrium C and N pools for given C and N from litter and decay rates */
  for(p=0;p<ntotpft;p++)
  {
    soil->decomp_litter_pft[p].carbon/=soil->count;
    soil->decomp_litter_pft[p].nitrogen/=soil->count;
  }
  forrootsoillayer(l)
  {
    if(soil->decay_rate[l].fast>0)
    {
      for(p=0;p<ntotpft;p++)
      {
        soil->pool[l].fast.carbon+=param.fastfrac*soil->decomp_litter_pft[p].carbon*soil->c_shift[l][p].fast/soil->decay_rate[l].fast;
        soil->pool[l].fast.nitrogen+=param.fastfrac*soil->decomp_litter_pft[p].nitrogen*soil->c_shift[l][p].fast/soil->decay_rate[l].fast;
      }
    }
    if(soil->decay_rate[l].slow>0)
    {
      for(p=0;p<ntotpft;p++)
      {
        soil->pool[l].slow.carbon+=(1-param.fastfrac)*soil->decomp_litter_pft[p].carbon*soil->c_shift[l][p].slow/soil->decay_rate[l].slow;
        soil->pool[l].slow.nitrogen+=(1-param.fastfrac)*soil->decomp_litter_pft[p].nitrogen*soil->c_shift[l][p].slow/soil->decay_rate[l].slow;
      }
    }
  }

  if(nremove)
    forrootsoillayer(l)
      soil->NH4[l]=soil->NO3[l]=0.0;

  free(sum);
}

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

void equilsoil(Soil *soil,           /**< pointer to soil data */
               int ntotpft,          /**< total number of PFTs */
               const Pftpar pftpar[], /**< PFT parameter array */
               Bool shift,
               Bool iswetland        /**< stand is wetland (TRUE/FALSE) */
              )                      /** \return void         */
{
  int l,p,f;
  Real sumlitter,pftlitter,wood=0,socfraction,fastfrac;
  Real epsilon_gas, soilmoist, V;
  Poolpar *k_mean, *c0, *sum,*k_mean_layer;
  Poolpar nc_ratio[LASTLAYER];
  Bool *present;
  Pool totalsum={{0,0},{0,0}};
  present=newvec(Bool,ntotpft);
  for (p=0;p<ntotpft;p++)
    present[p]=FALSE;
  k_mean_layer=newvec(Poolpar,LASTLAYER);
  check(k_mean_layer);
  k_mean=newvec(Poolpar,ntotpft);
  check(k_mean);
  c0=newvec(Poolpar,ntotpft);
  check(c0);
  sum=newvec(Poolpar,ntotpft);
  check(sum);
  sumlitter=littercarbon(&soil->litter);
  
  for(p=0;p<ntotpft;p++)
    k_mean[p].fast=k_mean[p].slow=sum[p].fast=sum[p].slow=c0[p].fast=c0[p].slow=0.0;
  forrootsoillayer(l)
  {
    V=getV(soil,l);  /*soil air content (m3 air/m3 soil)*/
    soilmoist=getsoilmoist(soil,l);
    epsilon_gas=max(0.00004, V+soilmoist*soil->wsat[l]*BO2);
    soil->O2[l]=p_s/R_gas/(10+273.15)*O2s*WO2*soildepth[l]*epsilon_gas/1000; /*266 g/m3 converted to g/m2 per layer*/
    epsilon_gas=max(0.00004, V+soilmoist*soil->wsat[l]*BCH4);
    soil->CH4[l]=p_s/R_gas/(10+273.15)*param.pch4*1e-9*WCH4*soildepth[l]*epsilon_gas/1000;    /* corresponding to atmospheric CH4 concentration to g/m2 per layer*/
    totalsum.fast.carbon+=soil->pool[l].fast.carbon;
    totalsum.slow.carbon+=soil->pool[l].slow.carbon;
    totalsum.fast.nitrogen+=soil->pool[l].fast.nitrogen;
    totalsum.slow.nitrogen+=soil->pool[l].slow.nitrogen;
  }

  if(!shift)
  {
    soil->decomp_litter_mean.carbon/=(soil_equil_year-param.veg_equil_year);
    soil->decomp_litter_mean.nitrogen/=(soil_equil_year-param.veg_equil_year);
  } 
  else //if(shift==TRUE)
  {
    for(p=0;p<ntotpft;p++)
      k_mean[p].fast=k_mean[p].slow=sum[p].fast=sum[p].slow=c0[p].fast=c0[p].slow=0.0;
    forrootsoillayer(l)
    {
      k_mean_layer[l].fast=soil->k_mean[l].fast/(cshift_year+1);
      k_mean_layer[l].slow=soil->k_mean[l].slow/(cshift_year+1);
      for(p=0;p<ntotpft;p++)
      {
        if (iswetland && present[p])
          socfraction=pow(10,pftpar[p].soc_k*0.9*logmidlayer[l])
                        -(l>0 ? pow(10,pftpar[p].soc_k*0.9*logmidlayer[l-1]): 0);
        else
          socfraction=pow(10,pftpar[p].soc_k*logmidlayer[l])
                        -(l>0 ? pow(10,pftpar[p].soc_k*logmidlayer[l-1]): 0);
        k_mean[p].fast+=k_mean_layer[l].fast*socfraction;
        k_mean[p].slow+=k_mean_layer[l].slow*socfraction;
      }
    }
    forrootsoillayer(l)
    {
      for(p=0;p<ntotpft;p++)
      {
        if (iswetland && present[p])
          socfraction=pow(10,pftpar[p].soc_k*0.9*logmidlayer[l])
                        -(l>0 ? pow(10,pftpar[p].soc_k*0.9*logmidlayer[l-1]): 0);
        else
          socfraction=pow(10,pftpar[p].soc_k*logmidlayer[l])
                       - (l>0 ? pow(10,pftpar[p].soc_k*logmidlayer[l-1]): 0);
        if(k_mean[p].fast==0)
          soil->c_shift[l][p].fast=0;
        else
          soil->c_shift[l][p].fast=socfraction*k_mean_layer[l].fast/k_mean[p].fast;
        if(k_mean[p].slow==0)
          soil->c_shift[l][p].slow=0;
        else
          soil->c_shift[l][p].slow=socfraction*k_mean_layer[l].slow/k_mean[p].slow;
        sum[p].fast+=soil->c_shift[l][p].fast;
        sum[p].slow+=soil->c_shift[l][p].slow;
      }
      soil->pool[l].slow.carbon=totalsum.slow.carbon*socfraction;
      soil->pool[l].fast.carbon=totalsum.fast.carbon*socfraction;
      soil->pool[l].slow.nitrogen=totalsum.slow.nitrogen*socfraction;
      soil->pool[l].fast.nitrogen=totalsum.fast.nitrogen*socfraction;
      soil->k_mean[l].slow=soil->k_mean[l].fast=0.0;
    }
    for(p=0;p<ntotpft;p++)
    {
      if(sum[p].fast<1.0 && sum[p].fast>0.0)
      //if(sum[p].fast>=epsilon)
        for (l=0;l<LASTLAYER;l++) 
          soil->c_shift[l][p].fast/=sum[p].fast;
      else if (sum[p].fast<epsilon)
      {
        soil->c_shift[0][p].fast=1.0;
        for (l=1;l<LASTLAYER;l++) 
          soil->c_shift[l][p].fast=0;
      }
      if(sum[p].slow<1.0 && sum[p].slow>0.0)
      //if(sum[p].slow>=epsilon)
        for (l=0;l<LASTLAYER;l++) 
          soil->c_shift[l][p].slow/=sum[p].slow;
      else if (sum[p].slow<epsilon)
      {
        soil->c_shift[0][p].slow=1.0;
        for (l=1;l<LASTLAYER;l++) 
          soil->c_shift[l][p].slow=0;
      }
    }
    if(soil->decomp_litter_mean.carbon<epsilon)
    { /* if there is no carbon, remove mineral N from initialization */
      forrootsoillayer(l)
      {
        soil->NH4[l]=soil->NO3[l]=0.0;
        soil->pool[l].slow.nitrogen=0;
        soil->pool[l].fast.nitrogen=0;
        soil->pool[l].slow.carbon=0;
        soil->pool[l].fast.carbon=0;
        soil->k_mean[l].slow=soil->k_mean[l].fast=0.0;
        soil->count=0;
      }
    }
    soil->count=0;
    soil->decomp_litter_mean.carbon=soil->decomp_litter_mean.nitrogen=0.0;
  }//if(shift==TRUE)

  if(!shift)
  {
    if(soil->decomp_litter_mean.carbon>epsilon)
    {
      forrootsoillayer(l)
      {
        k_mean_layer[l].fast=soil->k_mean[l].fast/(soil_equil_year-param.veg_equil_year);
        k_mean_layer[l].slow=soil->k_mean[l].slow/(soil_equil_year-param.veg_equil_year);
        for(p=0;p<ntotpft;p++)
        {
        if (iswetland && present[p])
          socfraction=pow(10,pftpar[p].soc_k*0.9*logmidlayer[l])
                        -(l>0 ? pow(10,pftpar[p].soc_k*0.9*logmidlayer[l-1]): 0);
        else
          socfraction=pow(10,pftpar[p].soc_k*logmidlayer[l])
                        -(l>0 ? pow(10,pftpar[p].soc_k*logmidlayer[l-1]): 0);

          fastfrac=soil->fastfrac;
          if(fastfrac>1 || fastfrac<0.5)
            fastfrac=param.fastfrac;
#ifdef LINEAR_DECAY
          c0[p].fast+=k_mean_layer[l].fast>epsilon ? (1-param.atmfrac)*fastfrac*soil->decomp_litter_mean.carbon/k_mean_layer[l].fast*soil->c_shift[l][p].fast : 0;
          c0[p].slow+=k_mean_layer[l].slow>epsilon ? (1-param.atmfrac)*(1.0-fastfrac)*soil->decomp_litter_mean.carbon/k_mean_layer[l].slow*soil->c_shift[l][p].slow : 0;
#else
          c0[p].fast+=k_mean_layer[l].fast>epsilon ? (1-param.atmfrac)*fastfrac*soil->decomp_litter_mean.carbon*soil->c_shift[l][p].fast/(1.0-exp(-k_mean_layer[l].fast)) : 0;
          c0[p].slow+=k_mean_layer[l].slow>epsilon ? (1-param.atmfrac)*(1.0-fastfrac)*soil->decomp_litter_mean.carbon*soil->c_shift[l][p].slow/(1.0-exp(-k_mean_layer[l].slow)) : 0;
#endif
        }
      }
      forrootsoillayer(l)
      {
        nc_ratio[l].slow=(soil->pool[l].slow.carbon==0) ? 0 : soil->pool[l].slow.nitrogen/soil->pool[l].slow.carbon;
        nc_ratio[l].fast=(soil->pool[l].fast.carbon==0) ? 0 : soil->pool[l].fast.nitrogen/soil->pool[l].fast.carbon;
        soil->pool[l].slow.carbon=soil->pool[l].fast.carbon=0;
        soil->pool[l].slow.nitrogen=soil->pool[l].fast.nitrogen=0;
        for(p=0;p<soil->litter.n;p++)
        {
          if (iswetland && present[p])
            socfraction=pow(10, pftpar[p].soc_k*0.9*logmidlayer[l])
          -   (l>0 ? pow(10, pftpar[p].soc_k*0.9*logmidlayer[l-1]) : 0);
          else
            socfraction=pow(10,pftpar[soil->litter.item[p].pft->id].soc_k*logmidlayer[l])
                    - (l>0 ? pow(10,pftpar[soil->litter.item[p].pft->id].soc_k*logmidlayer[l-1]): 0);
          wood=0;
          for(f=0;f<NFUELCLASS;f++)
            wood+=soil->litter.item[p].ag.wood[f].carbon+soil->litter.item[p].agsub.wood[f].carbon;
          pftlitter=soil->litter.item[p].bg.carbon+soil->litter.item[p].ag.leaf.carbon+soil->litter.item[p].agsub.leaf.carbon+wood;
          if(sumlitter>0)
             //soil->pool[l].slow.carbon+=c0[soil->litter.ag[p].pft->id].slow*soil->c_shift_slow[l][soil->litter.ag[p].pft->id]*pftlitter/sumlitter;
             soil->pool[l].slow.carbon+=c0[soil->litter.item[p].pft->id].slow*socfraction*pftlitter/sumlitter;
          if(sumlitter>0)
             //soil->pool[l].fast.carbon+=c0[soil->litter.ag[p].pft->id].fast*soil->c_shift_fast[l][soil->litter.ag[p].pft->id]*pftlitter/sumlitter;
             soil->pool[l].fast.carbon+=c0[soil->litter.item[p].pft->id].fast*socfraction*pftlitter/sumlitter;
        }
        soil->pool[l].slow.nitrogen=soil->pool[l].slow.carbon*nc_ratio[l].slow;
        soil->pool[l].fast.nitrogen=soil->pool[l].fast.carbon*nc_ratio[l].fast;
        soil->k_mean[l].slow=soil->k_mean[l].fast=0.0;
      }
      soil->count=0;
      soil->decomp_litter_mean.carbon=soil->decomp_litter_mean.nitrogen=0.0;
    }
    else //if(shift==TRUE)
    { /* if there is no carbon, remove mineral N from initialization */
      forrootsoillayer(l)
      {
        soil->NH4[l]=soil->NO3[l]=0.0;
        soil->pool[l].slow.nitrogen=0;
        soil->pool[l].fast.nitrogen=0;
        soil->k_mean[l].slow=soil->k_mean[l].fast=0.0;
        soil->count=0;
      }
      soil->decomp_litter_mean.carbon=soil->decomp_litter_mean.nitrogen=0.0;
    }
  }

  /* freeing memory */
  free(k_mean_layer);
  free(k_mean);
  free(c0);
  free(sum);
} /* of 'equilsoil' */

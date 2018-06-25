/**************************************************************************************/
/**                                                                                \n**/
/**                      l  i  t  t  e  r  s  o  m  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**               Vertical soil carbon distribution                                \n**/
/**                                                                                \n**/
/**               Litter and soil decomposition                                    \n**/
/**                                                                                \n**/
/**     Calculate daily litter decomposition using equation                        \n**/
/**       (1) dc/dt = -kc     where c=pool size, t=time,                           \n**/
/**           k=decomposition rate                                                 \n**/
/**     from (1),                                                                  \n**/
/**       (2) c = c0*exp(-kt) where c0=initial pool size                           \n**/
/**     from (2), decomposition in any month given by                              \n**/
/**       (3) delta_c = c0 - c0*exp(-k)                                            \n**/
/**     from (4)                                                                   \n**/
/**       (4) delta_c = c0*(1.0-exp(-k))                                           \n**/
/**     If LINEAR_DECAY is defined linear version of equations are used:           \n**/
/**       (3) delta_c = - c0*k                                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define MOIST_DENOM 0.63212055882855767841 /* (1.0-exp(-1.0)) */
#define cpool_low 1e-5
#define K10_YEDOMA 0.025/NDAYYEAR
#define INTERC 0.04021601
#define MOIST_3 -5.00505434
#define MOIST_2 4.26937932
#define MOIST  0.71890122

Real littersom(Soil *soil, /**< pointer to soil data */
               Real gtemp_soil[NSOILLAYER] /**< respiration coefficents */
              )                            /** \return decomposed carbon flux (gC/m2/day) */
{

  Real response[NSOILLAYER],response_wood;
  Real decay_litter;
  Pool cflux_soil[LASTLAYER];
  Real decom,decom_sum,soil_cflux,decom_litter;
  Real moist[NSOILLAYER];
  int i,p,l;
  Real yedoma_flux;

  foreachsoillayer(l) response[l]=0.0;
  decom_litter=soil_cflux=yedoma_flux=decom_sum=0.0;

  foreachsoillayer(l)
  {
    if(gtemp_soil[l]>0)
    {
      if (soil->par->wsats[l]-soil->ice_depth[l]-soil->ice_fw[l]-(soil->par->wpwps[l]*soil->ice_pwp[l])>epsilon)
           moist[l]=(soil->w[l]*soil->par->whcs[l]+(soil->par->wpwps[l]*(1-soil->ice_pwp[l]))+soil->w_fw[l])
                    /(soil->par->wsats[l]-soil->ice_depth[l]-soil->ice_fw[l]-(soil->par->wpwps[l]*soil->ice_pwp[l]));
      else
        moist[l]=epsilon;
      if (moist[l]<epsilon) moist[l]=epsilon;

      response[l]=gtemp_soil[l]*(INTERC+MOIST_3*(moist[l]*moist[l]*moist[l])+MOIST_2*(moist[l]*moist[l])+MOIST*moist[l]);
      if (response[l]<epsilon)
        response[l]=0.0;
      if (response[l]>1)
        response[l]=1.0;

      if(l<LASTLAYER)
      {
#ifdef LINEAR_DECAY
        cflux_soil[l].slow=soil->cpool[l].slow*param.k_soil10.slow*response[l];
        cflux_soil[l].fast=soil->cpool[l].fast*param.k_soil10.fast*response[l];
#else
        cflux_soil[l].slow=soil->cpool[l].slow*(1.0-exp(-(param.k_soil10.slow*response[l])));
        cflux_soil[l].fast=soil->cpool[l].fast*(1.0-exp(-(param.k_soil10.fast*response[l])));
#endif
        soil->cpool[l].slow-=cflux_soil[l].slow;
        soil->cpool[l].fast-=cflux_soil[l].fast;
        soil_cflux+=cflux_soil[l].slow+cflux_soil[l].fast;
        soil->k_mean[l].fast+=(param.k_soil10.fast*response[l]);
        soil->k_mean[l].slow+=(param.k_soil10.slow*response[l]);
#ifdef MICRO_HEATING
        soil->decomC[l]=cflux_soil[l].slow+cflux_soil[l].fast;
#endif
      }
      else
      {
        if (soil->YEDOMA>0.0 && response[l]>0.0)
        {
          yedoma_flux=soil->YEDOMA*(1.0-exp(-(K10_YEDOMA*response[l])));
          soil->YEDOMA-=yedoma_flux;
          soil_cflux+=yedoma_flux;
#ifdef MICRO_HEATING
          soil->decomC[l]+=yedoma_flux;
#endif
        }
      }
    }
  } /*end foreachsoillayer*/

  /*
  *       Calculate daily decomposition rates (k, /month) as a function of
  *       temperature and moisture
  *
  */

  if(gtemp_soil[0]>0)
   for(p=0;p<soil->litter.n;p++)
   {
     decom_sum=0;
#ifdef LINEAR_DECAY
     decay_litter=soil->litter.ag[p].pft->k_litter10.leaf*response[0];
#else
     decay_litter=1.0-exp(-(soil->litter.ag[p].pft->k_litter10.leaf*response[0]));
#endif
     decom=soil->litter.ag[p].trait.leaf*decay_litter;
     soil->litter.ag[p].trait.leaf-=decom;
     decom_sum+=decom;
     for(i=0;i<NFUELCLASS;i++)
     {
       response_wood=pow(soil->litter.ag[p].pft->k_litter10.q10_wood,(soil->temp[0]-10)/10.0)*(INTERC+MOIST_3*(moist[0]*moist[0]*moist[0])+MOIST_2*(moist[0]*moist[0])+MOIST*moist[0]);
#ifdef LINEAR_DECAY
       decay_litter=soil->litter.ag[p].pft->k_litter10.wood*response_wood;
#else
       decay_litter=1.0-exp(-(soil->litter.ag[p].pft->k_litter10.wood*response_wood));
#endif
       decom=soil->litter.ag[p].trait.wood[i]*decay_litter;
       soil->litter.ag[p].trait.wood[i]-=decom;
       decom_sum+=decom;
     }
#ifdef LINEAR_DECAY
     decay_litter=param.k_litter10*response[0];
#else
     decay_litter=1.0-exp(-(param.k_litter10*response[0]));
#endif
     decom=soil->litter.bg[p]*decay_litter;
     soil->litter.bg[p]-=decom;
     decom_sum+=decom;
     decom_litter+=decom_sum;
     forrootsoillayer(l)
     {
       soil->cpool[l].fast+=param.fastfrac*(1-param.atmfrac)*decom_sum*soil->c_shift_fast[l][soil->litter.ag[p].pft->id];
       soil->cpool[l].slow+=(1-param.fastfrac)*(1-param.atmfrac)*decom_sum*soil->c_shift_slow[l][soil->litter.ag[p].pft->id];
     }
   }   /*end soil->litter.n*/
  /*sum for equilsom-routine*/
  soil->decomp_litter_mean+=decom_litter;
#ifdef MICRO_HEATING
  soil->litter.decomC=decom_litter*param.atmfrac; /*only for mircobiological heating*/
#endif
  soil->count++;

  return decom_litter*param.atmfrac+soil_cflux;

} /* of 'littersom' */

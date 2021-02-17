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
#include "crop.h"
#include "agriculture.h"

#define MOIST_DENOM 0.63212055882855767841 /* (1.0-exp(-1.0)) */
#define K10_YEDOMA 0.025/NDAYYEAR
#define INTERCEPT 0.04021601
#define MOIST_3 -5.00505434
#define MOIST_2 4.26937932
#define MOIST  0.71890122
#define CN_ratio_fast 8
#define CN_ratio_slow 12
#define k_N 5e-3 /* Michaelis-Menten parameter k_S,1/2 (gN/m3) */
#define k_l 0.0  /* Parton et al., 2001 equ. 2 */

static Real f_wfps(const Soil *soil,      /* Soil data */
                   int l                  /* soil layer */
                  )                       /* return soil temperature (deg C) */
{
  Real x;
  x=(soil->w[l]*soil->whcs[l]+soil->wpwps[l]+soil->w_fw[l]+soil->ice_fw[l])/soil->wsats[l];
  return pow((x-soil->par->b_nit)/soil->par->n_nit,soil->par->z_nit)*
    pow((x-soil->par->c_nit)/soil->par->m_nit,soil->par->d_nit);
} /* of 'f_wfps' */

static Real f_ph(Real ph)
{
  return 0.56 + atan(M_PI*0.45*(-5 + ph)) / M_PI;
} /* of 'f_ph' */

Stocks littersom(Stand *stand,                /**< pointer to stand data */
                 const Real gtemp_soil[NSOILLAYER], /**< respiration coefficents */
                 Real cellfrac_agr,           /**< stand fraction of agricultural cells (0..1) */
                 int npft,                    /**< number of natural PFTs */
                 int ncft,                    /**< number of crop PFTs */
                 const Config *config         /**< nitrogen enabled? */
                ) /** \return decomposed carbon/nitrogen (g/m2) */
{

  Real response[NSOILLAYER];
  Real response_agtop_leaves,response_agtop_wood,response_agsub_leaves,response_agsub_wood,response_bg_litter,w_agtop;
  Real decay_litter;
  Pool flux_soil[LASTLAYER];
  Real decom,soil_cflux;
  Stocks decom_litter;
  Stocks decom_sum,flux;
  Real moist[NSOILLAYER];
  Real N_sum;
  Real n_immo;
  int i,p,l;
  Soil *soil;
  Real yedoma_flux;
  Real cshift_fast_sum[NSOILLAYER];
  Real cshift_slow_sum[NSOILLAYER];
  Real F_NO3=0;                /* soil nitrification rate gN *m-2*d-1*/
  Real F_N2O=0;                /* soil nitrification rate gN *m-2*d-1*/
  Real F_Nmineral;  /* net mineralization flux gN *m-2*d-1*/
  Real fac_wfps, fac_temp;
  String line;
  Pft *pft;
  Pftcrop *crop;
  Irrigation *data;
  soil=&stand->soil;
  flux.nitrogen=0;
  foreachsoillayer(l)
     response[l]=cshift_fast_sum[l]=cshift_slow_sum[l]=0.0;
  decom_litter.carbon=decom_litter.nitrogen=soil_cflux=yedoma_flux=decom_sum.carbon=decom_sum.nitrogen=0.0;

  foreachsoillayer(l)
  {
    if(gtemp_soil[l]>0)
    {
      if (soil->wsats[l]-soil->ice_depth[l]-soil->ice_fw[l]-(soil->wpwps[l]*soil->ice_pwp[l])>epsilon)
        moist[l]=(soil->w[l]*soil->whcs[l]+(soil->wpwps[l]*(1-soil->ice_pwp[l]))+soil->w_fw[l])
                 /(soil->wsats[l]-soil->ice_depth[l]-soil->ice_fw[l]-(soil->wpwps[l]*soil->ice_pwp[l]));
      else
        moist[l]=epsilon;
      if (moist[l]<epsilon) moist[l]=epsilon;

      response[l]=gtemp_soil[l]*(INTERCEPT+MOIST_3*(moist[l]*moist[l]*moist[l])+MOIST_2*(moist[l]*moist[l])+MOIST*moist[l]);
      if (response[l]<epsilon)
        response[l]=0.0;
      if (response[l]>1)
        response[l]=1.0;

      if(stand->type->landusetype==NATURAL)
        getoutputindex(&stand->cell->output,RESPONSE_LAYER_NV,l,config)+=response[l];
      if(isagriculture(stand->type->landusetype))
        getoutputindex(&stand->cell->output,RESPONSE_LAYER_AGR,l,config)+=response[l]*stand->frac/cellfrac_agr;

      if(l<LASTLAYER)
      {
#ifdef SAFE
        if(soil->NO3[l]<-epsilon)
          fail(NEGATIVE_SOIL_NO3_ERR,TRUE,"littersom: Negative soil NO3=%g in layer %d in cell (%s) before update",soil->NO3[l],l,sprintcoord(line,&stand->cell->coord));
        if(soil->NH4[l]<-epsilon)
          fail(NEGATIVE_SOIL_NH4_ERR,TRUE,"littersom: Negative soil NH4=%g in layer %d in cell (%s) before update",soil->NH4[l],l,sprintcoord(line,&stand->cell->coord));
#endif
#ifdef LINEAR_DECAY
        flux_soil[l].slow.carbon=max(0,soil->pool[l].slow.carbon*param.k_soil10.slow*response[l]);
        flux_soil[l].fast.carbon=max(0,soil->pool[l].fast.carbon*param.k_soil10.fast*response[l]);
        flux_soil[l].slow.nitrogen=max(0,soil->pool[l].slow.nitrogen*param.k_soil10.slow*response[l]);
        flux_soil[l].fast.nitrogen=max(0,soil->pool[l].fast.nitrogen*param.k_soil10.fast*response[l]);
#else
        flux_soil[l].slow.carbon=max(0,soil->pool[l].slow.carbon*(1.0-exp(-(param.k_soil10.slow*response[l]))));
        flux_soil[l].fast.carbon=max(0,soil->pool[l].fast.carbon*(1.0-exp(-(param.k_soil10.fast*response[l]))));
        flux_soil[l].slow.nitrogen=max(0,soil->pool[l].slow.nitrogen*(1.0-exp(-(param.k_soil10.slow*response[l]))));
        flux_soil[l].fast.nitrogen=max(0,soil->pool[l].fast.nitrogen*(1.0-exp(-(param.k_soil10.fast*response[l]))));
#endif

/* TODO nitrogen limitation of decomposition including variable decay rates Brovkin,
      fn_som=flux_soil[l].fast.carbon/CN_ratio_fast+flux_soil[l].slow.carbon/CN_ratio_slow;

        if (fn_som>(flux_soil[l].fast.nitrogen+flux_soil[l].slow.nitrogen+soil->NH4[l]))
        {
          fnlim=max(0,(soil->NH4[l]+flux_soil[l].fast.nitrogen+flux_soil[l].slow.nitrogen)/fn_som);
        }*/

        soil->pool[l].slow.carbon-=flux_soil[l].slow.carbon;
        soil->pool[l].fast.carbon-=flux_soil[l].fast.carbon;
        soil->pool[l].slow.nitrogen-=flux_soil[l].slow.nitrogen;
        soil->pool[l].fast.nitrogen-=flux_soil[l].fast.nitrogen;
        soil_cflux+=flux_soil[l].slow.carbon+flux_soil[l].fast.carbon;
        F_Nmineral=flux_soil[l].slow.nitrogen+flux_soil[l].fast.nitrogen;
        soil->NH4[l]+=F_Nmineral*(1-k_l);
#ifdef SAFE
        if(soil->NH4[l]<-epsilon)
          fail(NEGATIVE_SOIL_NH4_ERR,TRUE,"Negative soil NH4=%g in layer %d in cell (%s) at mineralization",soil->NH4[l],l,sprintcoord(line,&stand->cell->coord));
#endif
        soil->NO3[l]+=F_Nmineral*k_l;
        getoutput(&stand->cell->output,N_MINERALIZATION,config)+=F_Nmineral*stand->frac;
        if(isagriculture(stand->type->landusetype))
          getoutput(&stand->cell->output,NMINERALIZATION_AGR,config)+=F_Nmineral*stand->frac;
        soil->k_mean[l].fast+=(param.k_soil10.fast*response[l]);
        soil->k_mean[l].slow+=(param.k_soil10.slow*response[l]);
#ifdef MICRO_HEATING
        soil->decomC[l]=flux_soil[l].slow.carbon+flux_soil[l].fast.carbon;
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
  } /* end foreachsoillayer */

  /*
   *   Calculate daily decomposition rates (k, /month) as a function of
   *   temperature and moisture
   *
   */

  if(gtemp_soil[0]>0)
  {
    for(p=0;p<soil->litter.n;p++)
    {
      response_agsub_leaves=response[0];
      response_agsub_wood=pow(soil->litter.item[p].pft->k_litter10.q10_wood,(soil->temp[0]-10)/10.0)*(INTERC+MOIST_3*(moist[0]*moist[0]*moist[0])+MOIST_2*(moist[0]*moist[0])+MOIST*moist[0]);
      w_agtop=soil->litter.agtop_wcap>epsilon ? soil->litter.agtop_moist/soil->litter.agtop_wcap : moist[0];
      response_agtop_leaves=temp_response(soil->litter.agtop_temp)*(INTERC+MOIST_3*(w_agtop*w_agtop*w_agtop)+MOIST_2*(w_agtop*w_agtop)+MOIST*w_agtop);
      response_agtop_wood=pow(soil->litter.item[p].pft->k_litter10.q10_wood,(soil->litter.agtop_temp-10)/10.0)*(INTERC+MOIST_3*(w_agtop*w_agtop*w_agtop)+MOIST_2*(w_agtop*w_agtop)+MOIST*w_agtop);
      response_bg_litter=response[0];

      decom_sum.carbon=decom_sum.nitrogen=0;
      /* agtop leaves */
#ifdef LINEAR_DECAY
      decay_litter=soil->litter.item[p].pft->k_litter10.leaf*response_agtop_leaves;
#else
      decay_litter=1.0-exp(-(soil->litter.item[p].pft->k_litter10.leaf*response_agtop_leaves));
#endif
      decom=soil->litter.item[p].ag.leaf.carbon*decay_litter;
      soil->litter.item[p].ag.leaf.carbon-=decom;
      decom_sum.carbon+=decom;
      decom=soil->litter.item[p].ag.leaf.nitrogen*decay_litter;
      soil->litter.item[p].ag.leaf.nitrogen-=decom;
      decom_sum.nitrogen+=decom;

      /* agtop wood */
#ifdef LINEAR_DECAY
      decay_litter=soil->litter.item[p].pft->k_litter10.wood*response_agtop_wood;
#else
      decay_litter=1.0-exp(-(soil->litter.item[p].pft->k_litter10.wood*response_agtop_wood));
#endif
      for(i=0;i<NFUELCLASS;i++)
      {
        decom=soil->litter.item[p].ag.wood[i].carbon*decay_litter;
        soil->litter.item[p].ag.wood[i].carbon-=decom;
        decom_sum.carbon+=decom;
        decom=soil->litter.item[p].ag.wood[i].nitrogen*decay_litter;
        soil->litter.item[p].ag.wood[i].nitrogen-=decom;
        decom_sum.nitrogen+=decom;
      }

      /* agsub leaves */
#ifdef LINEAR_DECAY
      decay_litter=soil->litter.item[p].pft->k_litter10.leaf*response_agsub_leaves;
#else
      decay_litter=1.0-exp(-(soil->litter.item[p].pft->k_litter10.leaf*response_agsub_leaves));
#endif
      decom=soil->litter.item[p].agsub.leaf.carbon*decay_litter;
      soil->litter.item[p].agsub.leaf.carbon-=decom;
      decom_sum.carbon+=decom;
      decom=soil->litter.item[p].agsub.leaf.nitrogen*decay_litter;
      soil->litter.item[p].agsub.leaf.nitrogen-=decom;
      decom_sum.nitrogen+=decom;

      /* agsub wood */
#ifdef LINEAR_DECAY
      decay_litter=soil->litter.item[p].pft->k_litter10.wood*response_agsub_wood;
#else
      decay_litter=1.0-exp(-(soil->litter.item[p].pft->k_litter10.wood*response_agsub_wood));
#endif
      for(i=0;i<NFUELCLASS;i++)
      {
        decom=soil->litter.item[p].agsub.wood[i].carbon*decay_litter;
        soil->litter.item[p].agsub.wood[i].carbon-=decom;
        decom_sum.carbon+=decom;
        decom=soil->litter.item[p].agsub.wood[i].nitrogen*decay_litter;
        soil->litter.item[p].agsub.wood[i].nitrogen-=decom;
        decom_sum.nitrogen+=decom;
      }

      /* bg litter */
#ifdef LINEAR_DECAY
      decay_litter=param.k_litter10*response_bg_litter;
#else
      decay_litter=1.0-exp(-(param.k_litter10*response_bg_litter));
#endif
      decom=soil->litter.item[p].bg.carbon*decay_litter;
      soil->litter.item[p].bg.carbon-=decom;
      decom_sum.carbon+=decom;
      decom=soil->litter.item[p].bg.nitrogen*decay_litter;
      soil->litter.item[p].bg.nitrogen-=decom;
      decom_sum.nitrogen+=decom;
      decom_litter.carbon+=decom_sum.carbon;
      decom_litter.nitrogen+=decom_sum.nitrogen;
      forrootsoillayer(l)
      {
        soil->pool[l].fast.carbon+=param.fastfrac*(1-param.atmfrac)*decom_sum.carbon*soil->c_shift[l][soil->litter.item[p].pft->id].fast;
        soil->pool[l].slow.carbon+=(1-param.fastfrac)*(1-param.atmfrac)*decom_sum.carbon*soil->c_shift[l][soil->litter.item[p].pft->id].slow;
        cshift_fast_sum[l]+=decom_sum.carbon*soil->c_shift[l][soil->litter.item[p].pft->id].fast;
        cshift_slow_sum[l]+=decom_sum.carbon*soil->c_shift[l][soil->litter.item[p].pft->id].slow;
        if(config->with_nitrogen)
        {
          if(decom_sum.nitrogen>0)
          {
            soil->pool[l].slow.nitrogen+=(1-param.fastfrac)*(1-param.atmfrac)*decom_sum.nitrogen*soil->c_shift[l][soil->litter.item[p].pft->id].slow;
            soil->pool[l].fast.nitrogen+=param.fastfrac*(1-param.atmfrac)*decom_sum.nitrogen*soil->c_shift[l][soil->litter.item[p].pft->id].fast;
            /* NO3 and N2O from mineralization of organic matter */
            F_Nmineral=decom_sum.nitrogen*param.atmfrac*(param.fastfrac*soil->c_shift[l][soil->litter.item[p].pft->id].fast+(1-param.fastfrac)*soil->c_shift[l][soil->litter.item[p].pft->id].slow);
            soil->NH4[l]+=F_Nmineral*(1-k_l);
#ifdef SAFE
            if(soil->NH4[l]<-epsilon)
              fail(NEGATIVE_SOIL_NH4_ERR,TRUE,"Negative soil NH4=%g in layer %d in cell (%s) at mineralization",soil->NH4[l],l,sprintcoord(line,&stand->cell->coord));
#endif
            soil->NO3[l]+=F_Nmineral*k_l;
            getoutput(&stand->cell->output,N_MINERALIZATION,config)+=F_Nmineral*stand->frac;
            if(isagriculture(stand->type->landusetype))
              getoutput(&stand->cell->output,NMINERALIZATION_AGR,config)+=F_Nmineral*stand->frac;
          }
          N_sum=soil->NH4[l]+soil->NO3[l];
          if(N_sum>0) /* immobilization of N */
          {
            n_immo=param.fastfrac*(1-param.atmfrac)*(decom_sum.carbon/soil->par->cn_ratio-decom_sum.nitrogen)*soil->c_shift[l][soil->litter.item[p].pft->id].fast*N_sum/soildepth[l]*1e3/(k_N+N_sum/soildepth[l]*1e3);
            if(n_immo >0)
            {
              if(n_immo>N_sum)
                n_immo=N_sum;
              soil->pool[l].fast.nitrogen+=n_immo;
              getoutput(&stand->cell->output,N_IMMO,config)+=n_immo*stand->frac;
              if(isagriculture(stand->type->landusetype))
                getoutput(&stand->cell->output,NIMMOBILIZATION_AGR,config)+=n_immo*stand->frac;
              soil->NH4[l]-=n_immo*soil->NH4[l]/N_sum;
              soil->NO3[l]-=n_immo*soil->NO3[l]/N_sum;
#ifdef SAFE
              if(soil->NO3[l]<-epsilon)
                fail(NEGATIVE_SOIL_NO3_ERR,TRUE,"Negative soil NO3=%g in layer %d in cell (%s) at immobilization in littersom()",soil->NO3[l],l,sprintcoord(line,&stand->cell->coord));
              if(soil->NH4[l]<-epsilon)
                fail(NEGATIVE_SOIL_NH4_ERR,TRUE,"Negative soil NH4=%g in layer %d in cell (%s) at immobilization in littersom()",soil->NH4[l],l,sprintcoord(line,&stand->cell->coord));
#endif
            }
          }
          N_sum=soil->NH4[l]+soil->NO3[l];
          if(N_sum>0)
          {
            n_immo=(1-param.fastfrac)*(1-param.atmfrac)*(decom_sum.carbon/soil->par->cn_ratio-decom_sum.nitrogen)*soil->c_shift[l][soil->litter.item[p].pft->id].slow*N_sum/soildepth[l]*1e3/(k_N+N_sum/soildepth[l]*1e3);
            if(n_immo >0)
            {
              if(n_immo>N_sum)
                n_immo=N_sum;
              soil->pool[l].slow.nitrogen+=n_immo;
              getoutput(&stand->cell->output,N_IMMO,config)+=n_immo*stand->frac;
              if(isagriculture(stand->type->landusetype))
                getoutput(&stand->cell->output,NIMMOBILIZATION_AGR,config)+=n_immo*stand->frac;
              soil->NH4[l]-=n_immo*soil->NH4[l]/N_sum;
              soil->NO3[l]-=n_immo*soil->NO3[l]/N_sum;
#ifdef SAFE
              if(soil->NO3[l]<-epsilon)
                fail(NEGATIVE_SOIL_NO3_ERR,TRUE,"Negative soil NO3=%g in layer %d in cell (%s) at immobilization in littersom()",soil->NO3[l],l,sprintcoord(line,&stand->cell->coord));
              if(soil->NH4[l]<-epsilon)
                fail(NEGATIVE_SOIL_NH4_ERR,TRUE,"Negative soil NH4=%g in layer %d in cell (%s) at immobilization in littersom()",soil->NH4[l],l,sprintcoord(line,&stand->cell->coord));
#endif
            }
          }
        } /* of if(config->with_nitrogen) */
      } /* of forrootlayer */
      /*sum for equilsom-routine*/
    }   /*end soil->litter.n*/

    if(decom_litter.carbon>0 && stand->type->landusetype==NATURAL)
    {
      forrootsoillayer(l)
      {
        getoutputindex(&stand->cell->output,CSHIFT_FAST_NV,l,config)+=cshift_fast_sum[l]/decom_litter.carbon;
        getoutputindex(&stand->cell->output,CSHIFT_SLOW_NV,l,config)+=cshift_slow_sum[l]/decom_litter.carbon;
      }
    }

    /*sum for equilsom-routine*/
    soil->decomp_litter_mean.carbon+=decom_litter.carbon;
    soil->decomp_litter_mean.nitrogen+=decom_litter.nitrogen;
  } /* end of gtemp_soil[0]>0 */

  /* NO3 and N2O from nitrification */
  if(config->with_nitrogen)
    forrootsoillayer(l)
    {
      fac_wfps = f_wfps(soil,l);
      fac_temp = f_temp(soil->temp[l]);
      F_NO3=param.k_max*soil->NH4[l]*fac_temp*fac_wfps*f_ph(stand->cell->soilph);
      if(F_NO3>soil->NH4[l])
        F_NO3=soil->NH4[l];
      F_N2O=param.k_2*F_NO3;
      soil->NO3[l]+=F_NO3*(1-param.k_2);
#ifdef SAFE
      if(soil->NO3[l]<-epsilon)
        fail(NEGATIVE_SOIL_NO3_ERR,TRUE,"littersom: Negative soil NO3=%g in layer %d in cell (%.2f %.2f)",soil->NO3[l],l,stand->cell->coord.lat,stand->cell->coord.lon);
#endif

      soil->NH4[l]-=F_NO3;
#ifdef SAFE
      if(soil->NH4[l]<-epsilon)
        fail(NEGATIVE_SOIL_NH4_ERR,TRUE,"Negative soil NH4=%g in layer %d in cell (%.2f,%.2f)",soil->NH4[l],l,stand->cell->coord.lat,stand->cell->coord.lon);
#endif
      flux.nitrogen += F_N2O;
      /* F_N2O is given back for output */
      if(stand->type->landusetype==AGRICULTURE)
      {
        foreachpft(pft,p,&stand->pftlist)
        {
          crop=pft->data;
          if(crop->dh!=NULL)
          {
            crop->dh->n2o_nitsum+=F_N2O;
            crop->dh->c_emissum+=decom_litter.carbon*param.atmfrac+soil_cflux;
          }
          else
          {
            data=stand->data;
            getoutputindex(&stand->cell->output,CFT_N2O_NIT,pft->par->id-npft+data->irrigation*ncft,config)+=F_N2O;
            getoutputindex(&stand->cell->output,CFT_C_EMIS,pft->par->id-npft+data->irrigation*ncft,config)+=decom_litter.carbon *param.atmfrac+soil_cflux;
          }
        }
      }
    }
#ifdef MICRO_HEATING
  soil->litter.decomC=decom_litter.carbon*param.atmfrac; /*only for mircobiological heating*/
#endif
  soil->count++;
  getoutput(&stand->cell->output,RH_LITTER,config)+=decom_litter.carbon*param.atmfrac*stand->frac;
  if(isagriculture(stand->type->landusetype))
    getoutput(&stand->cell->output,RH_AGR,config) += (decom_litter.carbon*param.atmfrac+soil_cflux)*stand->frac;
  flux.carbon=decom_litter.carbon*param.atmfrac+soil_cflux;
  return flux;
} /* of 'littersom' */

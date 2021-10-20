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
#define k_red 10                           /*anoxic decomposition is much smaller than oxic decomposition Khovorostyanov et al., 2008*/
#define k_red_litter 5
#define INTERCEPT 0.10021601               /* changed from 0.04021601*/
#define MOIST_3 -5.00505434
#define MOIST_2 4.26937932
#define MOIST  0.71890122
#define DECOM_SLOW  0.000005            /*very slow decomposition rate under ice cover*/
#define DECOM_FAST  0.00002            /*very slow decomposition rate under ice cover*/
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

#define CHECK_BALANCE

Stocks littersom(Stand *stand,                      /**< [inout] pointer to stand data */
                 const Real gtemp_soil[NSOILLAYER], /**< [in] respiration coefficents */
                 Real cellfrac_agr,                 /**< [in] stand fraction of agricultural cells (0..1) */
                 Real *methaneflux_litter,          /**< [out] CH4 emissions (gC/m2/day) */
                 Real airtemp,                      /**< [in] air temperature (deg C) */
                 Real pch4,                         /**< [in] atmoshoperic methane (ppm) */
                 Real *runoff,                      /**< [out] runoff (mm/day) */
                 Real *MT_water,                    /**< [out] water from oxidized methane (mm/day) */
                 int npft,                          /**< [in] number of natural PFTs */
                 int ncft,                          /**< [in] number of crop PFTs */
                 const Config *config               /**< [in] LPJmL configuration */
                )                                   /** \return decomposed carbon/nitrogen (g/m2) */
{
  Real response[NSOILLAYER];
  Real response_agtop_leaves,response_agtop_wood,response_agsub_leaves,response_agsub_wood,response_bg_litter,w_agtop;
  Real decay_litter, oxidation, litter_flux;
  Pool flux_soil[LASTLAYER];
  Real decom,soil_cflux;
  Stocks decom_litter;
  Stocks decom_sum,flux;
  Real moist[NSOILLAYER],soil_moist[NSOILLAYER];
  Real N_sum;
  Real n_immo;
  int i,p,l;
  Soil *soil;
  Real yedoma_flux;
  Real F_NO3=0;     /* soil nitrification rate gN *m-2*d-1*/
  Real F_N2O=0;     /* soil nitrification rate gN *m-2*d-1*/
  Real F_Nmineral;  /* net mineralization flux gN *m-2*d-1*/
  Real fac_wfps, fac_temp;
  String line;
  Pft *pft;
  Pftcrop *crop;
  Irrigation *data;
  /* FOR METHANE IMPLEMENTATION */
  Real C_max[LASTLAYER], V;
  Real methaneflux_soil;
  Real h2o_mt;   /* methane production */
  //Real CH4_air;
  Real O2_need;
  Real epsilon_O2 = 0;
  Real oxid_frac = 0.5;  // Assume that 1/2 of the O2 is utilized by other electron acceptors (Wania etal.,2010)
// IMPLEMENTATION OF THE EFFECTIVE CARBON CONCENTRATION
  Real C_eff[NSOILLAYER];
  Real C_tot[LASTLAYER];
  Real K_v[NSOILLAYER][2];
  Real K_vdiff[LASTLAYER][3];
  Real S=0.2587;        // saturation factor MacDougall and Knutti, 2016
  Real KOVCON=0.001*1000; //Constant of diffusion (m2a-1)
  Real dKOVCON;
  Pool soilold[LASTLAYER];
  Stocks soilall[LASTLAYER];
#ifdef CHECK_BALANCE
  Stocks start;
  Stocks end;
#endif
  soil=&stand->soil;
#ifdef CHECK_BALANCE
  start = soilstocks(soil);
  start.carbon+=soilmethane(soil);
#endif
  flux.nitrogen=0;
  foreachsoillayer(l)
    response[l]=0.0;
  decom_litter.carbon=decom_litter.nitrogen=soil_cflux=yedoma_flux=decom_sum.carbon=decom_sum.nitrogen=0.0;
  //CH4_air = p_s / R_gas / (airtemp + 273.15)*pch4*1e-6*WCH4;    /*g/m3 methane concentration*/

  K_vdiff[0][0]=2./(midlayer[1]*(midlayer[1]+(midlayer[2]-midlayer[1])));
  K_vdiff[0][1]=-2./(midlayer[1]*(midlayer[2]-midlayer[1]));
  K_vdiff[0][2]=2./((midlayer[2]-midlayer[1])*(midlayer[1]+(midlayer[2]-midlayer[1])));

  foreachsoillayer(l)
  {
    response[l] = epsilon + epsilon;
    if(l>0)
    {
      K_vdiff[l][0]=2./((midlayer[l]-midlayer[l-1])*((midlayer[l]-midlayer[l-1])+(midlayer[l+1]-midlayer[l])));
      K_vdiff[l][1]=-2./((midlayer[l]-midlayer[l-1])*((midlayer[l+1]-midlayer[l])));
      K_vdiff[l][2]=2./((midlayer[l+1]-midlayer[l])*((midlayer[l]-midlayer[l-1])+(midlayer[l+1]-midlayer[l])));
    }
  }

  decom_litter.carbon = soil_cflux = yedoma_flux = decom_sum.carbon = oxidation = litter_flux = 0.0;
  *methaneflux_litter=*runoff=*MT_water=0;
  foreachsoillayer(l)
    if(gtemp_soil[l]>0)
    {
      if (soil->wsats[l]-soil->ice_depth[l]-soil->ice_fw[l]-(soil->wpwps[l]*soil->ice_pwp[l])>epsilon)
        moist[l]=(soil->w[l]*soil->whcs[l]+(soil->wpwps[l]*(1-soil->ice_pwp[l]))+soil->w_fw[l])
                 /(soil->wsats[l]-soil->ice_depth[l]-soil->ice_fw[l]-(soil->wpwps[l]*soil->ice_pwp[l]));
      else
        moist[l]=epsilon;
      /* moist[l] must be in the inteval [0,1] */
      if (moist[l]<epsilon)
        moist[l]=epsilon;
      else if (moist[l]>1)
        moist[l]=1;
    }
  foreachsoillayer(l)
    soil_moist[l] = (soil->w[l] * soil->whcs[l] + soil->wpwps[l] + soil->w_fw[l]) / soil->wsats[l];


  K_v[0][0]=KOVCON*(K_vdiff[0][0]*C_eff[0]+K_vdiff[0][1]*C_eff[0]+K_vdiff[0][2]*C_eff[1]);
  if(K_v[0][0]>0)
	if(C_eff[1]>C_eff[0]) {
	  K_v[0][1]=-1;
	 } else {
	    K_v[0][1]=0;
    }

  for(l=1;l<LASTLAYER;l++)
  {
    if(layerbound[l]<=soil->maxthaw_depth){
	    dKOVCON=KOVCON;
    }else{
        dKOVCON=0.0;
    }
    K_v[l][0]=dKOVCON*(K_vdiff[l][0]*C_eff[l-1]+ K_vdiff[l][1]*C_eff[l]+K_vdiff[l][2]*C_eff[l+1]);
    if(K_v[l][0]>0) {
	  if(C_eff[l+1]>C_eff[l])
		K_v[l][1]=-1;
    } else if (C_eff[l-1]>C_eff[l]) {
    	K_v[l][1]=1;
	} else {
	  	K_v[l][1]=0;
    }
  }

  K_v[LASTLAYER-1][0]=dKOVCON*(K_vdiff[LASTLAYER-1][0]*C_eff[LASTLAYER-2]+ K_vdiff[LASTLAYER-1][1]*C_eff[LASTLAYER-1]+K_vdiff[LASTLAYER-1][2]*C_eff[LASTLAYER-1]);

  if(K_v[LASTLAYER-1][0]> 0.) {
	  K_v[LASTLAYER-1][0]=1;
  } else {
	  K_v[LASTLAYER-1][0]=0;
  }

  for(l=(LASTLAYER-1);l==0;l--)
  {
    if(K_v[l][1] == 1) {
  	    if((K_v[l][0]*(soildepth[l]/soildepth[l-1])) > (0.5*C_tot[l-1]))
  	    	K_v[l][0]=(0.5*C_tot[l-1])*(soildepth[l-1]/soildepth[l]);

			C_tot[l-1]-=(K_v[l][0]*(soildepth[l]/soildepth[l-1]));
  	} else if (K_v[l][1] == -1) {
  	    if((K_v[l][0]*(soildepth[l]/soildepth[l+1])) > (0.5*C_tot[l+1]))
  	    	K_v[l][0]=(0.5*C_tot[l+1])*(soildepth[l+1]/soildepth[l]);
    }
  }




  /* forrootsoillayer(l)
  if(soil->O2[l]>1000 ||soil->O2[l]< 0  ){

  //	  fprintf(stdout,"BL WA O2[%d]: %g\n",l,soil->O2[l]);
  //         abort();
  }*/

  foreachsoillayer(l)
  {
    if(gtemp_soil[l]>0)
    {
      if (moist[l]<epsilon)
        moist[l]=epsilon;

      V = getV(soil,l);  /*soil air content (m3 air/m3 soil)*/
      if (V<=epsilon)
         V=epsilon+epsilon;
      epsilon_O2=max(0.00004,V+soil_moist[l]*soil->wsat[l]*BO2);

      response[l]=gtemp_soil[l]*(INTERCEPT+MOIST_3*(moist[l]*moist[l]*moist[l])+MOIST_2*(moist[l]*moist[l])+MOIST*moist[l]);
      if (response[l]<epsilon)
        response[l]= epsilon + epsilon;
      if (response[l]>1)
        response[l]=1.0;

      if(l<LASTLAYER)
      {
        oxidation = h2o_mt = 0;
        C_max[l]=soil->O2[l]*V*WC/WO2;                                     //soil->O2[l] * oxid_frac*WC / WO2;       // C_max[l]=soil->O2[l]*V*WC/WO2;
        if(stand->type->landusetype==NATURAL)
          getoutputindex(&stand->cell->output,RESPONSE_LAYER_NV,l,config)+=response[l];
        if(isagriculture(stand->type->landusetype))
          getoutputindex(&stand->cell->output,RESPONSE_LAYER_AGR,l,config)+=response[l]*stand->frac/cellfrac_agr;
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
        if (flux_soil[l].fast.carbon>C_max[l])
        {
          flux_soil[l].fast.carbon=C_max[l];
          C_max[l]=0;
        }
        else
          C_max[l]-=flux_soil[l].fast.carbon;
        if (flux_soil[l].slow.carbon>C_max[l]) flux_soil[l].slow.carbon=C_max[l];

/* TODO nitrogen limitation of decomposition including variable decay rates Brovkin,
        fn_som=flux_soil[l].fast.carbon/CN_ratio_fast+flux_soil[l].slow.carbon/CN_ratio_slow;

        if (fn_som>(flux_soil[l].fast.nitrogen+flux_soil[l].slow.nitrogen))
        {
          fnlim=max(0,(flux_soil[l].fast.nitrogen+flux_soil[l].slow.nitrogen)/fn_som);
        }*/
        h2o_mt=(flux_soil[l].slow.carbon+flux_soil[l].fast.carbon)*WH2O/WCO2/1000; // water produced during oxic decomposition C6H12O6 + O2 -> CO2 + H2O
        soil->O2[l]-=(flux_soil[l].slow.carbon + flux_soil[l].fast.carbon)*WO2/WC;
        soil->pool[l].slow.carbon-=flux_soil[l].slow.carbon;
        soil->pool[l].fast.carbon-=flux_soil[l].fast.carbon;
        soil->pool[l].slow.nitrogen-=flux_soil[l].slow.nitrogen;
        soil->pool[l].fast.nitrogen-=flux_soil[l].fast.nitrogen;
        soil_cflux+=flux_soil[l].slow.carbon+flux_soil[l].fast.carbon;
        F_Nmineral=flux_soil[l].slow.nitrogen+flux_soil[l].fast.nitrogen;
        soil->NH4[l]+=F_Nmineral*(1-k_l);
#ifdef SAFE
//if(soil->NH4[l]=epsilon)
        if(soil->NH4[l]<-epsilon)
         fail(NEGATIVE_SOIL_NH4_ERR,TRUE,"1 Negative soil NH4=%g in layer %d in cell (%s) at mineralization",soil->NH4[l],l,sprintcoord(line,&stand->cell->coord));
#endif
        soil->NO3[l]+=F_Nmineral*k_l;
        getoutput(&stand->cell->output,N_MINERALIZATION,config)+=F_Nmineral*stand->frac;
        if(isagriculture(stand->type->landusetype))
          getoutput(&stand->cell->output,NMINERALIZATION_AGR,config)+=F_Nmineral*stand->frac;
        soil->k_mean[l].fast+=(param.k_soil10.fast*response[l]);
        soil->k_mean[l].slow+=(param.k_soil10.slow*response[l]);
        if (soil->O2[l]<0)
        {
          //fprintf(stderr,"\t\t methanogenese layer[%d] O2: %g CH4: %g C_max: %g\n",l,soil->O2[l],soil->CH4[l],C_max[l]);
          soil->O2[l] = 0;
        }
        /*Methane production (Rprod) (anoxic decomposition rate is taken from Khovorostyanov et al., 2008) C6H12O6 -> 3CO2 + 3CH4*/
        methaneflux_soil=soil->pool[l].fast.carbon*param.k_soil10.fast/k_red*gtemp_soil[l]*exp(-(soil->O2[l]/soildepth[l]*1000)/O2star);
        if(methaneflux_soil<0) methaneflux_soil=0;
        //fprintf(stdout,"\t\t Methane production: layer:%d methane-flux: %g\n",l,methaneflux_soil[l]);
        soil->pool[l].fast.carbon-=methaneflux_soil;
        soil->CH4[l]+=methaneflux_soil;

        /*THIS IS DEDICATED TO MICROBIOLOGICAL HEATING*/
#ifdef MICRO_HEATING
        soil->micro_heating[l]+=methaneflux_soil*m_heat_mt;
#endif
        soil->k_mean[l].fast+=(param.k_soil10.fast*response[l]);
        soil->k_mean[l].slow+=(param.k_soil10.slow*response[l]);

        /*methanotrophy */
        if((soil_moist[l]<1) && layerbound[l]<=soil->wtable)
        {    //if((soil_moist[l]<1 && soil_moist[l]>epsilon) && soil->CH4[l]/soildepth[l]/epsilon_O2*1000>CH4_air && layerbound[l]<=soil->wtable)
           /*maybe calculating during diffusivity */
          //fprintf(stdout,"\t\t oxidation layer[%d] O2: %g CH4: %g\n",l,soil->O2[l],soil->CH4[l]);
          oxidation=(Vmax_CH4*1e-3*24*WCH4*soil->CH4[l]/soildepth[l]/epsilon_O2*1000)/(km_CH4*1e-3*WCH4+soil->CH4[l]/soildepth[l]/epsilon_O2*1000)*gtemp_soil[l]*soildepth[l]*epsilon_O2/1000;   // gCH4/m3/h*24 = gCH4/m3/d ->g/layer/m2
          //oxidation=oxidation*(soil->O2[l]/soildepth[l]/epsilon_O2*1000)/(km_O2*1e-3*WO2+soil->O2[l]/soildepth[l]/epsilon_O2*1000); //SEGERS 1997 if ----- >> CH4 + 2O2 -> CO2 + 2H2O
          O2_need=min(oxidation*soildepth[l]*epsilon_O2/1000*2*WO2/WCH4,soil->O2[l]);
          oxidation=O2_need/(2*WO2)*WCH4;
          oxidation=min(oxidation,soil->CH4[l]);
          soil->O2[l]-=oxidation*2*WO2/WCH4;
          if (soil->O2[l]<0)                                                /* TODO BE CAREFULL FOR THE O2 BALANCE HERE*/
            soil->O2[l]=0;
          soil->CH4[l]-=oxidation;
          /*THIS IS DEDICATED TO MICROBIOLOGICAL HEATING*/
#ifdef MICRO_HEATING
          soil->micro_heating[l]+=oxidation*m_heat_ox;
#endif
          h2o_mt+=oxidation*2*WH2O/WCH4/1000;                               // water produced during methane oxidation CH4 + 2O2 -> CO2 + 2H2O
         /*if(soil->O2[l]>1000  ||soil->O2[l]< 0 ||soil->CH4[l]< 0 ){*/
        // fprintf(stdout,"\t\t oxidation: %g %g O2: %g CH4: %g\n",oxidation,oxidation*WO2/WCH4,soil->O2[l],soil->CH4[l]);
           /*}*/
        }
        if (h2o_mt>0)
        {
          soil->w[l]+=(h2o_mt/soil->whcs[l]);
          if (soil->w[l]>1)
          {
            soil->w_fw[l]+=(1-soil->w[l])*soil->whcs[l];
            soil->w[l]=1;
          }
          if (soil->w_fw[l]>soil->wsats[l]-soil->whcs[l]-soil->wpwps[l])
          {
            *runoff+=soil->w_fw[l]-(soil->wsats[l]-soil->whcs[l]-soil->wpwps[l]);
            soil->w_fw[l]=(soil->wsats[l]-soil->whcs[l]-soil->wpwps[l]);
          }
          *MT_water+=h2o_mt;
        }
        soil_cflux+=oxidation;
#ifdef MICRO_HEATING
        soil->decomC[l]=flux_soil[l].slow.carbon+flux_soil[l].fast.carbon+oxidation;
#endif
        //soil_cflux+=*methaneflux_soil*WC/WCH4;      //CO2 produced during methane production C6H12O6 -> 3CO2 + 3CH4, already in deomposition?? mass balance not closed than!! Have to think about it


      } /*LASTLAYER*/
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
    }  /*end gtemp > 0 */
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
      response_agsub_wood=pow(soil->litter.item[p].pft->k_litter10.q10_wood,(soil->temp[0]-10)/10.0)*(INTERCEPT+MOIST_3*(moist[0]*moist[0]*moist[0])+MOIST_2*(moist[0]*moist[0])+MOIST*moist[0]);
      w_agtop=soil->litter.agtop_wcap>epsilon ? soil->litter.agtop_moist/soil->litter.agtop_wcap : moist[0];
      response_agtop_leaves=temp_response(soil->litter.agtop_temp,soil->amean_temp[0])*(INTERCEPT+MOIST_3*(w_agtop*w_agtop*w_agtop)+MOIST_2*(w_agtop*w_agtop)+MOIST*w_agtop);
      response_agtop_wood=pow(soil->litter.item[p].pft->k_litter10.q10_wood,(soil->litter.agtop_temp-10)/10.0)*(INTERCEPT+MOIST_3*(w_agtop*w_agtop*w_agtop)+MOIST_2*(w_agtop*w_agtop)+MOIST*w_agtop);
      response_bg_litter=response[0];

      decom_sum.carbon=decom_sum.nitrogen=0;
      /* agtop leaves */
#ifdef LINEAR_DECAY
      decay_litter=soil->litter.item[p].pft->k_litter10.leaf*response_agtop_leaves;
#else
      decay_litter=1.0-exp(-(soil->litter.item[p].pft->k_litter10.leaf*response_agtop_leaves));
#endif
      /*     if(soil->litter.ag[p].pft->type==TREE)
      litter_pores_wood=1-(soil->litter.ag[p].pft->fuelbulkdensity*1000/(wooddens/0.45));
      if(soil->litter.ag[p].pft->type==GRASS)
      litter_pores_grass=1-(soil->litter.ag[p].pft->fuelbulkdensity*1000/grassDM_vol);
      litter_height+=soil->litter.ag[p].pft->fuelbulkdensity*1000/(soil->litter.ag[p].trait.leaf/0.45);*/  //carbon2DM 1/0.45
      if(decay_litter>1) decay_litter=1;
      decom=soil->litter.item[p].ag.leaf.carbon*decay_litter;
      soil->litter.item[p].ag.leaf.carbon-=decom;
      decom_sum.carbon+=decom;
      decom=soil->litter.item[p].ag.leaf.nitrogen*decay_litter;
      soil->litter.item[p].ag.leaf.nitrogen-=decom;
      decom_sum.nitrogen+=decom;
      if (soil->wtable<=20 && gtemp_soil[0]>0 && soil->litter.item[p].ag.leaf.carbon>0)
      {
        litter_flux = soil->litter.item[p].ag.leaf.carbon*soil->litter.item[p].pft->k_litter10.leaf / k_red_litter*gtemp_soil[0];   // * exp((-soil->O2[0] / soildepth[0] * 1000) / O2star);
        soil->litter.item[p].ag.leaf.carbon -= litter_flux;
        *methaneflux_litter+=litter_flux;
      }

      /* agtop wood */
#ifdef LINEAR_DECAY
      decay_litter=soil->litter.item[p].pft->k_litter10.wood*response_agtop_wood;
#else
      decay_litter=1.0-exp(-(soil->litter.item[p].pft->k_litter10.wood*response_agtop_wood));
#endif
      if(decay_litter>1) decay_litter=1;
      for(i=0;i<NFUELCLASS;i++)
      {
        decom=soil->litter.item[p].ag.wood[i].carbon*decay_litter;
        soil->litter.item[p].ag.wood[i].carbon-=decom;
        decom_sum.carbon+=decom;
        decom=soil->litter.item[p].ag.wood[i].nitrogen*decay_litter;
//      if(decom=epsilon) fprintf(stdout,"decom %g litter.item[%d].ag.wood[%d].nit %g decay_litter %g\n",decom,p,i,soil->litter.item[p].ag.wood[i].nitrogen,decay_litter);
        soil->litter.item[p].ag.wood[i].nitrogen-=decom;
        decom_sum.nitrogen+=decom;
        if (soil->wtable<=20 && gtemp_soil[0]>0 && soil->litter.item[p].ag.wood[i].carbon>0)
        {
          litter_flux = soil->litter.item[p].ag.wood[i].carbon*soil->litter.item[p].pft->k_litter10.wood/k_red_litter*gtemp_soil[0];     // *exp((-soil->O2[0]/soildepth[0]*1000)/O2star);
          soil->litter.item[p].ag.wood[i].carbon-=litter_flux;
          *methaneflux_litter += litter_flux;
        }
      }

      /* agsub leaves */
#ifdef LINEAR_DECAY
      decay_litter=soil->litter.item[p].pft->k_litter10.leaf*response_agsub_leaves;
#else
      decay_litter=1.0-exp(-(soil->litter.item[p].pft->k_litter10.leaf*response_agsub_leaves));
#endif
      if(decay_litter>1) decay_litter=1;
      decom=soil->litter.item[p].agsub.leaf.carbon*decay_litter;
      soil->litter.item[p].agsub.leaf.carbon-=decom;
      decom_sum.carbon+=decom;
      decom=soil->litter.item[p].agsub.leaf.nitrogen*decay_litter;
      soil->litter.item[p].agsub.leaf.nitrogen-=decom;
      decom_sum.nitrogen+=decom;
      if (soil->wtable<=20 && gtemp_soil[0]>0 && soil->litter.item[p].agsub.leaf.carbon>0)
      {
        litter_flux = soil->litter.item[p].agsub.leaf.carbon * soil->litter.item[p].pft->k_litter10.leaf/ k_red_litter*gtemp_soil[0]; //* exp((-soil->O2[0] / soildepth[0] * 1000) / O2star);
        soil->litter.item[p].agsub.leaf.carbon -= litter_flux;
        *methaneflux_litter += litter_flux;
      }

      /* agsub wood */
#ifdef LINEAR_DECAY
      decay_litter=soil->litter.item[p].pft->k_litter10.wood*response_agsub_wood;
#else
      decay_litter=1.0-exp(-(soil->litter.item[p].pft->k_litter10.wood*response_agsub_wood));
#endif
      if(decay_litter>1) decay_litter=1;
      for(i=0;i<NFUELCLASS;i++)
      {
        decom=soil->litter.item[p].agsub.wood[i].carbon*decay_litter;
        soil->litter.item[p].agsub.wood[i].carbon-=decom;
        decom_sum.carbon+=decom;
        decom=soil->litter.item[p].agsub.wood[i].nitrogen*decay_litter;
        soil->litter.item[p].agsub.wood[i].nitrogen-=decom;
        decom_sum.nitrogen+=decom;
        // here agsub metahne routine is missing
        if (soil->wtable<=20 && gtemp_soil[0]>0 && soil->litter.item[p].agsub.wood[i].carbon>0)
        {
          litter_flux = soil->litter.item[p].agsub.wood[i].carbon * soil->litter.item[p].pft->k_litter10.wood / k_red_litter*gtemp_soil[0]; // * exp((-soil->O2[0] / soildepth[0] * 1000) / O2star);
          soil->litter.item[p].agsub.wood[i].carbon -= litter_flux;
          *methaneflux_litter += litter_flux;
        }
      }

      /* bg litter */
#ifdef LINEAR_DECAY
      decay_litter=param.k_litter10*response_bg_litter;
#else
      decay_litter=1.0-exp(-(param.k_litter10*response_bg_litter));
#endif
      if(decay_litter>1) decay_litter=1;
      decom=soil->litter.item[p].bg.carbon*decay_litter;
      soil->litter.item[p].bg.carbon-=decom;
      decom_sum.carbon+=decom;
      decom=soil->litter.item[p].bg.nitrogen*decay_litter;
      soil->litter.item[p].bg.nitrogen-=decom;
      decom_sum.nitrogen+=decom;
      decom_litter.carbon+=decom_sum.carbon;
      decom_litter.nitrogen+=decom_sum.nitrogen;
      if (soil->wtable<=200 && gtemp_soil[0]>0 && soil->litter.item[p].bg.carbon>0)
      {
        litter_flux = soil->litter.item[p].bg.carbon * param.k_litter10 / k_red_litter*gtemp_soil[0]; // * exp((-soil->O2[0] / soildepth[0] * 1000) / O2star);
        soil->litter.item[p].bg.carbon-=litter_flux;
        *methaneflux_litter+=litter_flux;
      }
      forrootsoillayer(l)
      {
        soil->pool[l].fast.carbon+=param.fastfrac*(1-param.atmfrac)*decom_sum.carbon*soil->c_shift[l][soil->litter.item[p].pft->id].fast;
        soil->pool[l].slow.carbon+=(1-param.fastfrac)*(1-param.atmfrac)*decom_sum.carbon*soil->c_shift[l][soil->litter.item[p].pft->id].slow;
        if(decom_sum.carbon>0 && stand->type->landusetype==NATURAL)
        {
          getoutputindex(&stand->cell->output,CSHIFT_FAST_NV,l,config)+=param.fastfrac*(1-param.atmfrac)*decom_sum.carbon*soil->c_shift[l][soil->litter.item[p].pft->id].fast;
          getoutputindex(&stand->cell->output,CSHIFT_SLOW_NV,l,config)+=(1-param.fastfrac)*(1-param.atmfrac)*decom_sum.carbon*soil->c_shift[l][soil->litter.item[p].pft->id].slow;
        }
        if(config->with_nitrogen)
        {
          if(decom_sum.nitrogen>0)
          {
            soil->pool[l].slow.nitrogen+=(1-param.fastfrac)*(1-param.atmfrac)*decom_sum.nitrogen*soil->c_shift[l][soil->litter.item[p].pft->id].slow;
            soil->pool[l].fast.nitrogen+=param.fastfrac*(1-param.atmfrac)*decom_sum.nitrogen*soil->c_shift[l][soil->litter.item[p].pft->id].fast;
            /* NO3 and N2O from mineralization of organic matter */
            F_Nmineral=max(0,decom_sum.nitrogen*param.atmfrac*(param.fastfrac*soil->c_shift[l][soil->litter.item[p].pft->id].fast+(1-param.fastfrac)*soil->c_shift[l][soil->litter.item[p].pft->id].slow));
            soil->NH4[l]+=F_Nmineral*(1-k_l);
#ifdef SAFE
            if(soil->NH4[l]<-epsilon)
              fail(NEGATIVE_SOIL_NH4_ERR,TRUE,"2 Negative soil NH4=%g in layer %d in cell (%s) at mineralization",soil->NH4[l],l,sprintcoord(line,&stand->cell->coord));
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
              if(soil->NH4[l]<0)
              {
                 soil->pool[l].fast.nitrogen+=soil->NH4[l];
                 soil->NH4[l]=0;
              }
              soil->NO3[l]-=n_immo*soil->NO3[l]/N_sum;
              if(soil->NO3[l]<0)
              {
                 soil->pool[l].fast.nitrogen+=soil->NO3[l];
                 soil->NO3[l]=0;
              }
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
              if(soil->NH4[l]<0)
              {
                 soil->pool[l].slow.nitrogen+=soil->NH4[l];
                 soil->NH4[l]=0;
              }
              soil->NO3[l]-=n_immo*soil->NO3[l]/N_sum;
              if(soil->NO3[l]<0)
              {
                 soil->pool[l].slow.nitrogen+=soil->NO3[l];
                 soil->NO3[l]=0;
              }
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
/*
       fprintf(stdout,"layer %d K_v[0]=%g ,K_v[[1]=%g \n",l, K_v[l][0],K_v[l][1]);
     fprintf(stdout,"soilold: %g soilall: %g fastcarbon: %g\n",soilold[l].fast.carbon,soilall[l].carbon,soil->pool[l].fast.carbon);
*/


#ifdef SAFE
      if(soil->NO3[l]<-epsilon)
        fail(NEGATIVE_SOIL_NO3_ERR,TRUE,"littersom: Negative soil NO3=%g in layer %d in cell (%s)",
             soil->NO3[l],l,sprintcoord(line,&stand->cell->coord));
#endif

      soil->NH4[l]-=F_NO3;
#ifdef SAFE
      if(soil->NH4[l]<-epsilon)
        fail(NEGATIVE_SOIL_NH4_ERR,TRUE,"Negative soil NH4=%g in layer %d in cell (%s)",
             soil->NH4[l],l,sprintcoord(line,&stand->cell->coord));
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

//saturation soil carbon calculation

  forrootsoillayer(l)
  {
	  C_tot[l]=(soil->pool[l].slow.carbon+soil->pool[l].fast.carbon)/soildepth[l];
      soilold[l].slow.nitrogen=soil->pool[l].slow.nitrogen;
      soilold[l].slow.carbon=soil->pool[l].slow.carbon;
      soilold[l].fast.nitrogen=soil->pool[l].fast.nitrogen;
      soilold[l].fast.carbon=soil->pool[l].fast.carbon;
      soilall[l].carbon=soil->pool[l].fast.carbon+soil->pool[l].slow.carbon;
      soilall[l].nitrogen=soil->pool[l].fast.nitrogen+soil->pool[l].slow.nitrogen;
      C_eff[l]=C_tot[l]/(S*soil->wsat[l]);
  }

  K_v[0][0]=KOVCON*(K_vdiff[0][0]*C_eff[0]+K_vdiff[0][1]*C_eff[0]+K_vdiff[0][2]*C_eff[1]);
  if(K_v[0][0]>0)
	if(C_eff[1]>C_eff[0]) {
	  K_v[0][1]=-1;
	 } else {
	    K_v[0][1]=0;
    }

  for(l=1;l<LASTLAYER;l++)
  {
    if(layerbound[l]<=soil->maxthaw_depth){
	    dKOVCON=KOVCON;
    }else{
        dKOVCON=0.0;
    }
    K_v[l][0]=dKOVCON*(K_vdiff[l][0]*C_eff[l-1]+ K_vdiff[l][1]*C_eff[l]+K_vdiff[l][2]*C_eff[l+1]);
    if(K_v[l][0]>0) {
	  if(C_eff[l+1]>C_eff[l])
		K_v[l][1]=-1;
    } else if (C_eff[l-1]>C_eff[l]) {
    	K_v[l][1]=1;
	} else {
	  	K_v[l][1]=0;
    }
  }

  K_v[LASTLAYER-1][0]=dKOVCON*(K_vdiff[LASTLAYER-1][0]*C_eff[LASTLAYER-2]+ K_vdiff[LASTLAYER-1][1]*C_eff[LASTLAYER-1]+K_vdiff[LASTLAYER-1][2]*C_eff[LASTLAYER-1]);

  if(K_v[LASTLAYER-1][0]> 0.) {
	  K_v[LASTLAYER-1][0]=1;
  } else {
	  K_v[LASTLAYER-1][0]=0;
  }

  for(l=(LASTLAYER-1);l==0;l--)
  {
    if(K_v[l][1] == 1) {
  	    if((K_v[l][0]*(soildepth[l]/soildepth[l-1])) > (0.5*C_tot[l-1]))
  	    	K_v[l][0]=(0.5*C_tot[l-1])*(soildepth[l-1]/soildepth[l]);

			C_tot[l-1]-=(K_v[l][0]*(soildepth[l]/soildepth[l-1]));
  	} else if (K_v[l][1] == -1) {
  	    if((K_v[l][0]*(soildepth[l]/soildepth[l+1])) > (0.5*C_tot[l+1]))
  	    	K_v[l][0]=(0.5*C_tot[l+1])*(soildepth[l+1]/soildepth[l]);
    }
  }


  //printf("vorher: TOTalCarbon: %.4f\n", soilstocks(soil).carbon);

  forrootsoillayer(l)
    if(l>0 && l<NSOILLAYER)
    {
      if(soilall[l].carbon>epsilon && soilall[l+1].carbon>epsilon && soilall[l-1].carbon>epsilon)
	  {
        if(K_v[l][1]==1){
          soil->pool[l-1].fast.carbon-=(K_v[l][0]*(soildepth[l]/soildepth[l-1])*(soilold[l-1].fast.carbon/soilall[l-1].carbon));
          soil->pool[l].fast.carbon+=(K_v[l][0]*(soildepth[l]/soildepth[l-1])*(soilold[l-1].fast.carbon/soilall[l-1].carbon));
          soil->pool[l-1].slow.carbon-=(K_v[l][0]*(soildepth[l]/soildepth[l-1])*((soilold[l-1].slow.carbon/soilall[l-1].carbon)));
          soil->pool[l].slow.carbon+=(K_v[l][0]*(soildepth[l]/soildepth[l-1])*(soilold[l-1].slow.carbon/soilall[l-1].carbon));
        }else if (K_v[l][1]==-1){
          soil->pool[l+1].fast.carbon-=(K_v[l][0]*(soildepth[l]/soildepth[l+1])*(soilold[l+1].fast.carbon/soilall[l+1].carbon));
          soil->pool[l].fast.carbon+=(K_v[l][0]*(soildepth[l]/soildepth[l+1])*(soilold[l+1].fast.carbon/soilall[l+1].carbon));
          soil->pool[l+1].slow.carbon-=(K_v[l][0]*(soildepth[l]/soildepth[l+1])*(soilold[l+1].slow.carbon/soilall[l+1].carbon));
          soil->pool[l].slow.carbon+=(K_v[l][0]*(soildepth[l]/soildepth[l+1])*(soilold[l+1].slow.carbon/soilall[l+1].carbon));
        }
	  }
    }
//printf("nachher: TOTalCarbon: %.4f\n", soilstocks(soil).carbon);




#ifdef MICRO_HEATING
  soil->litter.decomC=decom_litter.carbon*param.atmfrac; /*only for mircobiological heating*/
#endif
  soil->count++;
  getoutput(&stand->cell->output,RH_LITTER,config)+=decom_litter.carbon*param.atmfrac*stand->frac;
  if(isagriculture(stand->type->landusetype))
    getoutput(&stand->cell->output,RH_AGR,config) += (decom_litter.carbon*param.atmfrac+soil_cflux)*stand->frac;
  flux.carbon=decom_litter.carbon*param.atmfrac+soil_cflux;
#ifdef CHECK_BALANCE
  end = soilstocks(soil);
  end.carbon+=soilmethane(soil);
  if (fabs(start.carbon - end.carbon - (flux.carbon + *methaneflux_litter))>0.0001)
    fprintf(stdout, "C-ERROR in littersom: iswetland: %d %.5f start:%.5f  ende:%.5f decomCO2: %g methane_em: %.5f\n", soil->iswetland, start.carbon - end.carbon - (flux.carbon + *methaneflux_litter), start.carbon, end.carbon, flux.carbon, *methaneflux_litter);
  if (fabs(start.nitrogen - end.nitrogen - flux.nitrogen)>0.00001)
    fprintf(stdout, "N-ERROR in littersom: iswetland: %d %.5f start:%.5f  end:%.5f decomCO2: %g\n", soil->iswetland, start.nitrogen - end.nitrogen - flux.nitrogen, start.nitrogen, end.nitrogen, flux.nitrogen);
#endif
  return flux;
} /* of 'littersom' */

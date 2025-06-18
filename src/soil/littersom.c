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
#define k_red 1                           /*anoxic decomposition is much smaller than oxic decomposition*/
#define k_red_litter 1
#define INTERCEPT 0.04021601              /* changed from 0.10021601 now again original value*/
#define MOIST_3 -5.00505434
#define MOIST_2 4.26937932
#define MOIST  0.71890122
#define DECOM_SLOW  0.000005               /*very slow decomposition rate under ice cover*/
#define DECOM_FAST  0.00002                /*very slow decomposition rate under ice cover*/
#define CN_ratio_fast 8
#define CN_ratio_slow 12
#define k_N 5e-3 /* Michaelis-Menten parameter k_S,1/2 (gN/m3) */
#define S 0.2587 // saturation factor MacDougall and Knutti, 2016
#define KOVCON (0.001*1000) //Constant of diffusion (m2a-1)
#define WTABTHRES 3000
#define Q10 1.8
#define timesteps 30
#define LINEAR_DECAY

//#define CALC_EFF_CARBON

static Real f_wfps(const Soil *soil,      /* Soil data */
                   int l                  /* soil layer */
                  )                       /* return soil temperature (deg C) */
{
  Real x;
  x=(soil->w[l]*soil->whcs[l]+soil->ice_depth[l]+soil->wpwps[l]+soil->w_fw[l]+soil->ice_fw[l])/soil->wsats[l];
  return pow((x-soil->par->b_nit)/soil->par->n_nit,soil->par->z_nit)*
      pow((x-soil->par->c_nit)/soil->par->m_nit,soil->par->d_nit);
} /* of 'f_wfps' */

static Real f_ph(Real ph)
{
  return 0.56 + atan(M_PI*0.45*(-5 + ph)) / M_PI;
} /* of 'f_ph' */

Stocks littersom(Stand *stand,                      /**< [inout] pointer to stand data */
                 const Real gtemp_soil[NSOILLAYER], /**< [in] respiration coefficents */
                 Real cellfrac_agr,                 /**< [in] stand fraction of agricultural cells (0..1) */
                 Real *methaneflux_litter,          /**< [out] CH4 emissions (gC/m2/day) */
                 Real airtemp,                      /**< [in] air temperature (deg C) */
                 Real pch4,                         /**< [in] atmospheric methane (ppm) */
                 Real *runoff,                      /**< [out] runoff (mm/day) */
                 Real *MT_water,                    /**< [out] water from oxidized methane (mm/day) */
                 Real *ch4_sink,                    /**< [out] negative CH4 emissions (gC/m2/day) */
                 int npft,                          /**< [in] number of natural PFTs */
                 int ncft,                          /**< [in] number of crop PFTs */
                 const Config *config               /**< [in] LPJmL configuration */
                )                                   /** \return decomposed carbon/nitrogen (g/m2) */
{
  Real response[LASTLAYER];
  Real response_agtop_leaves,response_agtop_wood,response_agsub_leaves,response_agsub_wood,w_agtop;
  Real decay_litter, oxidation, litter_flux;
  Pool flux_soil[LASTLAYER];
  Poolpar k_soil10;
  Real decom,soil_cflux;
  Stocks decom_fast,decom_slow;
  Stocks decom_litter;
  Stocks decom_sum,flux;
  Real moist[LASTLAYER],soil_moist[LASTLAYER];
  Real N_sum=0;
  Real n_immo=0;
  int i,p,l,dt;
  Soil *soil;
  Real CH4_em=0;
  Real CH4_sink=0;
  Real yedoma_flux;
  Real F_NO3=0;     /* soil nitrification rate gN *m-2*d-1*/
  Real F_N2O=0;     /* soil nitrification rate gN *m-2*d-1*/
  Real F_Nmineral,F_Nmineral_all;  /* net mineralization flux gN *m-2*d-1*/
  Real fac_wfps, fac_temp;
#ifdef SAFE
  String line;
#endif
  Pft *pft;
  Pftcrop *crop;
  Irrigation *data;
  /* FOR METHANE IMPLEMENTATION */
  Real C_max[LASTLAYER], V;
  Real CN_fast=0, CN_slow=0;
  Real cn_bg;
  Real methaneflux_soil;
  Real oxidation_stand=0;    //oxidation of methane with in the soil column
  Real h2o_mt;   /* methane production */
  Real O2_need;
  Real epsilon_O2 = 0;
  Real temp;
  Real socfraction[BOTTOMLAYER];


// IMPLEMENTATION OF THE EFFECTIVE CARBON CONCENTRATION
#ifdef CALC_EFF_CARBON
  Real C_eff[LASTLAYER];
  Real C_tot[LASTLAYER];
  Real K_v[NSOILLAYER][2];
  Real K_vdiff[LASTLAYER][3];
  Real dKOVCON;
  Pool soilold[LASTLAYER];
  Stocks soilall[LASTLAYER];
#endif
  Real NH4_mineral;
  soil=&stand->soil;
#ifdef CHECK_BALANCE
  Stocks start;
  Stocks end;
  Real water_before=soilwater(soil);
  Real water_after=0;
  Real balanceW=0;
  start = soilstocks(soil);
  start.carbon+=soilmethane(soil)*WC/WCH4;
#endif
  k_soil10.fast=param.k_soil10.fast*(1-soil->icefrac)+DECOM_FAST*soil->icefrac;
  k_soil10.slow=param.k_soil10.slow*(1-soil->icefrac)+DECOM_SLOW*soil->icefrac;
  flux.nitrogen=0;
  forrootsoillayer(l){
    response[l]=0.0;
  }
  decom_litter.carbon=decom_litter.nitrogen=soil_cflux=yedoma_flux=decom_sum.carbon=decom_sum.nitrogen=decom_fast.carbon=decom_slow.carbon=decom_fast.nitrogen=
      decom_slow.nitrogen=F_Nmineral_all=oxidation=litter_flux=0.0;
#ifdef CALC_EFF_CARBON
  K_vdiff[0][0]=2./(midlayer[0]*(midlayer[0]+(midlayer[1]-midlayer[0])));
  K_vdiff[0][1]=-2./(midlayer[0]*(midlayer[1]-midlayer[0]));
  K_vdiff[0][2]=2./((midlayer[1]-midlayer[0])*(midlayer[0]+(midlayer[1]-midlayer[0])));
  forrootsoillayer(l)
  {
    if(l>0 && l<LASTLAYER)
    {
      K_vdiff[l][0]=2./((midlayer[l]-midlayer[l-1])*((midlayer[l]-midlayer[l-1])+(midlayer[l+1]-midlayer[l])));
      K_vdiff[l][1]=-2./((midlayer[l]-midlayer[l-1])*((midlayer[l+1]-midlayer[l])));
      K_vdiff[l][2]=2./((midlayer[l+1]-midlayer[l])*((midlayer[l]-midlayer[l-1])+(midlayer[l+1]-midlayer[l])));
    }
  }
#endif

  *methaneflux_litter=*runoff=*MT_water=0;
  forrootsoillayer(l)
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
    soil_moist[l] = (soil->w[l] * soil->whcs[l] + soil->wpwps[l] + soil->w_fw[l]) / soil->wsats[l];
    if (moist[l]<epsilon)
       moist[l]=epsilon;

     response[l]=gtemp_soil[l]*(INTERCEPT+MOIST_3*(moist[l]*moist[l]*moist[l])+MOIST_2*(moist[l]*moist[l])+MOIST*moist[l]);
     if (response[l]<epsilon)
       response[l]= epsilon + epsilon;
     if (response[l]>1)
       response[l]=1.0;
  }

  for(dt=0;dt<timesteps;dt++)
  {
    forrootsoillayer(l)
    {
      if(gtemp_soil[l]>epsilon)
      {
        C_max[l]=max(0,soil->O2[l]*WC/WO2);
        if(stand->type->landusetype==NATURAL || stand->type->landusetype==WETLAND)
          getoutputindex(&stand->cell->output,RESPONSE_LAYER_NV,l,config)+=response[l];
        if(isagriculture(stand->type->landusetype))
          getoutputindex(&stand->cell->output,RESPONSE_LAYER_AGR,l,config)+=response[l]*stand->frac/cellfrac_agr;
#ifdef SAFE
        if(soil->NO3[l]<-epsilon)
          fail(NEGATIVE_SOIL_NO3_ERR,TRUE,TRUE,"littersom: Negative soil NO3=%g in layer %d in cell (%s) before update",soil->NO3[l],l,sprintcoord(line,&stand->cell->coord));
        if(soil->NH4[l]<-epsilon)
          fail(NEGATIVE_SOIL_NH4_ERR,TRUE,TRUE,"littersom: Negative soil NH4=%g in layer %d in cell (%s) before update",soil->NH4[l],l,sprintcoord(line,&stand->cell->coord));
#endif
#ifdef LINEAR_DECAY
        flux_soil[l].slow.carbon=max(0,soil->pool[l].slow.carbon*k_soil10.slow/timesteps*response[l]);
        flux_soil[l].fast.carbon=max(0,soil->pool[l].fast.carbon*k_soil10.fast/timesteps*response[l]);
        flux_soil[l].slow.nitrogen=max(0,soil->pool[l].slow.nitrogen*k_soil10.slow/timesteps*response[l]);
        flux_soil[l].fast.nitrogen=max(0,soil->pool[l].fast.nitrogen*k_soil10.fast/timesteps*response[l]);
#else
        flux_soil[l].slow.carbon=max(0,soil->pool[l].slow.carbon*(1.0-exp(-(k_soil10.slow/timesteps*response[l]))));
        flux_soil[l].fast.carbon=max(0,soil->pool[l].fast.carbon*(1.0-exp(-(k_soil10.fast/timesteps*response[l]))));
        flux_soil[l].slow.nitrogen=max(0,soil->pool[l].slow.nitrogen*(1.0-exp(-(k_soil10.slow/timesteps*response[l]))));
        flux_soil[l].fast.nitrogen=max(0,soil->pool[l].fast.nitrogen*(1.0-exp(-(k_soil10.fast/timesteps*response[l]))));
#endif

        if(flux_soil[l].fast.nitrogen>epsilon && flux_soil[l].fast.carbon>0)
          CN_fast=flux_soil[l].fast.carbon/flux_soil[l].fast.nitrogen;
        else
          CN_fast=soil->par->cn_ratio;
        if(CN_fast<epsilon) CN_fast=soil->par->cn_ratio;
        if(flux_soil[l].slow.nitrogen>epsilon)
          CN_slow=flux_soil[l].slow.carbon/flux_soil[l].slow.nitrogen;
        else
          CN_slow=soil->par->cn_ratio;
        if(CN_slow<epsilon) CN_slow=soil->par->cn_ratio;

        if (flux_soil[l].slow.carbon>C_max[l])
        {
          flux_soil[l].slow.carbon=C_max[l];
          flux_soil[l].slow.nitrogen=flux_soil[l].slow.carbon/CN_fast;
          C_max[l]=0;
        }
        else
          C_max[l]-=flux_soil[l].slow.carbon;
        if (flux_soil[l].fast.carbon>C_max[l])
        {
          flux_soil[l].fast.carbon=C_max[l];
          flux_soil[l].fast.nitrogen=flux_soil[l].fast.carbon/CN_slow;
          C_max[l]=0;
        }
        else
          C_max[l]-=flux_soil[l].fast.carbon;

        /**
         * @brief NO3 and N2O from nitrification
         *
         * This describes the process or calculation of nitrate (NO3) and nitrous oxide (N2O)
         * produced during nitrification.
         */

        fac_wfps = f_wfps(soil,l);
        fac_temp = f_temp(soil->temp[l]);
        F_NO3=min(soil->O2[l]*WN/(WO2*2),param.k_max/timesteps*soil->NH4[l]*fac_temp*fac_wfps*f_ph(stand->cell->soilph));
        F_NO3=max(0,F_NO3);
        if(F_NO3>soil->NH4[l])
          F_NO3=soil->NH4[l];
        F_N2O=param.k_2*F_NO3;
        soil->NO3[l]+=F_NO3*(1-param.k_2);
#ifdef SAFE
         if(soil->NO3[l]<-epsilon)
           fail(NEGATIVE_SOIL_NO3_ERR,FALSE,TRUE,"littersom: Negative soil NO3=%g in layer %d in cell (%s)",
               soil->NO3[l],l,sprintcoord(line,&stand->cell->coord));
#endif

        soil->NH4[l]-=F_NO3;
        soil->O2[l]-=F_NO3*2*WO2/WN;   // Nitrification: NH4+ + 2O2 -> NO3- + 2H+ + H2O  (WN = 14 g/mol)

#ifdef SAFE
         if(soil->NH4[l]<-epsilon)
           fail(NEGATIVE_SOIL_NH4_ERR,FALSE,TRUE,"Negative soil NH4=%g in layer %d in cell (%s)",
               soil->NH4[l],l,sprintcoord(line,&stand->cell->coord));
#endif
        flux.nitrogen += F_N2O;
        /* F_N2O is given back for output */
        if(stand->type->landusetype==AGRICULTURE)
        {
          foreachpft(pft,p,&stand->pftlist)
          {
            crop=pft->data;
            if(crop->sh!=NULL)
            {
              crop->sh->n2o_nitsum+=F_N2O;
              crop->sh->c_emissum+=decom_litter.carbon*param.atmfrac+soil_cflux;
            }
            else
            {
              data=stand->data;
              getoutputindex(&stand->cell->output,CFT_N2O_NIT,pft->par->id-npft+data->irrigation*ncft,config)+=F_N2O;
              getoutputindex(&stand->cell->output,CFT_C_EMIS,pft->par->id-npft+data->irrigation*ncft,config)+=decom_litter.carbon *param.atmfrac+soil_cflux;
           }
          }
        }

        oxidation = h2o_mt = 0;
        h2o_mt=(flux_soil[l].slow.carbon+flux_soil[l].fast.carbon)*WH2O/WC/1000; // water produced during oxic decomposition C6H12O6 + 6O2 -> 6CO2 + 6H2O
        soil->O2[l]-=(flux_soil[l].slow.carbon + flux_soil[l].fast.carbon)*WO2/WC;

        V = getV(soil,l);  /*soil air content (m3 air/m3 soil)*/
        if (V<=epsilon)
          V=epsilon+epsilon;
        //epsilon_O2=getepsilon_O2(V,soil_moist[l],soil->wsat[l]);

        /*methanotrophy */
        if((V>0.25) && soil->wtable>=layerbound[l] && soil->freeze_depth[l]<soildepth[l])
        {
          //oxidation=(Vmax_CH4*1e-3*24/timesteps*WCH4*soil->CH4[l]/soildepth[l]/soil->wsat[l]*1000)/(km_CH4*1e-3*WCH4+soil->CH4[l]/soildepth[l]/soil->wsat[l]*1000)*gtemp_soil[l]*soildepth[l]*soil->wsat[l]/1000;   // gCH4/m3/h*24 = gCH4/m3/d ->gCH4/layer/m2
          if (soil->temp[l]>40)
            temp = 40;
          else
            temp=soil->temp[l];
          oxidation=(Vmax_CH4*1e-3*24/timesteps*WCH4*soil->CH4[l]/soildepth[l]/ soil->wsat[l]*1000)/(km_CH4*1e-3*WCH4+soil->CH4[l]/soildepth[l]/soil->wsat[l]*1000)*pow(Q10,(temp-soil->amean_temp[l])/10)*soildepth[l]*soil->wsat[l]/1000;   // gCH4/m3/h*24 = gCH4/m3/d ->gCH4/layer/m2
          O2_need=min(oxidation*2*WO2/WCH4,soil->O2[l]*oxid_frac);            // g02/layer/m2
          oxidation=max(0,O2_need/(2*WO2)*WCH4);                                    // gCH4/layer/m2
          oxidation=min(oxidation,soil->CH4[l]);
          soil->O2[l]-=oxidation*2*WO2/WCH4;
          soil->CH4[l]-=oxidation;
          oxidation_stand+=oxidation;
          /*THIS IS DEDICATED TO MICROBIOLOGICAL HEATING*/
#ifdef MICRO_HEATING
          soil->micro_heating[l]+=oxidation*m_heat_ox;
#endif
          h2o_mt+=oxidation*WH2O/WCH4/1000*2;                               // water produced during methane oxidation CH4 + 2O2 -> CO2 + 2H2O (mm)
          soil_cflux+=oxidation*WC/WCH4;                                    // CO2 flux into the atmosphere due to oxidation of methane
        }
        if (h2o_mt>0)
        {
          if(soil->ice_depth[l]>=soil->whcs[l])
            *runoff+=h2o_mt;
          else
          {
            soil->w[l]+=(h2o_mt/soil->whcs[l]);
            if ((soil->w[l]*soil->whcs[l]+soil->ice_depth[l])>soil->whcs[l])
            {
              soil->w_fw[l]+=((soil->w[l]*soil->whcs[l]+soil->ice_depth[l])-soil->whcs[l]);
              soil->w[l]-=((soil->w[l]*soil->whcs[l]+soil->ice_depth[l])-soil->whcs[l])/soil->whcs[l];
            }
            if ((soil->w_fw[l]+soil->ice_fw[l])>soil->wsats[l]-soil->whcs[l]-soil->wpwps[l])
            {
              *runoff+=(soil->w_fw[l]+soil->ice_fw[l])-(soil->wsats[l]-soil->whcs[l]-soil->wpwps[l]);
              soil->w_fw[l]-=(soil->w_fw[l]+soil->ice_fw[l])-(soil->wsats[l]-soil->whcs[l]-soil->wpwps[l]);
            }
            if(soil->w_fw[l]<0)
            {
              soil->ice_fw[l]+=soil->w_fw[l];
              soil->w_fw[l]=0;
            }
          }
          *MT_water+=h2o_mt;
          h2o_mt=0;
        }


        /* TODO nitrogen limitation of decomposition including variable decay rates Brovkin,
        fn_som=flux_soil[l].fast.carbon/CN_ratio_fast+flux_soil[l].slow.carbon/CN_ratio_slow;

        if (fn_som>(flux_soil[l].fast.nitrogen+flux_soil[l].slow.nitrogen))
        {
          fnlim=max(0,(flux_soil[l].fast.nitrogen+flux_soil[l].slow.nitrogen)/fn_som);
        }*/

        /*Methane production (Rprod) (anoxic decomposition rate is taken from Khovorostyanov et al., 2008) C6H12O6 -> 3CO2 + 3CH4*/
        methaneflux_soil=0;
        if(soil->freeze_depth[l]<soildepth[l])
        {
          methaneflux_soil=soil->pool[l].fast.carbon*k_soil10.fast/k_red/timesteps*gtemp_soil[l]*exp(-(soil->O2[l]*oxid_frac/ soil->wsat[l]/soildepth[l]*1000)/O2star);
          if(methaneflux_soil<0) methaneflux_soil=0;
          soil->pool[l].fast.carbon-=methaneflux_soil*WC/WCH4;                                                      ///// *WC/WCH4 correct for CH4 mass
          getoutput(&stand->cell->output,METHANOGENESIS,config) += methaneflux_soil*stand->frac;
          soil->CH4[l]+=methaneflux_soil;
          NH4_mineral=min(soil->pool[l].fast.nitrogen,methaneflux_soil*WC/WCH4/CN_fast);
          NH4_mineral=max(0,NH4_mineral);
          soil->pool[l].fast.nitrogen-=NH4_mineral;
          soil->NH4[l]+=NH4_mineral;
        }

#ifdef SAFE
        if(soil->NH4[l]<-epsilon)
          fail(NEGATIVE_SOIL_NH4_ERR,TRUE,TRUE,"0 littersom: Negative soil NH4=%g in layer %d in cell (%s) before update",soil->NH4[l],l,sprintcoord(line,&stand->cell->coord));
#endif

        if(soil->pool[l].fast.carbon>epsilon)
          soil->decay_rate[l].fast+=(flux_soil[l].fast.carbon+methaneflux_soil)/soil->pool[l].fast.carbon;
        if(soil->pool[l].slow.carbon>epsilon)
          soil->decay_rate[l].slow+=(flux_soil[l].slow.carbon)/soil->pool[l].slow.carbon;
        soil->pool[l].slow.carbon-=flux_soil[l].slow.carbon;
        soil->pool[l].fast.carbon-=flux_soil[l].fast.carbon;
        soil->pool[l].slow.nitrogen-=flux_soil[l].slow.nitrogen;
        soil->pool[l].fast.nitrogen-=flux_soil[l].fast.nitrogen;
        soil_cflux+=flux_soil[l].slow.carbon+flux_soil[l].fast.carbon;
        F_Nmineral=flux_soil[l].slow.nitrogen+flux_soil[l].fast.nitrogen;
        soil->NH4[l]+=F_Nmineral;
#ifdef SAFE
        if(soil->NH4[l]<-epsilon)
          fail(NEGATIVE_SOIL_NH4_ERR,TRUE,TRUE,"1 Negative soil NH4=%g in layer %d in cell (%s) at mineralization F_Nmineral: %g NH4_mineral: %g CN_fast: %g flux_soil[%d].fast.nitrogen: %g methaneflux_soil: %g epsilon_O2: %g soil->pool[l].fast.carbon: %g flux: %g",
              soil->NH4[l],l,sprintcoord(line,&stand->cell->coord),F_Nmineral,NH4_mineral,CN_fast,l,flux_soil[l].fast.nitrogen,methaneflux_soil,epsilon_O2,soil->pool[l].fast.carbon,methaneflux_soil*WC/WCH4/CN_fast);
#endif
        getoutput(&stand->cell->output,N_MINERALIZATION,config)+=F_Nmineral*stand->frac;
        if(isagriculture(stand->type->landusetype))
          getoutput(&stand->cell->output,NMINERALIZATION_AGR,config)+=F_Nmineral*stand->frac;
        soil->k_mean[l].fast+=(k_soil10.fast/timesteps*response[l]);
        soil->k_mean[l].slow+=(k_soil10.slow/timesteps*response[l]);
        if (soil->O2[l]<0)
        {
          soil->O2[l] = 0;
        }

        /*THIS IS DEDICATED TO MICROBIOLOGICAL HEATING*/
#ifdef MICRO_HEATING
        soil->micro_heating[l]+=methaneflux_soil*m_heat_mt;
        soil->decomC[l]=flux_soil[l].slow.carbon+flux_soil[l].fast.carbon+oxidation*WC/WCH4;
#endif
#ifdef SAFE
        if (soil->w[l]< -epsilon || soil->w_fw[l]< -epsilon )
        {
          fprintf(stderr,"\nlittersom Cell (%s) soilwater=%.6f soilice=%.6f wsats=%.6f agtop_moist=%.6f\n",
              sprintcoord(line,&stand->cell->coord),allwater(soil,l),allice(soil,l),soil->wsats[l],soil->litter.agtop_moist);
          fflush(stderr);
          fprintf(stderr,"Soil-moisture layer %d negative: w:%g, fw:%g,lutype %s soil_type %s \n\n",
              l,soil->w[l],soil->w_fw[l],stand->type->name,soil->par->name);
        }
#endif
        if(soil->pool[l].fast.carbon<-epsilon || soil->pool[l].slow.carbon<-epsilon)
        {
          fail(NEGATIV_SOIL_CARBON,TRUE,TRUE,"1 negative soil carbon: Lat: %g Lon: %g fast.carbon: %g slow.carbon: %g",stand->cell->coord.lat,stand->cell->coord.lon,soil->pool[l].fast.carbon,soil->pool[l].slow.carbon);
        }
#ifdef YEDOMA
        if (soil->YEDOMA>0.0 && response[LASTLAYER-1]>0.0)
        {
          yedoma_flux=soil->YEDOMA*(1.0-exp(-(K10_YEDOMA/timesteps*response[LASTLAYER-1])));
          soil->YEDOMA-=yedoma_flux;
          soil_cflux+=yedoma_flux;
#ifdef MICRO_HEATING
          soil->decomC[LASTLAYER]+=yedoma_flux;
#endif
        }
#endif
      }  /*end gtemp > 0 */
    } /* end forrootsoillayer */

    /*
     *   Calculate daily decomposition rates (k, /day) as a function of
     *   temperature and moisture
     *
     */

    for(p=0;p<soil->litter.n;p++)
    {
      decom_slow.carbon=decom_fast.nitrogen=decom_slow.nitrogen=decom_sum.carbon=decom_sum.nitrogen=0;
      if(gtemp_soil[0]>0)
      {
        V = getV(soil,0);  /*soil air content (m3 air/m3 soil)*/
        if (V<=epsilon)
          V=epsilon+epsilon;
        //epsilon_O2=getepsilon_O2(V,soil_moist[0],soil->wsat[l]);
        response_agsub_leaves=response[0];
        response_agsub_wood=pow(soil->litter.item[p].pft->k_litter10.q10_wood,(soil->temp[0]-10)/10.0)*(INTERCEPT+MOIST_3*(moist[0]*moist[0]*moist[0])+MOIST_2*(moist[0]*moist[0])+MOIST*moist[0]);
        w_agtop=soil->litter.agtop_wcap>epsilon ? soil->litter.agtop_moist/soil->litter.agtop_wcap : moist[0];
        response_agtop_leaves=temp_response(soil->litter.agtop_temp,soil->amean_temp[0])*(INTERCEPT+MOIST_3*(w_agtop*w_agtop*w_agtop)+MOIST_2*(w_agtop*w_agtop)+MOIST*w_agtop);
        response_agtop_wood=pow(soil->litter.item[p].pft->k_litter10.q10_wood,(soil->litter.agtop_temp-10)/10.0)*(INTERCEPT+MOIST_3*(w_agtop*w_agtop*w_agtop)+MOIST_2*(w_agtop*w_agtop)+MOIST*w_agtop);
        response_agtop_wood=max(epsilon+epsilon,response_agtop_wood);

        /* agtop leaves */
#ifdef LINEAR_DECAY
        decay_litter=soil->litter.item[p].pft->k_litter10.leaf/timesteps*response_agtop_leaves;
#else
        decay_litter=(1.0-exp(-(soil->litter.item[p].pft->k_litter10.leaf/timesteps*response_agtop_leaves)));
#endif
        /*     if(soil->litter.ag[p].pft->type==TREE)
      litter_pores_wood=1-(soil->litter.ag[p].pft->fuelbulkdensity*1000/(wooddens/0.45));
      if(soil->litter.ag[p].pft->type==GRASS)
      litter_pores_grass=1-(soil->litter.ag[p].pft->fuelbulkdensity*1000/grassDM_vol);
      litter_height+=soil->litter.ag[p].pft->fuelbulkdensity*1000/(soil->litter.ag[p].trait.leaf/0.45);*/  //carbon2DM 1/0.45
        if(decay_litter>1) decay_litter=1;

        if(soil->wtable>50)
        {
          decom=max(0,soil->litter.item[p].agtop.leaf.carbon*decay_litter);
          C_max[0]=min(decom,soil->O2[0]*WC/WO2);
          decom=max(0,C_max[0]);
          soil->litter.item[p].agtop.leaf.carbon-=decom;
          soil->O2[0]-=decom*WO2/WC;
          decom_sum.carbon+=decom;
          decom_fast.carbon+=decom;
          decom=max(0,soil->litter.item[p].agtop.leaf.nitrogen*decay_litter);
          soil->litter.item[p].agtop.leaf.nitrogen-=decom;
          decom_sum.nitrogen+=decom;
          decom_fast.nitrogen+=decom;
        }
        if (soil->wtable<=50 && soil->litter.item[p].agtop.leaf.carbon>0)
        {
          if(soil->litter.item[p].agtop.leaf.nitrogen>epsilon && soil->litter.item[p].agtop.leaf.carbon>epsilon)
            CN_fast=soil->litter.item[p].agtop.leaf.carbon/soil->litter.item[p].agtop.leaf.nitrogen;
          else
            CN_fast=soil->par->cn_ratio;
          litter_flux=soil->litter.item[p].agtop.leaf.carbon*soil->litter.item[p].pft->k_litter10.leaf / k_red_litter/timesteps*gtemp_soil[0]; //* exp(-(soil->O2[0]*oxid_frac/soil->wsat[l]/soildepth[l]*1000)/O2star);
          soil->litter.item[p].agtop.leaf.carbon -= litter_flux*WC/WCH4;
          *methaneflux_litter+=litter_flux;
          NH4_mineral=min(soil->litter.item[p].agtop.leaf.nitrogen,litter_flux*WC/WCH4/CN_fast);
          soil->litter.item[p].agtop.leaf.nitrogen-=NH4_mineral;
          decom_sum.nitrogen+=NH4_mineral;
          decom_fast.nitrogen+=NH4_mineral;
        }

        /* agtop wood */
#ifdef LINEAR_DECAY
        decay_litter=soil->litter.item[p].pft->k_litter10.wood/timesteps*response_agtop_wood;      // no methane production from above wood litter
#else
        decay_litter=(1.0-exp(-(soil->litter.item[p].pft->k_litter10.wood/timesteps*response_agtop_wood)));
#endif
        if(decay_litter>1) decay_litter=1;
        if(soil->wtable>50)
        {
          for(i=0;i<NFUELCLASS;i++)
          {
            decom=max(0,soil->litter.item[p].agtop.wood[i].carbon*decay_litter);
            C_max[0]=min(decom,soil->O2[0]*WC/WO2);
            decom=max(0,C_max[0]);
            soil->litter.item[p].agtop.wood[i].carbon-=decom;
            soil->O2[0]-=decom*WO2/WC;
            decom_sum.carbon+=decom;
            decom_slow.carbon+=decom;
            decom=max(0,soil->litter.item[p].agtop.wood[i].nitrogen*decay_litter);
            soil->litter.item[p].agtop.wood[i].nitrogen-=decom;
            decom_sum.nitrogen+=decom;
            decom_slow.nitrogen+=decom;
          }
        }
        /* agsub leaves only on agricultural stands in the first layer*/

#ifdef LINEAR_DECAY
        decay_litter=soil->litter.item[p].pft->k_litter10.leaf/timesteps*response_agsub_leaves;
#else
        decay_litter=(1.0-exp(-(soil->litter.item[p].pft->k_litter10.leaf/timesteps*response_agsub_leaves)));
#endif
        if(decay_litter>1) decay_litter=1;
        if (soil->wtable<=50 && soil->litter.item[p].agsub.leaf.carbon>0)
        {
          if(soil->litter.item[p].agsub.leaf.nitrogen>epsilon && soil->litter.item[p].agsub.leaf.carbon>epsilon)
            CN_fast=soil->litter.item[p].agsub.leaf.carbon/soil->litter.item[p].agsub.leaf.nitrogen;
          else
            CN_fast=soil->par->cn_ratio;
          litter_flux = soil->litter.item[p].agsub.leaf.carbon * soil->litter.item[p].pft->k_litter10.leaf/ k_red_litter/timesteps*gtemp_soil[0]* exp(-(soil->O2[0]*oxid_frac/soil->wsat[0]/soildepth[0]*1000)/O2star);
          soil->litter.item[p].agsub.leaf.carbon -= litter_flux*WC/WCH4;
          *methaneflux_litter += litter_flux;
          NH4_mineral=min(soil->litter.item[p].agsub.leaf.nitrogen,litter_flux*WC/WCH4/CN_fast);
          soil->litter.item[p].agsub.leaf.nitrogen-=NH4_mineral;
          decom_sum.nitrogen+=NH4_mineral;
          decom_fast.nitrogen+=NH4_mineral;
        }
        if(soil->wtable>50)
        {
          decom=max(0,soil->litter.item[p].agsub.leaf.carbon*decay_litter);
          C_max[0]=min(decom,soil->O2[0]*WC/WO2);
          decom=max(0,C_max[0]);
          soil->O2[0]-=decom*WO2/WC;
          if(soil->litter.item[p].agsub.leaf.carbon>epsilon)
            decay_litter=decom/soil->litter.item[p].agsub.leaf.carbon;
          soil->litter.item[p].agsub.leaf.carbon-=decom;
          decom_sum.carbon+=decom;
          decom_fast.carbon+=decom;
          decom=max(0,soil->litter.item[p].agsub.leaf.nitrogen*decay_litter);
          soil->litter.item[p].agsub.leaf.nitrogen-=decom;
          decom_sum.nitrogen+=decom;
          decom_fast.nitrogen+=decom;
        }
        /* agsub wood */
#ifdef LINEAR_DECAY
        decay_litter=soil->litter.item[p].pft->k_litter10.wood/timesteps*response_agsub_wood;
#else
        decay_litter=(1.0-exp(-(soil->litter.item[p].pft->k_litter10.wood/timesteps*response_agsub_wood)));
#endif
        if(decay_litter>1) decay_litter=1;
        if(soil->wtable>50)
        {
          for(i=0;i<NFUELCLASS;i++)
          {
            decom=max(0,soil->litter.item[p].agsub.wood[i].carbon*decay_litter);
            C_max[0]=min(decom,soil->O2[0]*WC/WO2);
            decom=max(0,C_max[0]);
            soil->O2[0]-=decom*WO2/WC;
            if(soil->litter.item[p].agsub.wood[i].carbon>epsilon)
              decay_litter=decom/soil->litter.item[p].agsub.wood[i].carbon;
            soil->litter.item[p].agsub.wood[i].carbon-=decom;
            decom_sum.carbon+=decom;
            decom_slow.carbon+=decom;
            decom=max(0,soil->litter.item[p].agsub.wood[i].nitrogen*decay_litter);
            soil->litter.item[p].agsub.wood[i].nitrogen-=decom;
            decom_sum.nitrogen+=decom;
            decom_slow.nitrogen+=decom;
          } /* of  for(i=0;i<NFUELCLASS;i++) */
        }
      } /* gtemp */

      if(soil->litter.item[p].bg.carbon>epsilon)
      {
        forrootsoillayer(l)
        {
          if(gtemp_soil[l]>0)
          {
#ifdef LINEAR_DECAY
            decay_litter=param.k_litter10/timesteps*response[l];
#else
            decay_litter=(1.0-exp(-(param.k_litter10/timesteps*response[l])));
#endif
            socfraction[l]=pow(10,soil->litter.item[p].pft->soc_k*logmidlayer[l])
                                        - (l>0 ? pow(10,soil->litter.item[p].pft->soc_k*logmidlayer[l-1]): 0);
            if(decay_litter>1) decay_litter=1;
            if(soil->litter.item[p].bg.nitrogen>epsilon && soil->litter.item[p].bg.carbon>epsilon)
              cn_bg=soil->litter.item[p].bg.carbon/soil->litter.item[p].bg.nitrogen;
            else
              cn_bg=soil->par->cn_ratio;
            if(soil->wtable>midlayer[l])
            {
              decom=max(0,soil->litter.item[p].bg.carbon*socfraction[l]*decay_litter);
              C_max[l]=min(decom,soil->O2[l]*WC/WO2);
              decom=max(0,C_max[l]);
              soil->litter.item[p].bg.carbon-=decom;
              soil->O2[l]-=decom*WO2/WC;
              decom_sum.carbon+=decom;
              decom_fast.carbon+=decom;
              decom=min(soil->litter.item[p].bg.nitrogen,decom/cn_bg);
              soil->litter.item[p].bg.nitrogen-=decom;
              decom_sum.nitrogen+=decom;
              decom_fast.nitrogen+=decom;
            }
            if (soil->wtable<WTABTHRES && (soil->freeze_depth[l]+epsilon)<soildepth[l])
            {
              V = getV(soil,l);  /*soil air content (m3 air/m3 soil)*/
              if (V<=epsilon)
                V=epsilon+epsilon;
              //epsilon_O2=getepsilon_O2(V,soil_moist[l],soil->wsat[l]);
              litter_flux=soil->litter.item[p].bg.carbon*param.k_litter10/k_red_litter/timesteps*socfraction[l]*gtemp_soil[l]*exp(-(soil->O2[l]*oxid_frac/ soil->wsat[l]/soildepth[l]*1000)/O2star);
              soil->litter.item[p].bg.carbon-=litter_flux*WC/WCH4;
              soil->CH4[l]+=litter_flux;
              getoutput(&stand->cell->output,METHANOGENESIS,config) += litter_flux*stand->frac;
              NH4_mineral=min(soil->litter.item[p].bg.nitrogen,litter_flux*WC/WCH4/cn_bg);
              NH4_mineral=max(0,NH4_mineral);
              soil->litter.item[p].bg.nitrogen-=NH4_mineral;
              soil->NH4[l]+=NH4_mineral;
              getoutput(&stand->cell->output,N_MINERALIZATION,config)+=NH4_mineral*stand->frac;
              if(isagriculture(stand->type->landusetype))
                getoutput(&stand->cell->output,NMINERALIZATION_AGR,config)+=NH4_mineral*stand->frac;
            }
          }/*end gtemp > 0 */
        } /*forrootsoillayer */
      }

      decom_litter.carbon+=decom_sum.carbon;
      decom_litter.nitrogen+=decom_sum.nitrogen;

      soil->fastfrac=param.fastfrac;             // Could be used for calculating internally
      soil->decomp_litter_pft[soil->litter.item[p].pft->id].carbon+=(1-param.atmfrac)*decom_sum.carbon;
      soil->decomp_litter_pft[soil->litter.item[p].pft->id].nitrogen+=(1-param.atmfrac)*decom_sum.nitrogen;

      forrootsoillayer(l)
      {
        soil->pool[l].fast.carbon+=param.fastfrac*(1-param.atmfrac)*decom_sum.carbon*soil->c_shift[l][soil->litter.item[p].pft->id].fast;
        soil->pool[l].slow.carbon+=(1-param.fastfrac)*(1-param.atmfrac)*decom_sum.carbon*soil->c_shift[l][soil->litter.item[p].pft->id].slow;
        if(decom_sum.carbon>0 && (stand->type->landusetype==NATURAL || stand->type->landusetype==WETLAND))
        {
          getoutputindex(&stand->cell->output,CSHIFT_FAST_NV,l,config)+=soil->fastfrac*(1-param.atmfrac)*decom_sum.carbon*soil->c_shift[l][soil->litter.item[p].pft->id].fast;
          getoutputindex(&stand->cell->output,CSHIFT_SLOW_NV,l,config)+=(1-soil->fastfrac)*(1-param.atmfrac)*decom_sum.carbon*soil->c_shift[l][soil->litter.item[p].pft->id].slow;
        }
        soil->pool[l].slow.nitrogen+=(1-soil->fastfrac)*(1-param.atmfrac)*decom_sum.nitrogen*soil->c_shift[l][soil->litter.item[p].pft->id].slow;
        soil->pool[l].fast.nitrogen+=soil->fastfrac*(1-param.atmfrac)*decom_sum.nitrogen*soil->c_shift[l][soil->litter.item[p].pft->id].fast;
        /* NO3 and N2O from mineralization of organic matter */
        F_Nmineral=max(0,decom_sum.nitrogen*param.atmfrac*(soil->fastfrac*soil->c_shift[l][soil->litter.item[p].pft->id].fast+(1-soil->fastfrac)*soil->c_shift[l][soil->litter.item[p].pft->id].slow));
        F_Nmineral_all+=F_Nmineral;
        soil->NH4[l]+=F_Nmineral;
#ifdef SAFE
        if(soil->NH4[l]<-epsilon)
          fail(NEGATIVE_SOIL_NH4_ERR,TRUE,TRUE,"2 Negative soil NH4=%g in layer %d in cell (%s) at mineralization",soil->NH4[l],l,sprintcoord(line,&stand->cell->coord));
        if(soil->pool[l].fast.carbon<-epsilon || soil->pool[l].slow.carbon<-epsilon)
        {
          fail(NEGATIV_SOIL_CARBON,TRUE,TRUE,"negative soil carbon: Lat: %g Lon: %g fast.carbon[%d]: %g slow.carbon: %g decom_sum.carbon: %g decay_litter: %g",
              stand->cell->coord.lat,stand->cell->coord.lon,l,soil->pool[l].fast.carbon,soil->pool[l].slow.carbon,decom_sum.carbon,decay_litter);
        }
#endif

        getoutput(&stand->cell->output,N_MINERALIZATION,config)+=F_Nmineral*stand->frac;
        if(isagriculture(stand->type->landusetype))
          getoutput(&stand->cell->output,NMINERALIZATION_AGR,config)+=F_Nmineral*stand->frac;

        N_sum=soil->NH4[l]+soil->NO3[l];
        if(N_sum>0) /* immobilization of N */
        {
          n_immo=param.fastfrac*(1-param.atmfrac)*(decom_sum.carbon/soil->par->cn_ratio-decom_sum.nitrogen)*soil->c_shift[l][soil->litter.item[p].pft->id].fast*N_sum/soildepth[l]*1e3/(k_N+N_sum/soildepth[l]*1e3);
          if(n_immo >0)
          {
            if(n_immo>N_sum)
              n_immo=N_sum;
            soil->pool[l].fast.nitrogen+=n_immo;
            soil->decomp_litter_pft[soil->litter.item[p].pft->id].nitrogen+=n_immo;
            getoutput(&stand->cell->output,N_IMMO,config)+=n_immo*stand->frac;
            if(isagriculture(stand->type->landusetype))
              getoutput(&stand->cell->output,NIMMOBILIZATION_AGR,config)+=n_immo*stand->frac;
            soil->NH4[l]-=n_immo*soil->NH4[l]/N_sum;
            soil->NO3[l]-=n_immo*soil->NO3[l]/N_sum;
#ifdef SAFE
            if(soil->NO3[l]<0)
            {
              soil->pool[l].fast.nitrogen+=soil->NO3[l];
              soil->NO3[l]=0;
            }
            if(soil->NH4[l]<0)
            {
              soil->pool[l].fast.nitrogen+=soil->NH4[l];
              soil->NH4[l]=0;
            }
#endif
          }
        }
        else
          n_immo=0;
        N_sum=soil->NH4[l]+soil->NO3[l];
        if(N_sum>0)
        {
          n_immo=(1-param.fastfrac)*(1-param.atmfrac)*(decom_sum.carbon/soil->par->cn_ratio-decom_sum.nitrogen)*soil->c_shift[l][soil->litter.item[p].pft->id].slow*N_sum/soildepth[l]*1e3/(k_N+N_sum/soildepth[l]*1e3);
          if(n_immo >0)
          {
            if(n_immo>N_sum)
              n_immo=N_sum;
            soil->pool[l].slow.nitrogen+=n_immo;
            soil->decomp_litter_pft[soil->litter.item[p].pft->id].nitrogen+=n_immo;
            getoutput(&stand->cell->output,N_IMMO,config)+=n_immo*stand->frac;
            if(isagriculture(stand->type->landusetype))
              getoutput(&stand->cell->output,NIMMOBILIZATION_AGR,config)+=n_immo*stand->frac;
            soil->NH4[l]-=n_immo*soil->NH4[l]/N_sum;
            soil->NO3[l]-=n_immo*soil->NO3[l]/N_sum;
#ifdef SAFE
            if(soil->NO3[l]<0)
            {
              soil->pool[l].slow.nitrogen+=soil->NO3[l];
              soil->NO3[l]=0;
            }
            if(soil->NH4[l]<0)
            {
              soil->pool[l].slow.nitrogen+=soil->NH4[l];
              soil->NH4[l]=0;
            }
            if(soil->NO3[l]<-epsilon)
              fail(NEGATIVE_SOIL_NO3_ERR,TRUE,TRUE,"Negative soil NO3=%g in layer %d in cell (%s) at immobilization in littersom()",soil->NO3[l],l,sprintcoord(line,&stand->cell->coord));
            if(soil->NH4[l]<-epsilon)
              fail(NEGATIVE_SOIL_NH4_ERR,TRUE,TRUE,"Negative soil NH4=%g in layer %d in cell (%s) at immobilization in littersom()",soil->NH4[l],l,sprintcoord(line,&stand->cell->coord));
#endif
          }
        }
        else
          n_immo=0;
      } /* of forrootlayer */
    }   /*end soil->litter.n*/

    if(soil->maxthaw_depth>0) gasdiffusion(&stand->soil,airtemp,pch4,&CH4_em,runoff,&CH4_sink,timesteps);
    *ch4_sink+=CH4_sink;
    *methaneflux_litter+=CH4_em;
  } /*end of timesteps*/

  /*sum for equilsom-routine*/
  soil->decomp_litter_mean.carbon+=decom_litter.carbon;
  soil->decomp_litter_mean.nitrogen+=decom_litter.nitrogen;

  /* NO3 and N2O from nitrification */

//  forrootsoillayer(l)
//  {
//    fac_wfps = f_wfps(soil,l);
//    fac_temp = f_temp(soil->temp[l]);
//    F_NO3=min(soil->O2[l]*WN/(WO2*2),param.k_max*soil->NH4[l]*fac_temp*fac_wfps*f_ph(stand->cell->soilph));
//    F_NO3=max(0,F_NO3);
//    if(F_NO3>soil->NH4[l])
//      F_NO3=soil->NH4[l];
//    F_N2O=param.k_2*F_NO3;
//    soil->NO3[l]+=F_NO3*(1-param.k_2);
//#ifdef SAFE
//    if(soil->NO3[l]<-epsilon)
//      fail(NEGATIVE_SOIL_NO3_ERR,FALSE,TRUE,"littersom: Negative soil NO3=%g in layer %d in cell (%s)",
//          soil->NO3[l],l,sprintcoord(line,&stand->cell->coord));
//#endif
//
//    soil->NH4[l]-=F_NO3;
//    soil->O2[l]-=F_NO3*2*WO2/WN;    //NH4+ + 2O2 -> NO3- + 2H+ + H2O  N=14g/mol
//#ifdef SAFE
//    if(soil->NH4[l]<-epsilon)
//      fail(NEGATIVE_SOIL_NH4_ERR,FALSE,TRUE,"Negative soil NH4=%g in layer %d in cell (%s)",
//          soil->NH4[l],l,sprintcoord(line,&stand->cell->coord));
//#endif
//    flux.nitrogen += F_N2O;
//    /* F_N2O is given back for output */
//    if(stand->type->landusetype==AGRICULTURE)
//    {
//      foreachpft(pft,p,&stand->pftlist)
//      {
//        crop=pft->data;
//        if(crop->sh!=NULL)
//        {
//          crop->sh->n2o_nitsum+=F_N2O;
//          crop->sh->c_emissum+=decom_litter.carbon*param.atmfrac+soil_cflux;
//        }
//        else
//        {
//          data=stand->data;
//          getoutputindex(&stand->cell->output,CFT_N2O_NIT,pft->par->id-npft+data->irrigation*ncft,config)+=F_N2O;
//          getoutputindex(&stand->cell->output,CFT_C_EMIS,pft->par->id-npft+data->irrigation*ncft,config)+=decom_litter.carbon *param.atmfrac+soil_cflux;
//        }
//      }
//    }
//  }
//

//saturation soil carbon calculation (I WOULD LIKE TO KEEP IT FOR NOW)
#ifdef CALC_EFF_CARBON
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
  {
    if(C_eff[1]>C_eff[0])
    {
      K_v[0][1]=-1;
    }
    else
    {
      K_v[0][1]=0;
    }
  }
  for(l=1;l<LASTLAYER;l++)
  {
    if(layerbound[l]<=soil->maxthaw_depth)
    {
      dKOVCON=KOVCON;
    }
    else
    {
      dKOVCON=0.0;
    }
    K_v[l][0]=dKOVCON*(K_vdiff[l][0]*C_eff[l-1]+ K_vdiff[l][1]*C_eff[l]+K_vdiff[l][2]*C_eff[l+1]);
    if(K_v[l][0]>0)
    {
      if(C_eff[l+1]>C_eff[l])
        K_v[l][1]=-1;
    }
    else if (C_eff[l-1]>C_eff[l])
    {
      K_v[l][1]=1;
    }
    else
    {
      K_v[l][1]=0;
    }
  }

  K_v[LASTLAYER-1][0]=dKOVCON*(K_vdiff[LASTLAYER-1][0]*C_eff[LASTLAYER-2]+ K_vdiff[LASTLAYER-1][1]*C_eff[LASTLAYER-1]+K_vdiff[LASTLAYER-1][2]*C_eff[LASTLAYER-1]);

  if(K_v[LASTLAYER-1][0]> 0.)
  {
    K_v[LASTLAYER-1][0]=1;
  }
  else
  {
    K_v[LASTLAYER-1][0]=0;
  }

  for(l=(LASTLAYER-1);l==0;l--)
  {
    if(K_v[l][1] == 1)
    {
      if((K_v[l][0]*(soildepth[l]/soildepth[l-1])) > (0.5*C_tot[l-1]))
        K_v[l][0]=(0.5*C_tot[l-1])*(soildepth[l-1]/soildepth[l]);

      C_tot[l-1]-=(K_v[l][0]*(soildepth[l]/soildepth[l-1]));
    }
    else if (K_v[l][1] == -1)
     {
      if((K_v[l][0]*(soildepth[l]/soildepth[l+1])) > (0.5*C_tot[l+1]))
        K_v[l][0]=(0.5*C_tot[l+1])*(soildepth[l+1]/soildepth[l]);
    }
  }
#ifdef DEBUG
  printf("before: TOTalCarbon: %.4f\n", soilstocks(soil).carbon);
#endif

  forrootsoillayer(l)
    if(l>0 && l<(NSOILLAYER-1))
    {
      if(soilall[l].carbon>epsilon && soilall[l+1].carbon>epsilon && soilall[l-1].carbon>epsilon)
      {
        if(K_v[l][1]==1)
        {
          soil->pool[l-1].fast.carbon-=(K_v[l][0]*(soildepth[l]/soildepth[l-1])*(soilold[l-1].fast.carbon/soilall[l-1].carbon));
          soil->pool[l].fast.carbon+=(K_v[l][0]*(soildepth[l]/soildepth[l-1])*(soilold[l-1].fast.carbon/soilall[l-1].carbon));
          soil->pool[l-1].slow.carbon-=(K_v[l][0]*(soildepth[l]/soildepth[l-1])*((soilold[l-1].slow.carbon/soilall[l-1].carbon)));
          soil->pool[l].slow.carbon+=(K_v[l][0]*(soildepth[l]/soildepth[l-1])*(soilold[l-1].slow.carbon/soilall[l-1].carbon));
        }
        else if (K_v[l][1]==-1)
        {
          soil->pool[l+1].fast.carbon-=(K_v[l][0]*(soildepth[l]/soildepth[l+1])*(soilold[l+1].fast.carbon/soilall[l+1].carbon));
          soil->pool[l].fast.carbon+=(K_v[l][0]*(soildepth[l]/soildepth[l+1])*(soilold[l+1].fast.carbon/soilall[l+1].carbon));
          soil->pool[l+1].slow.carbon-=(K_v[l][0]*(soildepth[l]/soildepth[l+1])*(soilold[l+1].slow.carbon/soilall[l+1].carbon));
          soil->pool[l].slow.carbon+=(K_v[l][0]*(soildepth[l]/soildepth[l+1])*(soilold[l+1].slow.carbon/soilall[l+1].carbon));
        }
      }
    }
#ifdef DEBUG
  printf("after: TOTalCarbon: %.4f\n", soilstocks(soil).carbon);
#endif
#endif

#ifdef MICRO_HEATING
  soil->litter.decomC=decom_litter.carbon*param.atmfrac; /*only for mircobiological heating*/
#endif
  soil->count++;
  getoutput(&stand->cell->output,RH_LITTER,config)+=decom_litter.carbon*param.atmfrac*stand->frac;
  if(isagriculture(stand->type->landusetype))
    getoutput(&stand->cell->output,RH_AGR,config) += (decom_litter.carbon*param.atmfrac+soil_cflux)*stand->frac;
  flux.carbon=decom_litter.carbon*param.atmfrac+soil_cflux;
  stand->cell->balance.aCH4_oxid+=oxidation_stand;
  getoutput(&stand->cell->output,CH4_OXIDATION,config) += oxidation_stand*stand->frac;

#ifdef CHECK_BALANCE
  end = soilstocks(soil);
  end.carbon+=soilmethane(soil)*WC/WCH4;
  water_after=soilwater(soil);
  if (fabs(start.carbon - end.carbon - (flux.carbon + *methaneflux_litter*WC/WCH4 + *ch4_sink*WC/WCH4))>0.0001)
  {
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid carbon balance in %s at the end: iswetland: %d type: %s %.8f start: %.8f  end: %.8f decomCO2: %.8f methane_em: %.8f"
        " oxidation: %.8f decom_litter: %.8f soil_cflux: %.8f stand->frac: %g decay_litter: %g ch4_sink: %g \n",
         __FUNCTION__,soil->iswetland,stand->type->name,start.carbon - end.carbon - (flux.carbon + *methaneflux_litter*WC/WCH4 + *ch4_sink*WC/WCH4), start.carbon, end.carbon, flux.carbon, *methaneflux_litter*WC/WCH4,
         oxidation_stand,decom_litter.carbon*param.atmfrac,soil_cflux,stand->frac,decay_litter,*ch4_sink*WC/WCH4);
    //fprintsoil(stderr,&stand->soil,config->pftpar,npft+ncft);
    //forrootsoillayer(l) fprintf(stderr,"socfracion: %g ",socfraction[l]);
    fprintf(stderr,"\n");
  }
  if (fabs(end.nitrogen-start.nitrogen+flux.nitrogen)>0.0001)
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid nitrogen balance in %s at the end: iswetland: %d %.8f start: %.8f  end: %.8f flux.nitrogen: %g F_Nmineral: %g  decom_sum.nitrogen: %g\n",
         __FUNCTION__,soil->iswetland,end.nitrogen-start.nitrogen+flux.nitrogen,
         start.nitrogen, end.nitrogen, flux.nitrogen,F_Nmineral_all,decom_sum.nitrogen);
  balanceW=water_after-water_before-*MT_water+*runoff;
  if(fabs(balanceW)>epsilon)
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,FALSE, "Invalid water balance in %s at the end: balanceW: %g  water_after: %.5f water_before: %.5f balance_stocks: %.5f w1: %g w2: %g stand->frac: %g\n"
         "MT_water_local: %g runoff_local= %g\n",__FUNCTION__,balanceW,water_after,water_before,water_after-water_before,soil->w[0],soil->w[1],*MT_water,*runoff,stand->frac);
#endif
  return flux;
} /* of 'littersom' */

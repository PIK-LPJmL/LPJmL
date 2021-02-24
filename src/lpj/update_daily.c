/**************************************************************************************/
/**                                                                                \n**/
/**                u  p  d  a  t  e  _  d  a  i  l  y  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function of daily update of individual grid cell                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define length 1.0 /* characteristic length (m) */
#ifdef IMAGE
#define GWCOEFF 100 /**< groundwater outflow coefficient (average amount of release time in reservoir) */
#endif
#define BIOTURBRATE 0.001897 /* daily rate for 50% annual bioturbation rate [-]*/
#define kkk (1/365.)
#define LEAF 0
#define WOOD 1

void update_daily(Cell *cell,            /**< cell pointer           */
                  Real co2,              /**< atmospheric CO2 (ppmv) */
                  Real ch4,              /* atmospheric CH4 (ppmv) */
                  Real popdensity,       /**< population density (capita/km2) */
                  Dailyclimate climate,  /**< Daily climate values */
                  int day,               /**< day (1..365)           */
                  int npft,              /**< number of natural PFTs */
                  int ncft,              /**< number of crop PFTs   */
                  int year,              /**< simulation year */
                  int month,             /**< month (0..11) */
                  Bool intercrop,        /**< enable intercropping */
                  const Config *config   /**< LPJmL configuration */
                 )
{
  int s,p;
  Pft *pft;
  Real melt=0,eeq,par,daylength,beta,gw_outflux,CH4_em;
  Real runoff,snowrunoff,epsilon_gas,soilmoist,V;
#ifdef IMAGE
  Real fout_gw; // local variable for groundwater outflow (baseflow)
#endif
  Real gtemp_air;  /* value of air temperature response function */
  Real gtemp_soil[NSOILLAYER]; /* value of soil temperature response function */
  Real temp_bs;    /* temperature beneath snow */
  Real prec_energy; /* energy from temperature difference between rain and soil [J/m2]*/
  Stocks flux_estab={0,0};
  Real evap=0;
  Real MT_water=0;
  Stocks hetres={0,0};
  Real avgprec;
  Stand *stand;
  Real ebul;
#ifdef CHECK_BALANCE
  Real ende, start;
  ende = start=0;
#endif
  ebul = 0;
  Real bnf;
  Real nh3;
  int index,l,i;
  Real rootdepth=0.0;
  Livefuel livefuel={0,0,0,0,0};
  const Real prec_save=climate.prec;
  Real agrfrac,fpc_total_stand;
  Real litsum_old_nv[2]={0,0},litsum_new_nv[2]={0,0};
  Real litsum_old_agr[2]={0,0},litsum_new_agr[2]={0,0};


  forrootmoist(l)
    rootdepth+=soildepth[l];

  updategdd(cell->gdd,config->pftpar,npft,climate.temp);
  cell->balance.aprec+=climate.prec;
  gtemp_air=temp_response(climate.temp,cell->climbuf.atemp_mean);
  daily_climbuf(&cell->climbuf,climate.temp,climate.prec);
  avgprec=getavgprec(&cell->climbuf);
  getoutput(&cell->output,SNOWF,config)+=climate.temp<tsnow ? climate.prec : 0;
  getoutput(&cell->output,RAIN,config)+=climate.temp<tsnow ? 0 : climate.prec;

  if(config->withlanduse) /* landuse enabled? */
    flux_estab=sowing(cell,climate.prec,day,year,npft,ncft,config); 
  cell->discharge.drunoff=0.0;

  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
    update_nesterov(cell,&climate);

  agrfrac=0;
  foreachstand(stand,s,cell->standlist)
  {
    if(isagriculture(stand->type->landusetype))
      agrfrac+=stand->frac;
    for(l=0;l<stand->soil.litter.n;l++)
    {
      stand->soil.litter.item[l].agsub.leaf.carbon += stand->soil.litter.item[l].ag.leaf.carbon*param.bioturbate;
      stand->soil.litter.item[l].ag.leaf.carbon *= (1 - param.bioturbate);
      stand->soil.litter.item[l].agsub.leaf.nitrogen += stand->soil.litter.item[l].ag.leaf.nitrogen*param.bioturbate;
      stand->soil.litter.item[l].ag.leaf.nitrogen *= (1 - param.bioturbate);
    }

    if(stand->type->landusetype==NATURAL && config->black_fallow && (day==152 || day==335))
    {
      if(config->prescribe_residues && param.residue_rate>0 && param.residue_pool<=0)
      {
        index=findlitter(&stand->soil.litter,config->pftpar+config->pft_residue);
        if(index==NOT_FOUND)
          index=addlitter(&stand->soil.litter,config->pftpar+config->pft_residue)-1;
        stand->soil.litter.item[index].ag.leaf.carbon+=param.residue_rate*(1-param.residue_fbg)/2;
        stand->soil.litter.item[index].ag.leaf.nitrogen+=param.residue_rate*(1-param.residue_fbg)/param.residue_cn/2;
        stand->soil.litter.item[index].bg.carbon+=param.residue_rate*param.residue_fbg*0.5;
        stand->soil.litter.item[index].bg.nitrogen+=param.residue_rate*param.residue_fbg/param.residue_cn/2;
        getoutput(&cell->output,FLUX_ESTABC,config)+=param.residue_rate*stand->frac*0.5;
        cell->balance.flux_estab.carbon+=param.residue_rate*stand->frac*0.5;
        getoutput(&cell->output,FLUX_ESTABN,config)+=param.residue_rate/param.residue_cn*0.5*stand->frac;
        cell->balance.flux_estab.nitrogen+=param.residue_rate/param.residue_cn*0.5*stand->frac;
        updatelitterproperties(stand,stand->frac);
      }
      if(config->fix_fertilization)
      {
        stand->soil.NO3[0]+=param.fertilizer_rate*0.25;
        stand->soil.NH4[0]+=param.fertilizer_rate*0.25;
        cell->balance.n_influx+=param.fertilizer_rate*0.5*stand->frac;
      }
      if(config->till_fallow)
      {
        tillage(&stand->soil,param.residue_frac);
        pedotransfer(stand,NULL,NULL,stand->frac);
        updatelitterproperties(stand,stand->frac);
      }
    }
    beta=albedo_stand(stand);
    radiation(&daylength,&par,&eeq,cell->coord.lat,day,&climate,beta,config->with_radiation);
    getoutput(&cell->output,PET,config)+=eeq*PRIESTLEY_TAYLOR*stand->frac;
    cell->output.mpet+=eeq*PRIESTLEY_TAYLOR*stand->frac;
    getoutput(&cell->output,ALBEDO,config) += beta * stand->frac;

    if((config->fire==SPITFIRE  || config->fire==SPITFIRE_TMAX)&& cell->afire_frac<1)
      dailyfire_stand(stand,&livefuel,popdensity,avgprec,&climate,config);
    if(config->permafrost)
    {
      snowrunoff=snow(&stand->soil,&climate.prec,&melt,
                      climate.temp,&temp_bs,&evap)*stand->frac;
      cell->discharge.drunoff+=snowrunoff;
      getoutput(&cell->output,EVAP,config)+=evap*stand->frac; /* evap from snow runoff*/
      cell->balance.aevap+=evap*stand->frac; /* evap from snow runoff*/
#if defined IMAGE && defined COUPLED
  if(cell->ml.image_data!=NULL)
    cell->ml.image_data->mevapotr[month] += evap*stand->frac;
#endif

      if(config->crop_index==ALLSTAND)
        getoutput(&cell->output,D_EVAP,config)+=evap*stand->frac;
      prec_energy = ((climate.temp-stand->soil.temp[TOPLAYER])*climate.prec*1e-3
                    +melt*1e-3*(T_zero-stand->soil.temp[TOPLAYER]))*c_water;
      stand->soil.perc_energy[TOPLAYER]=prec_energy;
#ifdef MICRO_HEATING
      /*THIS IS DEDICATED TO MICROBIOLOGICAL HEATING*/
      foreachsoillayer(l)
        stand->soil.micro_heating[l]=m_heat*stand->soil.decomC[l];
      stand->soil.micro_heating[0]+=m_heat*stand->soil.litter.decomC;
#endif
      soiltemp(&stand->soil,temp_bs,config->permafrost);
    }
    else
    {
      stand->soil.temp[0]=soiltemp_lag(&stand->soil,&cell->climbuf);
      for(l=1;l<NSOILLAYER;l++)
        stand->soil.temp[l]=stand->soil.temp[0];
      snowrunoff=snow_old(&stand->soil.snowpack,&climate.prec,&melt,climate.temp)*stand->frac;
      cell->discharge.drunoff+=snowrunoff;
    }

    foreachsoillayer(l)
    {
      gtemp_soil[l]=temp_response(stand->soil.temp[l],stand->soil.amean_temp[l]);
      stand->soil.amean_temp[l]=(1-1./365)*stand->soil.amean_temp[l]+1./365*stand->soil.temp[l];
    }
    stand->soil.amean_temp[SNOWLAYER]=(1-1./365)*stand->soil.amean_temp[SNOWLAYER]+1./365*stand->soil.temp[SNOWLAYER];
    getoutput(&cell->output,SOILTEMP1,config)+=stand->soil.temp[0]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(&cell->output,SOILTEMP2,config)+=stand->soil.temp[1]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(&cell->output,SOILTEMP3,config)+=stand->soil.temp[2]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(&cell->output,SOILTEMP4,config)+=stand->soil.temp[3]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(&cell->output,SOILTEMP5,config)+=stand->soil.temp[4]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(&cell->output,SOILTEMP6,config)+=stand->soil.temp[5]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    foreachsoillayer(l)
      getoutputindex(&cell->output,SOILTEMP,l,config)+=stand->soil.temp[l]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    plant_gas_transport(stand,climate.temp,ch4,config);
    CH4_em=runoff=0;
    gasdiffusion(&stand->soil,climate.temp,ch4,&CH4_em,&runoff);
    cell->discharge.drunoff += runoff*stand->frac;
    if (CH4_em>0)
      getoutput(&cell->output,CH4_EMISSIONS,config) += CH4_em*stand->frac;
    else
      getoutput(&cell->output,CH4_SINK,config) += CH4_em*stand->frac;
    CH4_em = runoff = MT_water = 0;
    fpc_total_stand = 0;
    foreachpft(pft, p, &stand->pftlist)
      fpc_total_stand += pft->fpc;
#ifdef CHECK_BALANCE
    start = standstocks(stand).carbon + soilmethane(&stand->soil);
#endif
    ebul = ebullition(&stand->soil, fpc_total_stand);
    //cell->output.mCH4_em+=ebullition(&stand->soil,fpc_total_stand)*stand->frac;
    getoutput(&cell->output,CH4_EMISSIONS,config) += ebul*stand->frac;
    getoutput(&cell->output,CH4_EBULLITION,config) += ebul*stand->frac;
#ifdef CHECK_BALANCE
    ende = standstocks(stand).carbon + soilmethane(&stand->soil);
    if (fabs(start - ende - ebul)>epsilon) fprintf(stdout, "C-ERROR: %g start:%g  ende:%g daily: %g\n", start - ende - ebul, start, ende, ebul);
#endif

    cell->discharge.drunoff += runoff*stand->frac;
    runoff = 0;

    /* update soil and litter properties to account for all changes since last call of littersom */
    pedotransfer(stand,NULL,NULL,stand->frac);
    updatelitterproperties(stand,stand->frac);

    if(stand->type->landusetype==NATURAL)
      for(l=0;l<stand->soil.litter.n;l++)
      {
        litsum_old_nv[LEAF]+=stand->soil.litter.item[l].ag.leaf.carbon+stand->soil.litter.item[l].agsub.leaf.carbon+stand->soil.litter.item[l].bg.carbon;
        for(i=0;i<NFUELCLASS;i++)
          litsum_old_nv[WOOD]+=stand->soil.litter.item[l].ag.wood[i].carbon+stand->soil.litter.item[l].agsub.wood[i].carbon;
      }
    if(isagriculture(stand->type->landusetype))
      for(l=0;l<stand->soil.litter.n;l++)
      {
        litsum_old_agr[LEAF]+=stand->soil.litter.item[l].ag.leaf.carbon+stand->soil.litter.item[l].agsub.leaf.carbon+stand->soil.litter.item[l].bg.carbon;
        for(i=0;i<NFUELCLASS;i++)
          litsum_old_agr[WOOD]+=stand->soil.litter.item[l].ag.wood[i].carbon+stand->soil.litter.item[l].agsub.wood[i].carbon;
      }

    hetres=littersom(stand,gtemp_soil,agrfrac,&CH4_em,climate.temp,ch4,&runoff,&MT_water,npft,ncft,config);
    cell->balance.arh+=hetres.carbon*stand->frac;
    getoutput(&cell->output,RH,config)+=hetres.carbon*stand->frac;
    getoutput(&cell->output,N2O_DENIT,config)+=hetres.nitrogen*stand->frac;
    cell->balance.n_outflux+=hetres.nitrogen*stand->frac;

    if(stand->type->landusetype==NATURAL)
      for(l=0;l<stand->soil.litter.n;l++)
      {
        litsum_new_nv[LEAF]+=stand->soil.litter.item[l].ag.leaf.carbon+stand->soil.litter.item[l].agsub.leaf.carbon+stand->soil.litter.item[l].bg.carbon;
        for(i=0;i<NFUELCLASS;i++)
          litsum_new_nv[WOOD]+=stand->soil.litter.item[l].ag.wood[i].carbon+stand->soil.litter.item[l].agsub.wood[i].carbon;
      }
    if(isagriculture(stand->type->landusetype))
      for(l=0;l<stand->soil.litter.n;l++)
      {
        litsum_new_agr[LEAF]+=stand->soil.litter.item[l].ag.leaf.carbon+stand->soil.litter.item[l].agsub.leaf.carbon+stand->soil.litter.item[l].bg.carbon;
        for(i=0;i<NFUELCLASS;i++)
          litsum_new_agr[WOOD]+=stand->soil.litter.item[l].ag.wood[i].carbon+stand->soil.litter.item[l].agsub.wood[i].carbon;
      }

    if(stand->type->landusetype==NATURAL && config->black_fallow && config->prescribe_residues && param.residue_pool>0)
    {
      index=findlitter(&stand->soil.litter,config->pftpar+config->pft_residue);
      if(index==NOT_FOUND)
        index=addlitter(&stand->soil.litter,config->pftpar+config->pft_residue)-1;
      getoutput(&cell->output,FLUX_ESTABC,config)+=(param.residue_pool-stand->soil.litter.item[index].ag.leaf.carbon)*stand->frac;
      cell->balance.flux_estab.carbon+=(param.residue_pool-stand->soil.litter.item[index].ag.leaf.carbon)*stand->frac;
      stand->soil.litter.item[index].ag.leaf.carbon=param.residue_pool;
      getoutput(&cell->output,FLUX_ESTABN,config)+=(param.residue_pool/param.residue_cn-stand->soil.litter.item[index].ag.leaf.nitrogen)*stand->frac;
      cell->balance.flux_estab.nitrogen+=(param.residue_pool/param.residue_cn-stand->soil.litter.item[index].ag.leaf.nitrogen)*stand->frac;
      stand->soil.litter.item[index].ag.leaf.nitrogen=param.residue_pool/param.residue_cn;
    }

    /* update soil and litter properties to account for all changes from littersom */
    pedotransfer(stand,NULL,NULL,stand->frac);
    updatelitterproperties(stand,stand->frac);

    getoutput(&cell->output,CH4_EMISSIONS,config) += CH4_em*stand->frac;
    cell->balance.aMT_water += MT_water*stand->frac;
    /*monthly rh for agricutural stands*/
    if (isagriculture(stand->type->landusetype))
    {
      getoutput(&cell->output,RH_AGR,config)+=hetres.carbon*stand->frac/agrfrac;
      getoutput(&cell->output,N2O_NIT_AGR,config)+=hetres.nitrogen*stand->frac;
    }
    getoutput(&cell->output,N2O_NIT,config)+=hetres.nitrogen*stand->frac;
    cell->output.dcflux+=hetres.carbon*stand->frac;
#if defined IMAGE && defined COUPLED
    if (stand->type->landusetype == NATURAL)
    {
      cell->rh_nat += hetres.carbon*stand->frac;
    } /* if NATURAL */
    if (stand->type->landusetype == WOODPLANTATION)
    {
      cell->rh_wp += hetres.carbon*stand->frac;
    } /* if woodplantation */
#endif

    getoutput(&cell->output,SWE,config)+=stand->soil.snowpack*stand->frac;
    if(isdailyoutput_stand(config,stand))
    {
      if(config->crop_index==ALLSTAND)
      {
        getoutput(&cell->output,D_RH,config) += hetres.carbon*stand->frac;
        getoutput(&cell->output,D_SWE,config)+= stand->soil.snowpack*stand->frac;
      }
      else
      {
        getoutput(&cell->output,D_RH,config)  += hetres.carbon;
        getoutput(&cell->output,D_SWE,config) += stand->soil.snowpack;
      }
    }

    getoutput(&cell->output,SNOWRUNOFF,config)+=snowrunoff;
    getoutput(&cell->output,MELT,config)+=melt*stand->frac;

    if(config->fire==FIRE && climate.temp>0 && stand->soil.wtable>400)
      stand->fire_sum+=fire_sum(&stand->soil.litter,stand->soil.w[0]);

    if(config->with_nitrogen)
    {
      if(config->with_nitrogen==UNLIM_NITROGEN)
      {
        if(stand->soil.par->type==ROCK)
        {
          getoutput(&cell->output,LEACHING,config)+=2000*stand->frac;
          cell->balance.n_outflux+=2000*stand->frac;
          if (isagriculture(stand->type->landusetype))
            getoutput(&cell->output,NLEACHING_AGR,config)+=2000*stand->frac;
        }
        else
        {
          stand->soil.NH4[0]+=1000;
          stand->soil.NO3[0]+=1000;
        }
        cell->balance.n_influx+=2000*stand->frac;
        if (isagriculture(stand->type->landusetype))
          getoutput(&cell->output,NDEPO_AGR,config)+=2000*stand->frac;
      }
      else if(!config->no_ndeposition)
      {
        if(stand->soil.par->type==ROCK)
        {
          getoutput(&cell->output,LEACHING,config)+=(climate.nh4deposition+climate.no3deposition)*stand->frac;
          cell->balance.n_outflux+=(climate.nh4deposition+climate.no3deposition)*stand->frac;
          if (isagriculture(stand->type->landusetype))
            getoutput(&cell->output,NLEACHING_AGR,config)+=(climate.nh4deposition+climate.no3deposition)*stand->frac;
        }
        else
        {
          /*adding daily N deposition to upper soil layer*/
          stand->soil.NH4[0]+=climate.nh4deposition;
          stand->soil.NO3[0]+=climate.no3deposition;
        }
        cell->balance.n_influx+=(climate.nh4deposition+climate.no3deposition)*stand->frac;
        if (isagriculture(stand->type->landusetype))
          getoutput(&cell->output,NDEPO_AGR,config)+=(climate.nh4deposition+climate.no3deposition)*stand->frac;
      }
#ifdef DEBUG_N
      printf("BEFORE_STRESS[%s], day %d: ",stand->type->name,day);
      for(l=0;l<NSOILLAYER-1;l++)
        printf("%g ",stand->soil.NO3[l]);
      printf("\n");
#endif
#ifdef DEBUG_N
      printf("AFTER_STRESS: ");
      for(l=0;l<NSOILLAYER-1;l++)
        printf("%g ",stand->soil.NO3[l]);
      printf("\n");
#endif

    } /* of if(config->with_nitrogen) */

    if(config->with_nitrogen)
    {
      bnf=biologicalnfixation(stand);
      stand->soil.NH4[0]+=bnf;
      getoutput(&cell->output,BNF,config)+=bnf*stand->frac;
      cell->balance.n_influx+=bnf*stand->frac;
    }

    runoff=daily_stand(stand,co2,&climate,day,month,daylength,
                       gtemp_air,gtemp_soil[0],eeq,par,
                       melt,npft,ncft,year,intercrop,agrfrac,config);
    if(config->with_nitrogen)
    {
      denitrification(stand,npft,ncft,config);

      nh3=volatilization(stand->soil.NH4[0],climate.windspeed,climate.temp,
                         length,cell->soilph);
      if(nh3>stand->soil.NH4[0])
        nh3=stand->soil.NH4[0];
      stand->soil.NH4[0]-=nh3;
      getoutput(&cell->output,N_VOLATILIZATION,config)+=nh3*stand->frac;
      if (isagriculture(stand->type->landusetype))
        getoutput(&cell->output,NH3_AGR,config)+=nh3*stand->frac;
      cell->balance.n_outflux+=nh3*stand->frac;
    }

    cell->discharge.drunoff+=runoff*stand->frac;
    climate.prec=prec_save;
    foreachpft(pft, p, &stand->pftlist)
      getoutput(&cell->output,VEGC_AVG,config)+=vegc_sum(pft)*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(&cell->output,SWC1,config)+=(stand->soil.w[0]*stand->soil.whcs[0]+stand->soil.w_fw[0]+stand->soil.wpwps[0]+
              stand->soil.ice_depth[0]+stand->soil.ice_fw[0])/stand->soil.wsats[0]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(&cell->output,SWC2,config)+=(stand->soil.w[1]*stand->soil.whcs[1]+stand->soil.w_fw[1]+stand->soil.wpwps[1]+
              stand->soil.ice_depth[1]+stand->soil.ice_fw[1])/stand->soil.wsats[1]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(&cell->output,SWC3,config)+=(stand->soil.w[2]*stand->soil.whcs[2]+stand->soil.w_fw[2]+stand->soil.wpwps[1]+
              stand->soil.ice_depth[2]+stand->soil.ice_fw[2])/stand->soil.wsats[2]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(&cell->output,SWC4,config)+=(stand->soil.w[3]*stand->soil.whcs[3]+stand->soil.w_fw[3]+stand->soil.wpwps[1]+
              stand->soil.ice_depth[3]+stand->soil.ice_fw[3])/stand->soil.wsats[3]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    getoutput(&cell->output,SWC5,config)+=(stand->soil.w[4]*stand->soil.whcs[4]+stand->soil.w_fw[4]+stand->soil.wpwps[1]+
              stand->soil.ice_depth[4]+stand->soil.ice_fw[4])/stand->soil.wsats[4]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    foreachsoillayer(l)
      getoutputindex(&cell->output,SWC,l,config)+=(stand->soil.w[l]*stand->soil.whcs[l]+stand->soil.w_fw[l]+stand->soil.wpwps[l]+
                     stand->soil.ice_depth[l]+stand->soil.ice_fw[l])/stand->soil.wsats[l]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    forrootmoist(l)
      getoutput(&cell->output,ROOTMOIST,config)+=stand->soil.w[l]*stand->soil.whcs[l]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac)); /* absolute soil water content between wilting point and field capacity (mm) */
      /*cell->output.mrootmoist+=stand->soil.w[l]*soildepth[l]/rootdepth*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac)); previous implementation that doesn't make sense to me, because the sum of soildepth[l]/rootdepth over the first 3 layers equals 1 (JJ, June 25, 2020)*/
    //getoutput(&cell->output,SOILC1,config)+=(stand->soil.pool[l].slow.carbon+stand->soil.pool[l].fast.carbon)*stand->frac;
    foreachsoillayer(l) {
      V = (stand->soil.wsats[l] - (stand->soil.w[l] * stand->soil.whcs[l] + stand->soil.ice_depth[l] + stand->soil.ice_fw[l] + stand->soil.wpwps[l] + stand->soil.w_fw[l])) / soildepth[l];  /*soil air content (m3 air/m3 soil)*/
      soilmoist = (stand->soil.w[l] * stand->soil.whcs[l] + (stand->soil.wpwps[l + 1] * (1 - stand->soil.ice_pwp[l + 1])) + stand->soil.w_fw[l]) / stand->soil.wsats[l];
      epsilon_gas = max(0.1, V + soilmoist*stand->soil.wsat[l]*BO2);
      getoutput(&cell->output,MEANSOILO2,config) += stand->soil.O2[l] / soildepth[l] / epsilon_gas * 1000 / LASTLAYER*stand->frac / NDAYYEAR;
      epsilon_gas = max(0.1, V + soilmoist*stand->soil.wsat[l]*BCH4);
      getoutput(&cell->output,MEANSOILCH4,config) += stand->soil.CH4[l] / soildepth[l] / epsilon_gas * 1000 / LASTLAYER*stand->frac / NDAYYEAR;
#ifdef DEBUG
      if (p_s / R_gas / (climate.temp + 273.15)*ch4*1e-6*WCH4 * 100000<stand->soil.CH4[l] / soildepth[l] / epsilon_gas * 1000)
      {
        fprintf(stdout, "Cell lat %.2f lon %.2f CH4[%d]:%.8f dCH4:%.8f \n", cell->coord.lat, cell->coord.lon, l, stand->soil.CH4[l]);
        fprintf(stdout, "CH4[%d]:%.8f\n", l, stand->soil.CH4[l] / soildepth[l] / epsilon_gas * 1000);
        fprintf(stdout, "epsilon:%.8f day=%d\n\n\n", epsilon_gas, day);
      }
#endif
    }
  } /* of foreachstand */

  getoutput(&cell->output,CELLFRAC_AGR,config)+=agrfrac;
  getoutput(&cell->output,DECAY_LEAF_NV,config)*=litsum_old_nv[LEAF]>0 ? litsum_new_nv[LEAF]/litsum_old_nv[LEAF] : 1;
  getoutput(&cell->output,DECAY_WOOD_NV,config)*=litsum_old_nv[WOOD]>0 ? litsum_new_nv[WOOD]/litsum_old_nv[WOOD] : 1;
  getoutput(&cell->output,DECAY_LEAF_AGR,config)*=litsum_old_agr[LEAF]>0 ? litsum_new_agr[LEAF]/litsum_old_agr[LEAF] : 1;
  getoutput(&cell->output,DECAY_WOOD_AGR,config)*=litsum_old_agr[WOOD]>0 ? litsum_new_agr[WOOD]/litsum_old_agr[WOOD] : 1;

#ifdef COUPLING_WITH_FMS
  if (cell->lakefrac > 0)
  {
    laketemp(cell, &climate);
    cell->output.mlaketemp+=cell->laketemp;
  }
  else
    getoutput(&cell->output,LAKETEMP,config)=config->missing_value;
#endif

  gw_outflux = cell->ground_st*cell->kbf;
  cell->ground_st -= gw_outflux;
  //cell->discharge.drunoff+=(gw_outflux+cell->ground_st_am*cell->kbf/100);
  getoutput(&cell->output,GW_OUTFLUX,config) += (gw_outflux + cell->ground_st_am*cell->kbf / 100);
  cell->ground_st_am -= cell->ground_st_am*cell->kbf / 100;

  getoutput(&cell->output,GW_STORAGE,config) += (cell->ground_st + cell->ground_st_am);

  hydrotopes(cell);
  getoutput(&cell->output,MWATER,config) += cell->hydrotopes.meanwater;
  getoutput(&cell->output,WTAB,config) += cell->hydrotopes.wetland_wtable_current;
#ifdef IMAGE
  // outflow from groundwater reservoir to river
  if (cell->discharge.dmass_gw > 0)
  {
    fout_gw=cell->discharge.dmass_gw/GWCOEFF;
    cell->discharge.drunoff+=fout_gw/cell->coord.area;
    cell->discharge.dmass_gw-=fout_gw;
    getoutput(&cell->output,SEEPAGE,config)+=fout_gw/cell->coord.area;
  }
#endif

  getoutput(&cell->output,RUNOFF,config)+=cell->discharge.drunoff;
  cell->balance.awater_flux+=cell->discharge.drunoff;
  if(config->river_routing)
  {
    radiation(&daylength,&par,&eeq,cell->coord.lat,day,&climate,c_albwater,config->with_radiation);
    getoutput(&cell->output,PET,config)+=eeq*PRIESTLEY_TAYLOR*(cell->lakefrac+cell->ml.reservoirfrac);
    cell->output.mpet+=eeq*PRIESTLEY_TAYLOR*(cell->lakefrac+cell->ml.reservoirfrac);
    getoutput(&cell->output,ALBEDO,config)+=c_albwater*(cell->lakefrac+cell->ml.reservoirfrac);

    /* reservoir waterbalance */
    if(cell->ml.dam)
      update_reservoir_daily(cell,climate.prec,eeq,month,config);

    /* lake waterbalance */
    cell->discharge.dmass_lake+=climate.prec*cell->coord.area*cell->lakefrac;
    getoutput(&cell->output,INPUT_LAKE,config)+=climate.prec*cell->coord.area*cell->lakefrac;
#ifdef COUPLING_WITH_FMS
    if(cell->discharge.next==-1 && cell->lakefrac>=0.999)
      /*this if statement allows to identify the caspian sea to be an evaporating surface by lakefrac map of lpj and river rooting DDM30 map*/
      /*this does nolonger make sense if discharge next is nolonger -1 (a parameterization of a river rooting map for the casp sea is possebly used
        which is DDM30-coarsemask-zerofill.asc in /p/projects/climber3/gengel/POEM/mom5.0.2/exp/.../Data_for_LPJ), hence discharge next is not -1*/
      {
        /*here evaporation for casp sea is computed*/
        getoutput(&cell->output,EVAP_LAKE,config)+=eeq*PRIESTLEY_TAYLOR*cell->lakefrac;
        cell->balance.aevap_lake+=eeq*PRIESTLEY_TAYLOR*cell->lakefrac;
#if defined IMAGE && defined COUPLED
        if(cell->ml.image_data!=NULL)
          cell->ml.image_data->mevapotr[month] += =eeq*PRIESTLEY_TAYLOR*stand->frac;
#endif
        cell->output.dwflux+=eeq*PRIESTLEY_TAYLOR*cell->lakefrac;
        cell->discharge.dmass_lake=cell->discharge.dmass_lake-eeq*PRIESTLEY_TAYLOR*cell->coord.area*cell->lakefrac;
      }
    else if(cell->discharge.next==-9)/*discharge for ocean cells, that are threated as land by lpj on land lad resolution is computed here*/
      {
        /*
        if (cell->coord.lat<-60) //we have to exclude antarctica here since cells there have cell->discharge.next==-9 and lakefrac1 following initialization.  They should not contribute to evap of lakes here
          {
            cell->output.mevap_lake+=0;
            cell->discharge.dmass_lake=0.0;
          }
            
          else1.4.2016  changed the grid initialization in newgrid.c such that we have here no problem anymore, since the lakefraction now is nearly zero everywhere. */
          {
            getoutput(&cell->output,EVAP_LAKE,config)+=eeq*PRIESTLEY_TAYLOR*cell->lakefrac;
            cell->balance.aevap_lake+=eeq*PRIESTLEY_TAYLOR*cell->lakefrac;
#if defined IMAGE && defined COUPLED
            if(cell->ml.image_data!=NULL)
              cell->ml.image_data->mevapotr[month] += =eeq*PRIESTLEY_TAYLOR*stand->frac;
#endif
            cell->discharge.dmass_lake=max(cell->discharge.dmass_lake-eeq*PRIESTLEY_TAYLOR*cell->coord.area*cell->lakefrac,0.0);
          }
      }
    else
#endif
    {
    getoutput(&cell->output,EVAP_LAKE,config)+=min(cell->discharge.dmass_lake/cell->coord.area,eeq*PRIESTLEY_TAYLOR*cell->lakefrac);
    if(config->crop_index==ALLSTAND)
      getoutput(&cell->output,D_EVAP,config)+=min(cell->discharge.dmass_lake/cell->coord.area,eeq*PRIESTLEY_TAYLOR*cell->lakefrac);
    cell->balance.aevap_lake+=min(cell->discharge.dmass_lake/cell->coord.area,eeq*PRIESTLEY_TAYLOR*cell->lakefrac);
#if defined IMAGE && defined COUPLED
     if(cell->ml.image_data!=NULL)
       cell->ml.image_data->mevapotr[month] += min(cell->discharge.dmass_lake/cell->coord.area,eeq*PRIESTLEY_TAYLOR*cell->lakefrac);
#endif
#ifdef COUPLING_WITH_FMS
    cell->output.dwflux+=min(cell->discharge.dmass_lake/cell->coord.area,eeq*PRIESTLEY_TAYLOR*cell->lakefrac);
#endif
    cell->discharge.dmass_lake=max(cell->discharge.dmass_lake-eeq*PRIESTLEY_TAYLOR*cell->coord.area*cell->lakefrac,0.0);
    }

    getoutput(&cell->output,LAKEVOL,config)+=cell->discharge.dmass_lake;
  } /* of 'if(river_routing)' */
  getoutput(&cell->output,DAYLENGTH,config)+=daylength;

  killstand(cell,npft, cell->ml.with_tillage,intercrop,year,config);
#ifdef SAFE
  check_stand_fracs(cell,cell->lakefrac+cell->ml.reservoirfrac);
#endif
  /* Establishment fluxes are area weighted in subroutines */
  getoutput(&cell->output,FLUX_ESTABC,config)+=flux_estab.nitrogen;
  getoutput(&cell->output,FLUX_ESTABN,config)+=flux_estab.carbon;
  cell->balance.flux_estab.nitrogen+=flux_estab.nitrogen;
  cell->balance.flux_estab.carbon+=flux_estab.carbon;
  cell->output.dcflux-=flux_estab.carbon;
} /* of 'update_daily' */

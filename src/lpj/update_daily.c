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
#define GWCOEFF 0.01 /**< groundwater outflow coefficient (average amount of release time in reservoir) */
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
  int s,p,isrice;
  Pft *pft;
  Real melt=0,eeq,par,daylength,beta,gw_outflux;
  Real CH4_em=0;
  Real melt_all,runoff,snowrunoff,epsilon_gas,soilmoist,V;
#ifdef IMAGE
  //Real fout_gw; // local variable for groundwater outflow (baseflow)
#endif
  Real gtemp_air;  /* value of air temperature response function */
  Real gtemp_soil[NSOILLAYER]; /* value of soil temperature response function */
  Stocks flux_estab={0,0};
  Real evap=0;
  Real MT_water=0;
  Stocks hetres={0,0};
  Real avgprec;
  Stand *stand;
  Real ebul;
  ebul = 0;
  Real bnf;
  Real nh3;
  int l,i;
  Livefuel livefuel={0,0,0,0,0};
  const Real prec_save=climate.prec;
  Real agrfrac,fpc_total_stand;
  Real litsum_old_nv[2]={0,0},litsum_new_nv[2]={0,0};
  Real litsum_old_agr[2]={0,0},litsum_new_agr[2]={0,0};
  runoff=snowrunoff=melt_all=0;
  Irrigation *data;

#ifdef CHECK_BALANCE
  Real groundwater= cell->ground_st+cell->ground_st_am;
  Real end=0;
  Stocks start={0,0};
  Stocks start1={0,0};
  Real water_before=(cell->discharge.dmass_lake+cell->discharge.dmass_river)/cell->coord.area+groundwater;
  Real water_after=0;
  Real balanceW=0;
  Real wfluxes_old=cell->balance.awater_flux+cell->balance.atransp+cell->balance.aevap+cell->balance.ainterc+cell->balance.aevap_lake+cell->balance.aevap_res-cell->balance.airrig-cell->balance.aMT_water;
  Real exess_old=cell->balance.excess_water+cell->lateral_water;
  Real CH4_fluxes=(cell->balance.aCH4_em+cell->balance.aCH4_sink)*WC/WCH4;
  Stocks fluxes_in,fluxes_out;
  fluxes_in.carbon=cell->balance.anpp+cell->balance.flux_estab.carbon+cell->balance.influx.carbon; //influxes
  fluxes_out.carbon=cell->balance.arh+cell->balance.fire.carbon+cell->balance.neg_fluxes.carbon+cell->balance.flux_harvest.carbon+cell->balance.biomass_yield.carbon; //outfluxes
  fluxes_in.nitrogen=cell->balance.flux_estab.nitrogen+cell->balance.influx.nitrogen; //influxes
  fluxes_out.nitrogen=cell->balance.fire.nitrogen+cell->balance.n_outflux+cell->balance.neg_fluxes.nitrogen
      +cell->balance.flux_harvest.nitrogen+cell->balance.biomass_yield.nitrogen+cell->balance.deforest_emissions.nitrogen; //outfluxes
  Real irrigstore=0;
  foreachstand(stand,s,cell->standlist)
  {
    if(stand->type->landusetype!=KILL)
    {
      water_before+=soilwater(&stand->soil)*stand->frac;
      start1.carbon+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
      start1.nitrogen+=standstocks(stand).nitrogen*stand->frac;
    }
    if(stand->type->landusetype==GRASSLAND || stand->type->landusetype==OTHERS ||
       stand->type->landusetype==AGRICULTURE || stand->type->landusetype==AGRICULTURE_GRASS || stand->type->landusetype==AGRICULTURE_TREE ||
       stand->type->landusetype==BIOMASS_TREE || stand->type->landusetype==BIOMASS_GRASS || stand->type->landusetype==WOODPLANTATION)
    {
      isrice=FALSE;
      foreachpft(pft, p, &stand->pftlist)
        if(pft->par->id==config->rice_pft)
          isrice=TRUE;
      data = stand->data;
      if(data->irrigation||isrice) irrigstore+=data->irrig_stor;
    }
  }
  start1.carbon+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
      cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  start1.nitrogen+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
      cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;
  if(cell->ml.dam)
  {
    water_before+=cell->ml.resdata->dmass/cell->coord.area;/*+cell->resdata->dfout_irrigation/cell->coord.area; */
    for(i=0;i<NIRRIGDAYS;i++)
      water_before+=cell->ml.resdata->dfout_irrigation_daily[i]/cell->coord.area;
    start1.carbon+=cell->ml.resdata->pool.carbon;
    start1.nitrogen+=cell->ml.resdata->pool.nitrogen;
  }
  start=start1;
#endif

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
  killstand(cell,npft,ncft,cell->ml.with_tillage,intercrop,year,config);

  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
    update_nesterov(cell,&climate);

  agrfrac=0;
  foreachstand(stand,s,cell->standlist)
  {
    if(isagriculture(stand->type->landusetype))
      agrfrac+=stand->frac;
    for(l=0;l<stand->soil.litter.n;l++)
    {
      stand->soil.litter.item[l].agsub.leaf.carbon += stand->soil.litter.item[l].agtop.leaf.carbon*param.bioturbate;
      stand->soil.litter.item[l].agtop.leaf.carbon *= (1 - param.bioturbate);
      stand->soil.litter.item[l].agsub.leaf.nitrogen += stand->soil.litter.item[l].agtop.leaf.nitrogen*param.bioturbate;
      stand->soil.litter.item[l].agtop.leaf.nitrogen *= (1 - param.bioturbate);
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
                      climate.temp,&evap)*stand->frac;
      cell->discharge.drunoff+=snowrunoff;
      getoutput(&cell->output,EVAP,config)+=evap*stand->frac; /* evap from snow runoff*/
      cell->balance.aevap+=evap*stand->frac; /* evap from snow runoff*/
#if defined IMAGE && defined COUPLED
  if(cell->ml.image_data!=NULL)
    cell->ml.image_data->mevapotr[month] += evap*stand->frac;
#endif

#ifdef MICRO_HEATING
      /*THIS IS DEDICATED TO MICROBIOLOGICAL HEATING*/
      foreachsoillayer(l)
        stand->soil.micro_heating[l]=m_heat*stand->soil.decomC[l];
      stand->soil.micro_heating[0]+=m_heat*stand->soil.litter.decomC;
#endif
      update_soil_thermal_state(&stand->soil,climate.temp,config);
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
    getoutput(&cell->output,TWS,config)+=stand->soil.litter.agtop_moist*stand->frac;
    plant_gas_transport(stand,climate.temp,ch4,config);   //fluxes in routine written to output
    gasdiffusion(&stand->soil,climate.temp,ch4,&CH4_em,&runoff);
    cell->discharge.drunoff += runoff*stand->frac;
    if(CH4_em>0)
    {
      getoutput(&cell->output,CH4_EMISSIONS,config)+=CH4_em*stand->frac;
      cell->balance.aCH4_em+=CH4_em*stand->frac;
      if(stand->type->landusetype==WETLAND)
        getoutput(&stand->cell->output,CH4_EMISSIONS_WET,config)+=CH4_em*WC/WCH4;
    }
    else
    {
      getoutput(&cell->output,CH4_SINK,config)+=CH4_em*stand->frac;
      cell->balance.aCH4_sink+=CH4_em*stand->frac;
    }
    if((stand->type->landusetype==SETASIDE_RF || stand->type->landusetype==SETASIDE_IR || stand->type->landusetype==AGRICULTURE || stand->type->landusetype==SETASIDE_WETLAND)&& CH4_em>0)
    {
      foreachpft(pft, p, &stand->pftlist)
      {
        if(pft->par->id==config->rice_pft || stand->soil.iswetland)
        {
          getoutput(&stand->cell->output,CH4_RICE_EM,config)+=CH4_em;
          stand->cell->balance.aCH4_rice+=CH4_em*stand->frac;
        }
        else
        {
          stand->cell->balance.aCH4_setaside+=CH4_em*stand->frac;
          getoutput(&stand->cell->output,CH4_SETASIDE,config)+=CH4_em*stand->frac;

        }
      }
    }
    fpc_total_stand = 0;
    foreachpft(pft, p, &stand->pftlist)
      fpc_total_stand += pft->fpc;

    ebul = ebullition(&stand->soil, fpc_total_stand);
    getoutput(&cell->output,CH4_EMISSIONS,config) += ebul*stand->frac;
    if(stand->type->landusetype==WETLAND)
      getoutput(&stand->cell->output,CH4_EMISSIONS_WET,config)+=ebul*WC/WCH4;
    cell->balance.aCH4_em+=ebul*stand->frac;
    getoutput(&cell->output,CH4_EBULLITION,config) += ebul*stand->frac;

    if((stand->type->landusetype==SETASIDE_RF || stand->type->landusetype==SETASIDE_IR || stand->type->landusetype==AGRICULTURE || stand->type->landusetype==SETASIDE_WETLAND) && ebul>0)
    {
      foreachpft(pft, p, &stand->pftlist)
      {
        if(pft->par->id==config->rice_pft || stand->soil.iswetland)
        {
          getoutput(&stand->cell->output,CH4_RICE_EM,config)+=ebul;
          stand->cell->balance.aCH4_rice+=ebul*stand->frac;
        }
        else
        {
          stand->cell->balance.aCH4_setaside+=ebul*stand->frac;
          getoutput(&stand->cell->output,CH4_SETASIDE,config)+=ebul*stand->frac;

        }
      }
    }

    /* update soil and litter properties to account for all changes since last call of littersom */
    if(config->soilpar_option==NO_FIXED_SOILPAR || (config->soilpar_option==FIXED_SOILPAR && year<config->soilpar_fixyear))
      pedotransfer(stand,NULL,NULL,stand->frac);
    updatelitterproperties(stand,stand->frac);
    if(stand->type->landusetype==NATURAL  || stand->type->landusetype==WETLAND)
      for(l=0;l<stand->soil.litter.n;l++)
      {
        litsum_old_nv[LEAF]+=stand->soil.litter.item[l].agtop.leaf.carbon+stand->soil.litter.item[l].agsub.leaf.carbon+stand->soil.litter.item[l].bg.carbon;
        for(i=0;i<NFUELCLASS;i++)
          litsum_old_nv[WOOD]+=stand->soil.litter.item[l].agtop.wood[i].carbon+stand->soil.litter.item[l].agsub.wood[i].carbon;
      }
    if(isagriculture(stand->type->landusetype))
      for(l=0;l<stand->soil.litter.n;l++)
      {
        litsum_old_agr[LEAF]+=stand->soil.litter.item[l].agtop.leaf.carbon+stand->soil.litter.item[l].agsub.leaf.carbon+stand->soil.litter.item[l].bg.carbon;
        for(i=0;i<NFUELCLASS;i++)
          litsum_old_agr[WOOD]+=stand->soil.litter.item[l].agtop.wood[i].carbon+stand->soil.litter.item[l].agsub.wood[i].carbon;
      }
    CH4_em=0;
    hetres=littersom(stand,gtemp_soil,agrfrac,&CH4_em,climate.temp,ch4,&runoff,&MT_water,npft,ncft,config);
    cell->discharge.drunoff += runoff*stand->frac;
    if(stand->type->landusetype==NATURAL || stand->type->landusetype==WETLAND)
      cell->balance.nat_fluxes-=hetres.carbon*stand->frac;
    cell->balance.arh+=hetres.carbon*stand->frac;
    getoutput(&cell->output,RH,config)+=hetres.carbon*stand->frac;
    getoutput(&cell->output,N2O_NIT,config)+=hetres.nitrogen*stand->frac;
    cell->balance.n_outflux+=hetres.nitrogen*stand->frac;
    getoutput(&cell->output,CH4_EMISSIONS,config) += CH4_em*stand->frac;
    cell->balance.aCH4_em+=CH4_em*stand->frac;
    if(stand->type->landusetype==WETLAND)
      getoutput(&stand->cell->output,CH4_EMISSIONS_WET,config)+=CH4_em*WC/WCH4;
    if((stand->type->landusetype==SETASIDE_RF || stand->type->landusetype==SETASIDE_IR || stand->type->landusetype==AGRICULTURE || stand->type->landusetype==SETASIDE_WETLAND)&& CH4_em>0)
    {
      foreachpft(pft, p, &stand->pftlist)
      {
        if(pft->par->id==config->rice_pft || stand->soil.iswetland)
        {
          getoutput(&stand->cell->output,CH4_RICE_EM,config)+=CH4_em;
          stand->cell->balance.aCH4_rice+=CH4_em*stand->frac;
        }
        else
        {
          stand->cell->balance.aCH4_setaside+=CH4_em*stand->frac;
          getoutput(&stand->cell->output,CH4_SETASIDE,config)+=CH4_em*stand->frac;

        }
      }
    }
    cell->balance.aMT_water+= MT_water*stand->frac;
    getoutput(&cell->output,MT_WATER,config) += MT_water*stand->frac;

    if(stand->type->landusetype==NATURAL || stand->type->landusetype==WETLAND)
      for(l=0;l<stand->soil.litter.n;l++)
      {
        litsum_new_nv[LEAF]+=stand->soil.litter.item[l].agtop.leaf.carbon+stand->soil.litter.item[l].agsub.leaf.carbon+stand->soil.litter.item[l].bg.carbon;
        for(i=0;i<NFUELCLASS;i++)
          litsum_new_nv[WOOD]+=stand->soil.litter.item[l].agtop.wood[i].carbon+stand->soil.litter.item[l].agsub.wood[i].carbon;
      }
    if(isagriculture(stand->type->landusetype))
      for(l=0;l<stand->soil.litter.n;l++)
      {
        litsum_new_agr[LEAF]+=stand->soil.litter.item[l].agtop.leaf.carbon+stand->soil.litter.item[l].agsub.leaf.carbon+stand->soil.litter.item[l].bg.carbon;
        for(i=0;i<NFUELCLASS;i++)
          litsum_new_agr[WOOD]+=stand->soil.litter.item[l].agtop.wood[i].carbon+stand->soil.litter.item[l].agsub.wood[i].carbon;
      }

    /* update soil and litter properties to account for all changes from littersom */
    if(config->soilpar_option==NO_FIXED_SOILPAR || (config->soilpar_option==FIXED_SOILPAR && year<config->soilpar_fixyear))
      pedotransfer(stand,NULL,NULL,stand->frac);
    updatelitterproperties(stand,stand->frac);

    /*monthly rh for agricutural stands*/
    if (isagriculture(stand->type->landusetype))
    {
      getoutput(&cell->output,RH_AGR,config)+=hetres.carbon*stand->frac/agrfrac;
      getoutput(&cell->output,N2O_NIT_AGR,config)+=hetres.nitrogen*stand->frac;
    }
    if(stand->type->landusetype==GRASSLAND)
    {
      getoutput(&cell->output,N2O_NIT_MGRASS,config)+=hetres.nitrogen*stand->frac;
      getoutput(&cell->output,RH_MGRASS,config)+=hetres.carbon*stand->frac;
    }
    cell->output.dcflux+=hetres.carbon*stand->frac;
#if defined IMAGE && defined COUPLED
    if (stand->type->landusetype == NATURAL || stand->type->landusetype == WETLAND )
    {
      cell->rh_nat += hetres.carbon*stand->frac;
    } /* if NATURAL */
    if (stand->type->landusetype == WOODPLANTATION)
    {
      cell->rh_wp += hetres.carbon*stand->frac;
    } /* if woodplantation */
#endif

    getoutput(&cell->output,LITTERTEMP,config)+=stand->soil.litter.agtop_temp*stand->frac*(1.0/(1-cell->lakefrac-cell->ml.reservoirfrac));
    getoutput(&cell->output,SWE,config)+=stand->soil.snowpack*stand->frac;
    getoutput(&cell->output,TWS,config)+=stand->soil.snowpack*stand->frac;
    getoutput(&cell->output,SNOWRUNOFF,config)+=snowrunoff;
    getoutput(&cell->output,MELT,config)+=melt*stand->frac;
    melt_all+=melt*stand->frac;
    if(config->fire==FIRE && climate.temp>0 && stand->soil.wtable>400)
      stand->fire_sum+=fire_sum(&stand->soil.litter,stand->soil.w[0]);

    if(config->unlim_nitrogen ||
       (config->equilsoil && param.veg_equil_unlim && year<=(config->firstyear-config->nspinup+param.veg_equil_year)))
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
      cell->balance.influx.nitrogen+=2000*stand->frac;
      if (isagriculture(stand->type->landusetype))
        getoutput(&cell->output,NDEPO_AGR,config)+=2000*stand->frac;
      if(stand->type->landusetype!=NATURAL && stand->type->landusetype!=WOODPLANTATION)
        getoutput(&cell->output,NDEPO_MG,config)+=2000*stand->frac;
      getoutput(&cell->output,NDEPOS,config)+=2000*stand->frac;
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
      cell->balance.influx.nitrogen+=(climate.nh4deposition+climate.no3deposition)*stand->frac;
      if (isagriculture(stand->type->landusetype))
       getoutput(&cell->output,NDEPO_AGR,config)+=(climate.nh4deposition+climate.no3deposition)*stand->frac;
      if(stand->type->landusetype!=NATURAL && stand->type->landusetype!=WOODPLANTATION)
        getoutput(&cell->output,NDEPO_MG,config)+=(climate.nh4deposition+climate.no3deposition)*stand->frac;
      getoutput(&cell->output,NDEPOS,config)+=(climate.nh4deposition+climate.no3deposition)*stand->frac;
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

    if(!config->npp_controlled_bnf)
    {
      bnf=biologicalnfixation(stand, npft, ncft, config);
      stand->soil.NH4[0]+=bnf;
      getoutput(&cell->output,BNF,config)+=bnf*stand->frac;
      if(stand->type->landusetype!=NATURAL && stand->type->landusetype!=WOODPLANTATION)
        getoutput(&cell->output,BNF_MG,config)+=bnf*stand->frac;
      cell->balance.influx.nitrogen+=bnf*stand->frac;
    }

    runoff=daily_stand(stand,co2,&climate,day,month,daylength,
                       gtemp_air,gtemp_soil[0],eeq,par,
                       melt,npft,ncft,year,intercrop,agrfrac,config);
    denitrification(stand,npft,ncft,config);

    nh3=volatilization(stand->soil.NH4[0],climate.windspeed,climate.temp,
                       length,cell->soilph);
    if(nh3>stand->soil.NH4[0])
      nh3=stand->soil.NH4[0];
    stand->soil.NH4[0]-=nh3;
    getoutput(&cell->output,N_VOLATILIZATION,config)+=nh3*stand->frac;
    if (isagriculture(stand->type->landusetype))
      getoutput(&cell->output,NH3_AGR,config)+=nh3*stand->frac;
    if(stand->type->landusetype==GRASSLAND)
      getoutput(&cell->output,NH3_MGRASS,config)+=nh3*stand->frac;

    cell->balance.n_outflux+=nh3*stand->frac;
    cell->discharge.drunoff+=runoff*stand->frac;
    climate.prec=prec_save;
    foreachpft(pft, p, &stand->pftlist)
      getoutput(&cell->output,VEGC_AVG,config)+=vegc_sum(pft)*stand->frac*(1.0/(1-cell->lakefrac-cell->ml.reservoirfrac));
    getoutput(&cell->output,SWC1,config)+=(stand->soil.w[0]*stand->soil.whcs[0]+stand->soil.w_fw[0]+stand->soil.wpwps[0]+
              stand->soil.ice_depth[0]+stand->soil.ice_fw[0])/stand->soil.wsats[0]*stand->frac*(1.0/(1-cell->lakefrac-cell->ml.reservoirfrac));
    getoutput(&cell->output,SWC2,config)+=(stand->soil.w[1]*stand->soil.whcs[1]+stand->soil.w_fw[1]+stand->soil.wpwps[1]+
              stand->soil.ice_depth[1]+stand->soil.ice_fw[1])/stand->soil.wsats[1]*stand->frac*(1.0/(1-cell->lakefrac-cell->ml.reservoirfrac));
    getoutput(&cell->output,SWC3,config)+=(stand->soil.w[2]*stand->soil.whcs[2]+stand->soil.w_fw[2]+stand->soil.wpwps[2]+
              stand->soil.ice_depth[2]+stand->soil.ice_fw[2])/stand->soil.wsats[2]*stand->frac*(1.0/(1-cell->lakefrac-cell->ml.reservoirfrac));
    getoutput(&cell->output,SWC4,config)+=(stand->soil.w[3]*stand->soil.whcs[3]+stand->soil.w_fw[3]+stand->soil.wpwps[3]+
              stand->soil.ice_depth[3]+stand->soil.ice_fw[3])/stand->soil.wsats[3]*stand->frac*(1.0/(1-cell->lakefrac-cell->ml.reservoirfrac));
    getoutput(&cell->output,SWC5,config)+=(stand->soil.w[4]*stand->soil.whcs[4]+stand->soil.w_fw[4]+stand->soil.wpwps[4]+
              stand->soil.ice_depth[4]+stand->soil.ice_fw[4])/stand->soil.wsats[4]*stand->frac*(1.0/(1-cell->lakefrac-cell->ml.reservoirfrac));
    foreachsoillayer(l)
    {
      getoutputindex(&cell->output,SWC,l,config)+=(stand->soil.w[l]*stand->soil.whcs[l]+stand->soil.w_fw[l]+stand->soil.wpwps[l]+
                     stand->soil.ice_depth[l]+stand->soil.ice_fw[l])/stand->soil.wsats[l]*stand->frac*(1.0/(1-cell->lakefrac-cell->ml.reservoirfrac));
      getoutput(&cell->output,TWS,config)+=(stand->soil.w[l]*stand->soil.whcs[l]+stand->soil.w_fw[l]+stand->soil.wpwps[l]+
                     stand->soil.ice_depth[l]+stand->soil.ice_fw[l])*stand->frac;
    }
    forrootmoist(l)
      getoutput(&cell->output,ROOTMOIST,config)+=stand->soil.w[l]*stand->soil.whcs[l]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac)); /* absolute soil water content between wilting point and field capacity (mm) */
      /*cell->output.mrootmoist+=stand->soil.w[l]*soildepth[l]/rootdepth*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac)); previous implementation that doesn't make sense to me, because the sum of soildepth[l]/rootdepth over the first 3 layers equals 1 (JJ, June 25, 2020)*/
    //getoutput(&cell->output,SOILC1,config)+=(stand->soil.pool[l].slow.carbon+stand->soil.pool[l].fast.carbon)*stand->frac;
    forrootsoillayer(l)
    {
      V = getV(&stand->soil,l);  /*soil air content (m3 air/m3 soil)*/
      soilmoist = getsoilmoist(&stand->soil,l);
      epsilon_gas = max(0.1, V + soilmoist*stand->soil.wsat[l]*BO2);
      getoutput(&cell->output,MEANSOILO2,config) += stand->soil.O2[l] / soildepth[l] / epsilon_gas * 1000 / LASTLAYER*stand->frac;
      epsilon_gas = max(0.1, V + soilmoist*stand->soil.wsat[l]*BCH4);
      getoutput(&cell->output,MEANSOILCH4,config) += stand->soil.CH4[l] / soildepth[l] / epsilon_gas * 1000 / LASTLAYER*stand->frac;
#ifdef DEBUG
      if (p_s / R_gas / (climate.temp + 273.15)*ch4*1e-6*WCH4 * 100000<stand->soil.CH4[l] / soildepth[l] / epsilon_gas * 1000)
      {
        fprintf(stdout, "Cell lat %.2f lon %.2f CH4[%d]:%.8f dCH4:%.8f \n", cell->coord.lat, cell->coord.lon, l, stand->soil.CH4[l]);
        fprintf(stdout, "CH4[%d]:%.8f\n", l, stand->soil.CH4[l] / soildepth[l] / epsilon_gas * 1000);
        fprintf(stdout, "epsilon:%.8f day=%d\n\n\n", epsilon_gas, day);
      }
#endif
    }
    if(stand->type->landusetype==GRASSLAND || stand->type->landusetype==OTHERS ||
       stand->type->landusetype==AGRICULTURE || stand->type->landusetype==AGRICULTURE_GRASS || stand->type->landusetype==AGRICULTURE_TREE ||
       stand->type->landusetype==BIOMASS_TREE || stand->type->landusetype==BIOMASS_GRASS || stand->type->landusetype==WOODPLANTATION)
    {
      data = stand->data;
      isrice=FALSE;
      foreachpft(pft, p, &stand->pftlist)
        if(pft->par->id==config->rice_pft)
          isrice=TRUE;
      if(data->irrigation||isrice)
      {
        getoutput(&cell->output,IRRIG_STOR,config)+=data->irrig_stor*stand->frac*cell->coord.area;
        getoutput(&cell->output,TWS,config)+=data->irrig_stor*stand->frac;
      }
    }
    /* only first 5 layers for SWC_VOL output */
    forrootsoillayer(l)
    {
      getoutputindex(&cell->output,SWC_VOL,l,config)+=(stand->soil.w[l]*stand->soil.whcs[l]+stand->soil.w_fw[l]+stand->soil.wpwps[l]+
                     stand->soil.ice_depth[l]+stand->soil.ice_fw[l])*stand->frac*cell->coord.area;
    }
    if(stand->soil.iswetland)
    {
     getoutput(&cell->output,WTAB,config) += cell->hydrotopes.wetland_wtable_current;
    }

  } /* of foreachstand */
  if (cell->lateral_water>100)
  {
    cell->discharge.drunoff+=cell->lateral_water-100;
    cell->lateral_water=100;
  }

  getoutput(&cell->output,CELLFRAC_AGR,config)+=agrfrac;
  getoutput(&cell->output,DECAY_LEAF_NV,config)*=litsum_old_nv[LEAF]>0 ? litsum_new_nv[LEAF]/litsum_old_nv[LEAF] : 1;
  getoutput(&cell->output,DECAY_WOOD_NV,config)*=litsum_old_nv[WOOD]>0 ? litsum_new_nv[WOOD]/litsum_old_nv[WOOD] : 1;
  getoutput(&cell->output,DECAY_LEAF_AGR,config)*=litsum_old_agr[LEAF]>0 ? litsum_new_agr[LEAF]/litsum_old_agr[LEAF] : 1;
  getoutput(&cell->output,DECAY_WOOD_AGR,config)*=litsum_old_agr[WOOD]>0 ? litsum_new_agr[WOOD]/litsum_old_agr[WOOD] : 1;

  if(cell->discharge.dmass_gw>epsilon)
  {
    gw_outflux = cell->ground_st*GWCOEFF; //cell->kbf;
    cell->ground_st -= gw_outflux;
    cell->discharge.dmass_gw-=gw_outflux*cell->coord.area;
    cell->discharge.drunoff+=gw_outflux;
    getoutput(&cell->output,GW_OUTFLUX,config) += gw_outflux;
    gw_outflux=cell->ground_st_am*GWCOEFF/100;     //*cell->kbf/100;
    cell->ground_st_am -= gw_outflux;
    cell->discharge.dmass_gw-=gw_outflux*cell->coord.area;
    cell->discharge.drunoff+=gw_outflux;
    getoutput(&cell->output,GW_OUTFLUX,config) += gw_outflux;
  }


/////// replace storage with the new stand specific groundwater storage see above and groundwater discharge are set in infil_perc


  hydrotopes(cell);
  getoutput(&cell->output,MWATER,config) += cell->hydrotopes.meanwater;
  cell->hydrotopes.wetland_wtable_monthly+= cell->hydrotopes.wetland_wtable_current;
  cell->hydrotopes.wtable_monthly+= cell->hydrotopes.meanwater;

#ifdef IMAGE
  // outflow from groundwater reservoir to river THIS IS NOT NECESSARY ANYMORE IS HANDLED in infil_perc
//  if (cell->discharge.dmass_gw > 0)
//  {
//    fout_gw=cell->discharge.dmass_gw*GWCOEFF;
//    cell->discharge.drunoff+=fout_gw/cell->coord.area;
//    cell->discharge.dmass_gw-=fout_gw;
//    getoutput(&cell->output,SEEPAGE,config)+=fout_gw/cell->coord.area;
//  }
#endif

  getoutput(&cell->output,RUNOFF,config)+=cell->discharge.drunoff;
  cell->balance.awater_flux+=cell->discharge.drunoff;
  if(config->with_lakes)
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
          cell->ml.image_data->mevapotr[month] += eeq*PRIESTLEY_TAYLOR*stand->frac;
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
              cell->ml.image_data->mevapotr[month] += eeq*PRIESTLEY_TAYLOR*stand->frac;
#endif
            cell->discharge.dmass_lake=max(cell->discharge.dmass_lake-eeq*PRIESTLEY_TAYLOR*cell->coord.area*cell->lakefrac,0.0);
          }
      }
    else
#endif
    {
    getoutput(&cell->output,EVAP_LAKE,config)+=min(cell->discharge.dmass_lake/cell->coord.area,eeq*PRIESTLEY_TAYLOR*cell->lakefrac);
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
    getoutput(&cell->output,RIVERVOL,config)+=cell->discharge.dmass_river;
    getoutput(&cell->output,TWS,config)+=(cell->discharge.dmass_lake+cell->discharge.dmass_river)/cell->coord.area;
  } /* of 'if(river_routing)' */
  getoutput(&cell->output,DAYLENGTH,config)+=daylength;
  soilpar_output(cell,agrfrac,config);
  killstand(cell,npft, ncft,cell->ml.with_tillage,intercrop,year,config);
#ifdef SAFE
  check_stand_fracs(cell,cell->lakefrac+cell->ml.reservoirfrac,ncft);
#endif
  /* Establishment fluxes are area weighted in subroutines */
  getoutput(&cell->output,FLUX_ESTABC,config)+=flux_estab.carbon;
  getoutput(&cell->output,FLUX_ESTABN,config)+=flux_estab.nitrogen;
  cell->balance.flux_estab.nitrogen+=flux_estab.nitrogen;
  cell->balance.flux_estab.carbon+=flux_estab.carbon;
#ifdef CHECK_BALANCE
  fluxes_out.carbon=(cell->balance.arh+cell->balance.fire.carbon+cell->balance.neg_fluxes.carbon
                   +cell->balance.flux_harvest.carbon+cell->balance.biomass_yield.carbon)-fluxes_out.carbon; //outfluxes
  fluxes_in.carbon=(cell->balance.anpp+cell->balance.flux_estab.carbon+cell->balance.influx.carbon)-fluxes_in.carbon;
  water_after=(cell->discharge.dmass_lake+cell->discharge.dmass_river)/cell->coord.area+cell->ground_st+cell->ground_st_am;
  CH4_fluxes-=(cell->balance.aCH4_em+cell->balance.aCH4_sink)*WC/WCH4;                                 //will be negative, because emissions at the end are higher, thus we have to substract
  end=0;
  Real irrigstore_end=0;
  foreachstand(stand,s,cell->standlist)
  {
    if(stand->type->landusetype==GRASSLAND || stand->type->landusetype==OTHERS ||
       stand->type->landusetype==AGRICULTURE || stand->type->landusetype==AGRICULTURE_GRASS || stand->type->landusetype==AGRICULTURE_TREE ||
       stand->type->landusetype==BIOMASS_TREE || stand->type->landusetype==BIOMASS_GRASS || stand->type->landusetype==WOODPLANTATION)
     {
       isrice=FALSE;
        if(pft->par->id==config->rice_pft)
          isrice=TRUE;
       data = stand->data;
       if(data->irrigation||isrice) irrigstore_end+=data->irrig_stor;
     }
    if(stand->type->landusetype!=KILL)
    {
      water_after+=soilwater(&stand->soil)*stand->frac;
      end+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac ;
    }
  }
  end+=cell->ml.product.fast.carbon+cell->ml.product.slow.carbon+
       cell->balance.estab_storage_grass[0].carbon+cell->balance.estab_storage_tree[0].carbon+cell->balance.estab_storage_grass[1].carbon+cell->balance.estab_storage_tree[1].carbon;
  if(cell->ml.dam)
  {
    water_after+=cell->ml.resdata->dmass/cell->coord.area;/*+cell->resdata->dfout_irrigation/cell->coord.area; */
    for(i=0;i<NIRRIGDAYS;i++)
      water_after+=cell->ml.resdata->dfout_irrigation_daily[i]/cell->coord.area;
  }

  balanceW=water_after-water_before-climate.prec+
          ((cell->balance.awater_flux+cell->balance.atransp+cell->balance.aevap+cell->balance.ainterc+cell->balance.aevap_lake+cell->balance.aevap_res-cell->balance.airrig-cell->balance.aMT_water)-wfluxes_old)
          +((cell->balance.excess_water+cell->lateral_water)-exess_old);
  if (fabs(end-start1.carbon-CH4_fluxes+fluxes_out.carbon-fluxes_in.carbon)>0.001)
  {
    fprintf(stderr, "C_ERROR in %s : day: %d  %g start: %g  end: %g CH4_fluxes: %g flux_estab.carbon: %g flux_harvest.carbon: %g dcflux: %g fluxes_in.carbon: %g "
        "fluxes_out.carbon: %g neg_fluxes: %g bm_inc: %g rh: %g aCH4_sink: %g aCH4_em : %g \n",
        __FUNCTION__,day,end-start.carbon-CH4_fluxes-fluxes_in.carbon+fluxes_out.carbon,start.carbon,end,CH4_fluxes,cell->balance.flux_estab.carbon,cell->balance.flux_harvest.carbon,
        cell->output.dcflux, fluxes_in.carbon,fluxes_out.carbon, cell->balance.neg_fluxes.carbon,cell->output.bm_inc,cell->balance.arh,cell->balance.aCH4_sink*WC/WCH4,cell->balance.aCH4_em*WC/WCH4);
  }
  fluxes_out.nitrogen=(cell->balance.fire.nitrogen+cell->balance.n_outflux+cell->balance.neg_fluxes.nitrogen
                      +cell->balance.flux_harvest.nitrogen+cell->balance.biomass_yield.nitrogen+cell->balance.deforest_emissions.nitrogen)-fluxes_out.nitrogen;
  fluxes_in.nitrogen=(cell->balance.flux_estab.nitrogen+cell->balance.influx.nitrogen)-fluxes_in.nitrogen;
  end=0;
  foreachstand(stand,s,cell->standlist)
    if(stand->type->landusetype!=KILL)
      end+=standstocks(stand).nitrogen*stand->frac;
  end+=cell->ml.product.fast.nitrogen+cell->ml.product.slow.nitrogen+
       cell->balance.estab_storage_grass[0].nitrogen+cell->balance.estab_storage_tree[0].nitrogen+cell->balance.estab_storage_grass[1].nitrogen+cell->balance.estab_storage_tree[1].nitrogen;
  if (fabs(end-start1.nitrogen+fluxes_out.nitrogen-fluxes_in.nitrogen)>0.1)
  {
    fprintf(stderr, "N_ERROR in %s end: day: %d    %g start: %g  end: %g flux_estab.nitrogen: %g flux_harvest.nitrogen: %g "
           "influx: %g outflux: %g neg_fluxes: %g\n",
           __FUNCTION__,day,end-start1.nitrogen-fluxes_in.nitrogen+fluxes_out.nitrogen,start1.nitrogen, end,cell->balance.flux_estab.nitrogen,cell->balance.flux_harvest.nitrogen,
           fluxes_in.nitrogen,fluxes_out.nitrogen, cell->balance.neg_fluxes.nitrogen);
//    foreachstand(stand,s,cell->standlist)
//             fprintf(stderr,"update_daily: standfrac: %g standtype: %s s= %d iswetland: %d cropfraction_rf: %g cropfraction_irr: %g grasfrac_rf: %g grasfrac_irr: %g\n",
//                     stand->frac, stand->type->name,s,stand->soil.iswetland, crop_sum_frac(cell->ml.landfrac,12,config->nagtree,cell->ml.reservoirfrac+cell->lakefrac,FALSE),
//                     crop_sum_frac(cell->ml.landfrac,12,config->nagtree,cell->ml.reservoirfrac+cell->lakefrac,TRUE),
//                     cell->ml.landfrac[0].grass[0]+cell->ml.landfrac[0].grass[1],cell->ml.landfrac[1].grass[0]+cell->ml.landfrac[1].grass[1]);
  }
  if(fabs(balanceW)>0.1)
  {
    fprintf(stderr,"W-BALANCE-ERROR in %s: day %d balanceW: %g  exess_old: %g balance.excess_water: %g gw_outflux: %g water_after: %g water_before: %g prec: %g melt: %g "
        "atransp: %g  aevap %g ainterc %g aevap_lake  %g aevap_res: %g    airrig : %g aMT_water : %g MT_water: %g flux_bal: %g runoff %g awater_flux %g lateral_water %g mfin-mfout: %g dmass_lake: %g  dmassriver : %g"
        "  ground_st_am: %g ground_st: %g gw_balance: %g  groundwater: %g  irrigstore_bal: %g\n\n",
        __FUNCTION__,day,balanceW,exess_old,cell->balance.excess_water,gw_outflux,
        water_after,water_before,climate.prec,melt_all,cell->balance.atransp,cell->balance.aevap,cell->balance.ainterc,cell->balance.aevap_lake,cell->balance.aevap_res,cell->balance.airrig,cell->balance.aMT_water,MT_water,
        ((cell->balance.awater_flux+cell->balance.atransp+cell->balance.aevap+cell->balance.ainterc+cell->balance.aevap_lake+cell->balance.aevap_res-cell->balance.airrig-cell->balance.aMT_water)-wfluxes_old),
        cell->discharge.drunoff,cell->balance.awater_flux,cell->lateral_water,((cell->discharge.mfout-cell->discharge.mfin)/cell->coord.area),cell->discharge.dmass_lake/cell->coord.area,cell->discharge.dmass_river/cell->coord.area,
        cell->ground_st_am,cell->ground_st,groundwater-(cell->ground_st_am+cell->ground_st),groundwater,irrigstore_end-irrigstore);
  }
 //if(year==-4407) fprintf(stderr,"day: %d\n",day);
#endif



} /* of 'update_daily' */

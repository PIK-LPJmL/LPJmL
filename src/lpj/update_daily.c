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
#include "natural.h"
#include "agriculture.h"

#define length 1.0 /* characteristic length (m) */
#define BIOTURBRATE 0.001897 /* daily rate for 50% annual bioturbation rate [-]*/

void update_daily(Cell *cell,            /**< cell pointer           */
                  Real co2,              /**< atmospheric CO2 (ppmv) */
                  Real popdensity,       /**< population density (capita/km2) */
                  Dailyclimate climate,  /**< Daily climate values */
                  int day,               /**< day (1..365)           */
                  int npft,              /**< number of natural PFTs */
                  int ncft,              /**< number of crop PFTs   */
                  int year,              /**< simulation year */
                  int month,             /**< month (0..11) */
                  Bool withdailyoutput,  /**< enable daily output */
                  Bool intercrop,        /**< enable intercropping */
                  const Config *config   /**< LPJmL configuration */
                 )
{
  int p,s;
  Real melt=0,eeq,par,daylength,beta;
  Real gp_stand,gp_stand_leafon,runoff,snowrunoff;
  Real fpc_total_stand;
  Real gtemp_air;  /* value of air temperature response function */
  Real gtemp_soil[NSOILLAYER]; /* value of soil temperature response function */
  Real temp_bs;    /* temperature beneath snow */
  Real prec_energy; /* energy from temperature difference between rain and soil [J/m2]*/
  Stocks flux_estab={0,0};
  Real evap=0;
  Stocks hetres={0,0};
  Real *gp_pft;
  Stand *stand;
  Pft *pft;
  Real bnf;
  Real nh3;
  Irrigation *data;
  int index,l;
  Real rootdepth=0.0;
  Livefuel livefuel={0,0,0,0,0};
  const Real prec_save=climate.prec;
  gp_pft=newvec(Real,npft+ncft);
  check(gp_pft);

  forrootmoist(l)
    rootdepth+=soildepth[l];

  updategdd(cell->gdd,config->pftpar,npft,climate.temp);
  cell->balance.aprec+=climate.prec;
  gtemp_air=temp_response(climate.temp);
  daily_climbuf(&cell->climbuf,climate.temp);

  cell->output.mprec+=climate.prec;
  cell->output.msnowf+=climate.temp<tsnow ? climate.prec : 0;
  cell->output.mrain+=climate.temp<tsnow ? 0 : climate.prec;

  if(cell->ml.landfrac!=NULL) /* landuse enabled? */
    flux_estab=sowing(cell,climate.prec,day,year,npft,ncft,config); 
  cell->discharge.drunoff=0.0;

  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
    update_nesterov(cell,&climate);
  foreachstand(stand,s,cell->standlist)
  {
    for (l = 0; l < stand->soil.litter.n; l++) {
      stand->soil.litter.item[l].agsub.leaf.carbon += stand->soil.litter.item[l].ag.leaf.carbon*BIOTURBRATE;
      stand->soil.litter.item[l].ag.leaf.carbon *= 1 - BIOTURBRATE;
      stand->soil.litter.item[l].agsub.leaf.nitrogen += stand->soil.litter.item[l].ag.leaf.nitrogen*BIOTURBRATE;
      stand->soil.litter.item[l].ag.leaf.nitrogen *= 1 - BIOTURBRATE;
    }

    if(stand->type->landusetype==NATURAL && config->black_fallow && (day==152 || day==335))
    {
      if(config->prescribe_residues && param.residue_rate>0 && param.residue_pool<=0)
      {
        index=findlitter(&stand->soil.litter,config->pftpar+npft);
        if(index==NOT_FOUND)
          index=addlitter(&stand->soil.litter,config->pftpar+npft)-1;
        stand->soil.litter.item[index].ag.leaf.carbon+=param.residue_rate*(1-param.residue_fbg)/2;
        stand->soil.litter.item[index].ag.leaf.nitrogen+=param.residue_rate*(1-param.residue_fbg)/param.residue_cn/2;
        stand->soil.litter.item[index].bg.carbon+=param.residue_rate*param.residue_fbg*0.5;
        stand->soil.litter.item[index].bg.nitrogen+=param.residue_rate*param.residue_fbg/param.residue_cn/2;
        stand->cell->output.flux_estab.carbon+=param.residue_rate*0.5;
        stand->cell->output.flux_estab.nitrogen+=param.residue_rate/param.residue_cn*0.5;
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
    cell->output.mpet+=eeq*PRIESTLEY_TAYLOR*stand->frac;
    cell->output.malbedo += beta * stand->frac;

    if((config->fire==SPITFIRE  || config->fire==SPITFIRE_TMAX)&& cell->afire_frac<1)
      dailyfire_stand(stand,&livefuel,popdensity,&climate,config->ntypes,config->prescribe_burntarea);
    if(config->permafrost)
    {
      snowrunoff=snow(&stand->soil,&climate.prec,&melt,
                      climate.temp,&temp_bs,&evap)*stand->frac;
      cell->discharge.drunoff+=snowrunoff;
      cell->output.mevap+=evap*stand->frac; /* evap from snow runoff*/
      if(climate.prec+melt>epsilon)
      {
        prec_energy=climate.temp*climate.prec*1e-3*c_water + T_zero*melt*1e-3*c_water + stand->soil.litter.agtop_temp*stand->soil.litter.agtop_moist*1e-3*c_water + stand->soil.litter.agtop_temp*litter_ag_sum(&stand->soil.litter)/0.5*1e-3/1300*2.5e6;
        stand->soil.litter.agtop_temp=prec_energy/(climate.prec*1e-3*c_water + melt*1e-3*c_water + stand->soil.litter.agtop_moist*1e-3*c_water + litter_ag_sum(&stand->soil.litter)/0.5*1e-3/1300*2.5e6);
        stand->soil.perc_energy[TOPLAYER]=(stand->soil.litter.agtop_temp-stand->soil.temp[TOPLAYER])*(climate.prec+melt)*1e-3*c_water;
      }
      else
        stand->soil.perc_energy[TOPLAYER]=0;

      prec_energy = ((climate.temp-stand->soil.temp[TOPLAYER])*climate.prec*1e-3
                    +melt*1e-3*(T_zero-stand->soil.temp[TOPLAYER]))*c_water;
      stand->soil.perc_energy[TOPLAYER]=prec_energy;
#ifdef MICRO_HEATING
      /*THIS IS DEDICATED TO MICROBIOLOGICAL HEATING*/
      foreachsoillayer(l)
        stand->soil.micro_heating[l]=m_heat*stand->soil.decomC[l];
      stand->soil.micro_heating[0]+=m_heat*stand->soil.litter.decomC;
#endif

      soiltemp(&stand->soil,temp_bs);
      foreachsoillayer(l)
        gtemp_soil[l]=temp_response(stand->soil.temp[l]);
    }
    else
    {
      gtemp_soil[0]=temp_response(soiltemp_lag(&stand->soil,&cell->climbuf));
      for(l=1;l<NSOILLAYER;l++)
        gtemp_soil[l]=gtemp_soil[0];
      stand->soil.litter.agtop_temp=soiltemp_lag(&stand->soil,&cell->climbuf);
      snowrunoff=snow_old(&stand->soil.snowpack,&climate.prec,&melt,climate.temp)*stand->frac;
      cell->discharge.drunoff+=snowrunoff;
    }
    foreachsoillayer(l)
      cell->output.msoiltemp[l]+=stand->soil.temp[l]*ndaymonth1[month]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));

    /* update soil and litter properties to account for all changes since last call of littersom */
    pedotransfer(stand,NULL,NULL,stand->frac);
    updatelitterproperties(stand,stand->frac);

    hetres=littersom(stand,gtemp_soil);

    if(stand->type->landusetype==NATURAL && config->black_fallow && config->prescribe_residues && param.residue_pool>0)
    {
      index=findlitter(&stand->soil.litter,config->pftpar+npft);
      if(index==NOT_FOUND)
        index=addlitter(&stand->soil.litter,config->pftpar+npft)-1;
      stand->cell->output.flux_estab.carbon+=param.residue_pool-stand->soil.litter.item[index].ag.leaf.carbon;
      stand->soil.litter.item[index].ag.leaf.carbon=param.residue_pool;
      stand->cell->output.flux_estab.nitrogen+=param.residue_pool/param.residue_cn-stand->soil.litter.item[index].ag.leaf.nitrogen;
      stand->soil.litter.item[index].ag.leaf.nitrogen=param.residue_pool/param.residue_cn;
    }

    /* update soil and litter properties to account for all changes from littersom */
    pedotransfer(stand,NULL,NULL,stand->frac);
    updatelitterproperties(stand,stand->frac);

    cell->output.mrh+=hetres.carbon*stand->frac;
    cell->output.mn2o_nit+=hetres.nitrogen*stand->frac;
    cell->output.dcflux+=hetres.carbon*stand->frac;
    cell->output.mswe+=stand->soil.snowpack*stand->frac;
    if (withdailyoutput)
    {
      switch(stand->type->landusetype)
      {
        case GRASSLAND:
          data = stand->data;
          if (cell->output.daily.cft == TEMPERATE_HERBACEOUS && cell->output.daily.irrigation == data->irrigation)
          {
            cell->output.daily.rh  += hetres.carbon;
            cell->output.daily.swe += stand->soil.snowpack;
          }
          break;
        case AGRICULTURE:
          data = stand->data;
          foreachpft(pft,p,&stand->pftlist)
            if (pft->par->id == cell->output.daily.cft && cell->output.daily.irrigation == data->irrigation)
            {
              cell->output.daily.rh  = hetres.carbon;
              cell->output.daily.swe = stand->soil.snowpack;
            }
          break;
        case NATURAL:
          if (cell->output.daily.cft == ALLNATURAL)
          {
            cell->output.daily.rh  += hetres.carbon;
            cell->output.daily.swe += stand->soil.snowpack;
          }
          break;
      } /* of switch() */
    }

    cell->output.msnowrunoff+=snowrunoff;
    cell->output.mmelt+=melt*stand->frac;

    if(config->fire==FIRE && climate.temp>0)
      stand->fire_sum+=fire_sum(&stand->soil.litter,stand->soil.w[0]);
    if(config->with_nitrogen)
    {
      if(config->with_nitrogen==UNLIM_NITROGEN)
      {
        if(stand->soil.par->type==ROCK)
          stand->cell->output.mn_leaching+=2000*stand->frac;
        else
        {
          stand->soil.NH4[0]+=1000;
          stand->soil.NO3[0]+=1000;
        }
        cell->balance.n_influx+=2000*stand->frac;
      }
      else if(!config->no_ndeposition)
      {
        if(stand->soil.par->type==ROCK)
        {
          stand->cell->output.mn_leaching+=climate.nh4deposition*stand->frac;
          stand->cell->output.mn_leaching+=climate.no3deposition*stand->frac;
        }
        else
        {
          /*adding daily N deposition to upper soil layer*/
          stand->soil.NH4[0]+=climate.nh4deposition;
          stand->soil.NO3[0]+=climate.no3deposition;
        }
        cell->balance.n_influx+=(climate.nh4deposition+climate.no3deposition)*stand->frac;
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
    gp_stand=gp_sum(&stand->pftlist,co2,climate.temp,par,daylength,
                    &gp_stand_leafon,gp_pft,&fpc_total_stand,config);
    if(config->with_nitrogen)
    {
      bnf=biologicalnfixation(stand);
      stand->soil.NH4[0]+=bnf;
      cell->output.mbnf+=bnf*stand->frac;
    }
    runoff=daily_stand(stand,co2,&climate,day,daylength,gp_pft,
                       gtemp_air,gtemp_soil[0],gp_stand,gp_stand_leafon,eeq,par,
                       melt,npft,ncft,year,withdailyoutput,intercrop,config);
    if(config->with_nitrogen)
    {
      denitrification(stand);
      nh3=volatilization(stand->soil.NH4[0],climate.windspeed,climate.temp,
                         length,cell->soilph);
      if(nh3>stand->soil.NH4[0])
        nh3=stand->soil.NH4[0];
      stand->soil.NH4[0]-=nh3;
      cell->output.mn_volatilization+=nh3*stand->frac;
      cell->balance.n_outflux+=nh3*stand->frac;
    }

    cell->discharge.drunoff+=runoff*stand->frac;
    climate.prec=prec_save;

    foreachsoillayer(l)
      cell->output.mswc[l]+=(stand->soil.w[l]*stand->soil.whcs[l]+stand->soil.w_fw[l]+stand->soil.wpwps[l]+
                     stand->soil.ice_depth[l]+stand->soil.ice_fw[l])/stand->soil.wsats[l]*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    forrootmoist(l)
      cell->output.mrootmoist+=stand->soil.w[l]*soildepth[l]/rootdepth*stand->frac*(1.0/(1-stand->cell->lakefrac-stand->cell->ml.reservoirfrac));
    cell->output.msoilc1+=(stand->soil.pool[l].slow.carbon+stand->soil.pool[l].fast.carbon)*stand->frac;
  } /* of foreachstand */

#ifdef COUPLING_WITH_FMS
  if (cell->lakefrac > 0)
  {
    laketemp(cell, &climate);
    cell->output.mlaketemp+=cell->laketemp*ndaymonth1[month];
  }
  else
    cell->output.mlaketemp=config->missing_value;
#endif

  cell->output.mrunoff+=cell->discharge.drunoff;
  cell->output.daily.runoff+=cell->discharge.drunoff;
  if(config->river_routing)
  {
    radiation(&daylength,&par,&eeq,cell->coord.lat,day,&climate,c_albwater,config->with_radiation);
    cell->output.mpet+=eeq*PRIESTLEY_TAYLOR*(cell->lakefrac+cell->ml.reservoirfrac);
    cell->output.malbedo+=c_albwater*(cell->lakefrac+cell->ml.reservoirfrac);

    /* reservoir waterbalance */
    if(cell->ml.dam)
      update_reservoir_daily(cell,climate.prec,eeq);

    /* lake waterbalance */
    cell->discharge.dmass_lake+=climate.prec*cell->coord.area*cell->lakefrac;
    cell->output.input_lake+=climate.prec*cell->coord.area*cell->lakefrac;
#ifdef COUPLING_WITH_FMS
    if(cell->discharge.next==-1 && cell->lakefrac>=0.999)
      /*this if statement allows to identify the caspian sea to be an evaporating surface by lakefrac map of lpj and river rooting DDM30 map*/
      /*this does nolonger make sense if discharge next is nolonger -1 (a parameterization of a river rooting map for the casp sea is possebly used
        which is DDM30-coarsemask-zerofill.asc in /p/projects/climber3/gengel/POEM/mom5.0.2/exp/.../Data_for_LPJ), hence discharge next is not -1*/
      {
        /*here evaporation for casp sea is computed*/
        cell->output.mevap_lake+=eeq*PRIESTLEY_TAYLOR*cell->lakefrac;
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
            cell->output.mevap_lake+=eeq*PRIESTLEY_TAYLOR*cell->lakefrac;
            cell->discharge.dmass_lake=max(cell->discharge.dmass_lake-eeq*PRIESTLEY_TAYLOR*cell->coord.area*cell->lakefrac,0.0);
          }
      }
    else
#endif
    {
    cell->output.mevap_lake+=min(cell->discharge.dmass_lake/cell->coord.area,eeq*PRIESTLEY_TAYLOR*cell->lakefrac);
#ifdef COUPLING_WITH_FMS
    cell->output.dwflux+=min(cell->discharge.dmass_lake/cell->coord.area,eeq*PRIESTLEY_TAYLOR*cell->lakefrac);
#endif
    cell->discharge.dmass_lake=max(cell->discharge.dmass_lake-eeq*PRIESTLEY_TAYLOR*cell->coord.area*cell->lakefrac,0.0);
    }

    cell->output.mlakevol+=cell->discharge.dmass_lake*ndaymonth1[month];
  } /* of 'if(river_routing)' */


  killstand(cell,config->pftpar,npft,config->with_tillage,intercrop,year);
#ifdef SAFE
  check_stand_fracs(cell,cell->lakefrac+cell->ml.reservoirfrac);
#endif
  /* Establishment fluxes are area weighted in subroutines */
  cell->output.flux_estab.nitrogen+=flux_estab.nitrogen;
  cell->output.flux_estab.carbon+=flux_estab.carbon;
  cell->output.dcflux-=flux_estab.carbon;
  free(gp_pft);
} /* of 'update_daily' */

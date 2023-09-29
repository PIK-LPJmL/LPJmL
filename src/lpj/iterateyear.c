/**************************************************************************************/
/**                                                                                \n**/
/**               i  t  e  r  a  t  e  y  e  a  r  .  c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function performs iteration over the cell grid for one year with           \n**/
/**     river routing enabled.                                                     \n**/
/**                                                                                \n**/
/**     Principal structure:                                                       \n**/
/**                                                                                \n**/
/**           for(cell=0;cell<config->ngridcell;cell++)                            \n**/
/**             init_annual();                                                     \n**/
/**           foreachmonth(month)                                                  \n**/
/**           {                                                                    \n**/
/**             foreachdayofmonth(dayofmonth,month)                                \n**/
/**             {                                                                  \n**/
/**               for(cell=0;cell<config->ngridcell;cell++)                        \n**/
/**                 update_daily();                                                \n**/
/**               drain();                                                         \n**/
/**             }                                                                  \n**/
/**             for(cell=0;cell<config->ngridcell;cell++)                          \n**/
/**               update_monthly();                                                \n**/
/**           }                                                                    \n**/
/**           for(cell=0;cell<config->ngridcell;cell++)                            \n**/
/**           {                                                                    \n**/
/**             update_annual();                                                   \n**/
/**             check_fluxes();                                                    \n**/
/**           }                                                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void iterateyear(Outputfile *output,  /**< Output file data */
    Cell grid[],         /**< cell array */
    Input input,         /**< input data */
    Real co2,            /**< atmospheric CO2 (ppmv) */
    Real ch4,            /**< CH4 concentration (ppmv) */
    int npft,            /**< number of natural PFTs */
    int ncft,            /**< number of crop PFTs */
    int year,            /**< simulation year (AD) */
    const Config *config /**< LPJ configuration */
)
{
  Dailyclimate daily;
  Bool intercrop;
  int month,dayofmonth,day;
  int cell, s;
  Stand *stand;
  Real popdens=0; /* population density (capita/km2) */
  Real norg_soil_agr,nmin_soil_agr,nveg_soil_agr;
  intercrop=getintercrop(input.landuse);
  for(cell=0;cell<config->ngridcell;cell++)
  {
#ifdef CHECK_BALANCE2
      int i;
      Real end=0;
      Stocks start={0,0};
      Stocks fluxes_in,fluxes_out;
      Real water_before=(grid[cell].discharge.dmass_lake+grid[cell].discharge.dmass_river)/grid[cell].coord.area;
      Real water_after=0;
      Real balanceW=0;
      Real wfluxes_old=grid[cell].balance.awater_flux+grid[cell].balance.atransp+grid[cell].balance.aevap+grid[cell].balance.ainterc+grid[cell].balance.aevap_lake+grid[cell].balance.aevap_res-grid[cell].balance.airrig-grid[cell].balance.aMT_water;
      Real exess_old=grid[cell].balance.excess_water+grid[cell].lateral_water;
      Real CH4_fluxes=(grid[cell].balance.aCH4_em+grid[cell].balance.aCH4_sink)*WC/WCH4;
      fluxes_in.carbon=grid[cell].balance.anpp+grid[cell].balance.flux_estab.carbon+grid[cell].balance.influx.carbon; //influxes
      fluxes_out.carbon=grid[cell].balance.arh+grid[cell].balance.fire.carbon+grid[cell].balance.flux_firewood.carbon+grid[cell].balance.neg_fluxes.carbon
          +grid[cell].balance.flux_harvest.carbon+grid[cell].balance.biomass_yield.carbon+grid[cell].balance.deforest_emissions.carbon; //outfluxes
      fluxes_in.nitrogen=grid[cell].balance.flux_estab.nitrogen+grid[cell].balance.influx.nitrogen; //influxes
      fluxes_out.nitrogen=grid[cell].balance.fire.nitrogen+grid[cell].balance.flux_firewood.nitrogen+grid[cell].balance.n_outflux+grid[cell].balance.neg_fluxes.nitrogen
          +grid[cell].balance.flux_harvest.nitrogen+grid[cell].balance.biomass_yield.nitrogen+grid[cell].balance.deforest_emissions.nitrogen; //outfluxes
      foreachstand(stand,s,grid[cell].standlist)
      {
        water_before+=soilwater(&stand->soil)*stand->frac;
        start.carbon+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
        start.nitrogen+=standstocks(stand).nitrogen*stand->frac;
      }
      start.carbon+=grid[cell].ml.product.fast.carbon+grid[cell].ml.product.slow.carbon+
          grid[cell].balance.estab_storage_grass[0].carbon+grid[cell].balance.estab_storage_tree[0].carbon+grid[cell].balance.estab_storage_grass[1].carbon+grid[cell].balance.estab_storage_tree[1].carbon;
      start.nitrogen+=grid[cell].ml.product.fast.nitrogen+grid[cell].ml.product.slow.nitrogen+
          grid[cell].balance.estab_storage_grass[0].nitrogen+grid[cell].balance.estab_storage_tree[0].nitrogen+grid[cell].balance.estab_storage_grass[1].nitrogen+grid[cell].balance.estab_storage_tree[1].nitrogen;
      if(grid[cell].ml.dam)
      {
        water_before+=grid[cell].ml.resdata->dmass/grid[cell].coord.area;/*+grid[cell].resdata->dfout_irrigation/grid[cell].coord.area; */
        for(i=0;i<NIRRIGDAYS;i++)
          water_before+=grid[cell].ml.resdata->dfout_irrigation_daily[i]/grid[cell].coord.area;
      }
#endif
    initoutputdata(&grid[cell].output,ANNUAL,year,config);
    grid[cell].balance.surface_storage=grid[cell].balance.adischarge=0;
    grid[cell].discharge.afin_ext=0;
    if(!grid[cell].skip)
    {
      init_annual(grid+cell,ncft,config);
      if (grid[cell].is_glaciated)
        check_glaciated(grid + cell,config);
      else
      {
        if(config->withlanduse)
        {
          if(grid[cell].lakefrac<1)
          {
            /* calculate landuse change */
            if(config->laimax_interpolate==LAIMAX_INTERPOLATE)
              laimax_manage(&grid[cell].ml.manage,config->pftpar+npft,npft,ncft,year);
            if(year>config->firstyear-config->nspinup || config->from_restart)
              landusechange(grid+cell,npft,ncft,intercrop,year,config);
            else if(grid[cell].ml.dam)
              landusechange_for_reservoir(grid+cell,npft,ncft,
                  intercrop,year,config);
          }
#if defined IMAGE && defined COUPLED
          setoutput_image(grid+cell,ncft,config);
#endif
          getnsoil_agr(&norg_soil_agr,&nmin_soil_agr,&nveg_soil_agr,grid+cell);
          getoutput(&grid[cell].output,DELTA_NORG_SOIL_AGR,config)-=norg_soil_agr;
          getoutput(&grid[cell].output,DELTA_NMIN_SOIL_AGR,config)-=nmin_soil_agr;
          getoutput(&grid[cell].output,DELTA_NVEG_SOIL_AGR,config)-=nveg_soil_agr;
          foreachstand(stand,s,(grid+cell)->standlist)
          if(stand->type->landusetype==GRASSLAND)
            getoutput(&grid[cell].output,DELTAC_MGRASS,config)-=standstocks(stand).carbon*stand->frac;
        }
      }
      initgdd(grid[cell].gdd,npft);
    } /*gridcell skipped*/
#ifdef CHECK_BALANCE2
      fluxes_out.carbon=(grid[cell].balance.arh+grid[cell].balance.fire.carbon+grid[cell].balance.flux_firewood.carbon+grid[cell].balance.neg_fluxes.carbon
          +grid[cell].balance.flux_harvest.carbon+grid[cell].balance.biomass_yield.carbon+grid[cell].balance.deforest_emissions.carbon)-fluxes_out.carbon; //outfluxes
      fluxes_in.carbon=(grid[cell].balance.anpp+grid[cell].balance.flux_estab.carbon+grid[cell].balance.influx.carbon)-fluxes_in.carbon;
      fluxes_out.nitrogen=(grid[cell].balance.fire.nitrogen+grid[cell].balance.flux_firewood.nitrogen+grid[cell].balance.n_outflux+grid[cell].balance.neg_fluxes.nitrogen
          +grid[cell].balance.flux_harvest.nitrogen+grid[cell].balance.biomass_yield.nitrogen+grid[cell].balance.deforest_emissions.nitrogen)-fluxes_out.nitrogen;
      fluxes_in.nitrogen=(grid[cell].balance.flux_estab.nitrogen+grid[cell].balance.influx.nitrogen)-fluxes_in.nitrogen;
      water_after=(grid[cell].discharge.dmass_lake+grid[cell].discharge.dmass_river)/grid[cell].coord.area;
      CH4_fluxes-=(grid[cell].balance.aCH4_em+grid[cell].balance.aCH4_sink)*WC/WCH4;                                 //will be negative, because emissions at the end are higher, thus we have to substract
      end=0;
      foreachstand(stand,s,grid[cell].standlist)
      {
        water_after+=soilwater(&stand->soil)*stand->frac;
        end+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac ;
      }
      end+=grid[cell].ml.product.fast.carbon+grid[cell].ml.product.slow.carbon+
          grid[cell].balance.estab_storage_grass[0].carbon+grid[cell].balance.estab_storage_tree[0].carbon+grid[cell].balance.estab_storage_grass[1].carbon+grid[cell].balance.estab_storage_tree[1].carbon;
      if(grid[cell].ml.dam)
      {
        water_after+=grid[cell].ml.resdata->dmass/grid[cell].coord.area;/*+grid[cell].resdata->dfout_irrigation/grid[cell].coord.area; */
        for(i=0;i<NIRRIGDAYS;i++)
          water_after+=grid[cell].ml.resdata->dfout_irrigation_daily[i]/grid[cell].coord.area;
      }

      balanceW=water_after-water_before+
          ((grid[cell].balance.awater_flux+grid[cell].balance.atransp+grid[cell].balance.aevap+grid[cell].balance.ainterc+grid[cell].balance.aevap_lake+grid[cell].balance.aevap_res-grid[cell].balance.airrig-grid[cell].balance.aMT_water)-wfluxes_old)
          +((grid[cell].balance.excess_water+grid[cell].lateral_water)-exess_old);
      if (fabs(end-start.carbon-CH4_fluxes+fluxes_out.carbon-fluxes_in.carbon)>0.001)
      {
        fprintf(stderr, "C_ERROR-iterateyear after update_annual: day: %d    %g start: %g  end: %g CH4_fluxes: %g flux_estab.carbon: %g flux_harvest.carbon: %g dcflux: %g fluxes_in.carbon: %g "
            "fluxes_out.carbon: %g neg_fluxes: %g bm_inc: %g rh: %g aCH4_sink: %g aCH4_em : %g \n",
            day,end-start.carbon-CH4_fluxes-fluxes_in.carbon+fluxes_out.carbon,start.carbon,end,CH4_fluxes,grid[cell].balance.flux_estab.carbon,grid[cell].balance.flux_harvest.carbon,
            grid[cell].output.dcflux, fluxes_in.carbon,fluxes_out.carbon, grid[cell].balance.neg_fluxes.carbon,grid[cell].output.bm_inc,grid[cell].balance.arh,grid[cell].balance.aCH4_sink*WC/WCH4,grid[cell].balance.aCH4_em*WC/WCH4);
        //    foreachstand(stand,s,grid[cell].standlist)
        //             fprintf(stderr,"update_daily: standfrac: %g standtype: %s s= %d iswetland: %d cropfraction_rf: %g cropfraction_irr: %g grasfrac_rf: %g grasfrac_irr: %g\n",
        //                     stand->frac, stand->type->name,s,stand->soil.iswetland, crop_sum_frac(grid[cell].ml.landfrac,12,config->nagtree,grid[cell].ml.reservoirfrac+grid[cell].lakefrac,FALSE),
        //                     cr.carbonop_sum_frac(grid[cell].ml.landfrac,12,config->nagtree,grid[cell].ml.reservoirfrac+grid[cell].lakefrac,TRUE),
        //                     grid[cell].ml.landfrac[0].grass[0]+grid[cell].ml.landfrac[0].grass[1],grid[cell].ml.landfrac[1].grass[0]+grid[cell].ml.landfrac[1].grass[1]);
      }
      end=0;
      foreachstand(stand,s,grid[cell].standlist)
      end+=standstocks(stand).nitrogen*stand->frac;
      end+=grid[cell].ml.product.fast.nitrogen+grid[cell].ml.product.slow.nitrogen+
          grid[cell].balance.estab_storage_grass[0].nitrogen+grid[cell].balance.estab_storage_tree[0].nitrogen+grid[cell].balance.estab_storage_grass[1].nitrogen+grid[cell].balance.estab_storage_tree[1].nitrogen;
      if (fabs(end-start.nitrogen+fluxes_out.nitrogen-fluxes_in.nitrogen)>0.005)
      {
        fprintf(stderr, "N_ERROR-iterateyear after landusechange: day: %d    %g start: %g  end: %g flux_estab.nitrogen: %g flux_harvest.nitrogen: %g "
            "influx: %g outflux: %g neg_fluxes: %g\n",
            day,end-start.nitrogen-fluxes_in.nitrogen+fluxes_out.nitrogen,start.nitrogen, end,grid[cell].balance.flux_estab.nitrogen,grid[cell].balance.flux_harvest.nitrogen,
            fluxes_in.nitrogen,fluxes_out.nitrogen, grid[cell].balance.neg_fluxes.nitrogen);
      }
      if(fabs(balanceW)>0.1)
        fprintf(stderr,"W-BALANCE-ERROR in iterateyear after landusechange: day %d balanceW: %g  exess_old: %g balance.excess_water: %g water_after: %g water_before: %g"
            "atransp: %g  aevap %g ainterc %g aevap_lake  %g aevap_res: %g    airrig : %g aMT_water : %g flux_bal: %g runoff %g awater_flux %g lateral_water %g mfin-mfout : %g dmass_lake : %g  dmassriver : %g  \n",day,balanceW,exess_old,grid[cell].balance.excess_water,
            water_after,water_before,grid[cell].balance.atransp,grid[cell].balance.aevap,grid[cell].balance.ainterc,grid[cell].balance.aevap_lake,grid[cell].balance.aevap_res,grid[cell].balance.airrig,grid[cell].balance.aMT_water,
            ((grid[cell].balance.awater_flux+grid[cell].balance.atransp+grid[cell].balance.aevap+grid[cell].balance.ainterc+grid[cell].balance.aevap_lake+grid[cell].balance.aevap_res-grid[cell].balance.airrig-grid[cell].balance.aMT_water)-wfluxes_old),
            grid[cell].discharge.drunoff,grid[cell].balance.awater_flux,grid[cell].lateral_water,((grid[cell].discharge.mfout-grid[cell].discharge.mfin)/grid[cell].coord.area),grid[cell].discharge.dmass_lake/grid[cell].coord.area,grid[cell].discharge.dmass_river/grid[cell].coord.area);
#endif
  } /* of for(cell=...) */

  day=1;
  foreachmonth(month)
  {
    for(cell=0;cell<config->ngridcell;cell++)
    {
      grid[cell].discharge.mfin=grid[cell].discharge.mfout=grid[cell].ml.mdemand=0.0;
      grid[cell].output.mpet=0;
      grid[cell].hydrotopes.wetland_wtable_monthly=0;
      grid[cell].hydrotopes.wtable_monthly=0;
      if(grid[cell].ml.dam)
        grid[cell].ml.resdata->mprec_res=0;
      initoutputdata(&((grid+cell)->output),MONTHLY,year,config);
      if(!grid[cell].skip)
      {
        initclimate_monthly(input.climate,&grid[cell].climbuf,cell,month,grid[cell].seed);

#if defined IMAGE && defined COUPLED
        monthlyoutput_image(&grid[cell].output,input.climate,cell,month,config);
#endif

#ifdef DEBUG
        printf("temp = %.2f prec = %.2f wet = %.2f",
            (getcelltemp(input.climate,cell))[month],
            (getcellprec(input.climate,cell))[month],
            (israndomprec(input.climate)) ? (getcellwet(input.climate,cell))[month] : 0);
        if(config->with_radiation)
        {
          if(config->with_radiation==RADIATION)
            printf("lwnet = %.2f ",(getcelllwnet(input.climate,cell))[month]);
          else if(config->with_radiation==RADIATION_LWDOWN)
            printf("lwdown = %.2f ",(getcelllwnet(input.climate,cell))[month]);
          printf("swdown = %.2f\n",(getcellswdown(input.climate,cell))[month]);
        }
        else
          printf("sun = %.2f\n",(getcellsun(input.climate,cell))[month]);
        if(config->prescribe_burntarea)
          printf("burntarea = %.2f \n",
              (getcellburntarea(input.climate,cell))[month]);
#endif
      }
    } /* of 'for(cell=...)' */
    foreachdayofmonth(dayofmonth,month)
    {
      for(cell=0;cell<config->ngridcell;cell++)
      {
#ifdef CHECK_BALANCE2
        int i;
        Real end=0;
        Stocks start={0,0};
        Stocks fluxes_in,fluxes_out;
        Real water_before=(grid[cell].discharge.dmass_lake+grid[cell].discharge.dmass_river)/grid[cell].coord.area;
        Real water_after=0;
        Real balanceW=0;
        Real wfluxes_old=grid[cell].balance.awater_flux+grid[cell].balance.atransp+grid[cell].balance.aevap+grid[cell].balance.ainterc+grid[cell].balance.aevap_lake+grid[cell].balance.aevap_res-grid[cell].balance.airrig-grid[cell].balance.aMT_water;
        Real exess_old=grid[cell].balance.excess_water+grid[cell].lateral_water;
        Real CH4_fluxes=(grid[cell].balance.aCH4_em+grid[cell].balance.aCH4_sink)*WC/WCH4;
        fluxes_in.carbon=grid[cell].balance.anpp+grid[cell].balance.flux_estab.carbon+grid[cell].balance.influx.carbon; //influxes
        fluxes_out.carbon=grid[cell].balance.arh+grid[cell].balance.fire.carbon+grid[cell].balance.flux_firewood.carbon+grid[cell].balance.neg_fluxes.carbon
            +grid[cell].balance.flux_harvest.carbon+grid[cell].balance.biomass_yield.carbon+grid[cell].balance.deforest_emissions.carbon; //outfluxes
        fluxes_in.nitrogen=grid[cell].balance.flux_estab.nitrogen+grid[cell].balance.influx.nitrogen; //influxes
        fluxes_out.nitrogen=grid[cell].balance.fire.nitrogen+grid[cell].balance.flux_firewood.nitrogen+grid[cell].balance.n_outflux+grid[cell].balance.neg_fluxes.nitrogen
            +grid[cell].balance.flux_harvest.nitrogen+grid[cell].balance.biomass_yield.nitrogen+grid[cell].balance.deforest_emissions.nitrogen; //outfluxes
        foreachstand(stand,s,grid[cell].standlist)
        {
          water_before+=soilwater(&stand->soil)*stand->frac;
          start.carbon+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
          start.nitrogen+=standstocks(stand).nitrogen*stand->frac;
        }
        start.carbon+=grid[cell].ml.product.fast.carbon+grid[cell].ml.product.slow.carbon+
            grid[cell].balance.estab_storage_grass[0].carbon+grid[cell].balance.estab_storage_tree[0].carbon+grid[cell].balance.estab_storage_grass[1].carbon+grid[cell].balance.estab_storage_tree[1].carbon;
        start.nitrogen+=grid[cell].ml.product.fast.nitrogen+grid[cell].ml.product.slow.nitrogen+
            grid[cell].balance.estab_storage_grass[0].nitrogen+grid[cell].balance.estab_storage_tree[0].nitrogen+grid[cell].balance.estab_storage_grass[1].nitrogen+grid[cell].balance.estab_storage_tree[1].nitrogen;
        if(grid[cell].ml.dam)
        {
          water_before+=grid[cell].ml.resdata->dmass/grid[cell].coord.area;/*+grid[cell].resdata->dfout_irrigation/grid[cell].coord.area; */
          for(i=0;i<NIRRIGDAYS;i++)
            water_before+=grid[cell].ml.resdata->dfout_irrigation_daily[i]/grid[cell].coord.area;
        }

#endif
        if(!grid[cell].skip)
        {
          if(config->ispopulation)
            popdens=getpopdens(input.popdens,cell);
          if (config->isanomaly)
            grid[cell].icefrac = geticefrac(input.icefrac, cell);
          grid[cell].output.dcflux=grid[cell].output.bm_inc=0;
          initoutputdata(&((grid+cell)->output),DAILY,year,config);
          /* get daily values for temperature, precipitation and sunshine */
          dailyclimate(&daily,input.climate,&grid[cell].climbuf,cell,day,
              month,dayofmonth);
#ifdef SAFE
          if(degCtoK(daily.temp)<0)
          {
            if(degCtoK(daily.temp)<(-0.2)) /* avoid precision errors: only fail if values are more negative than -0.2 */
              fail(INVALID_CLIMATE_ERR,TRUE,FALSE,"Temperature=%g K less than zero for cell %d at day %d",degCtoK(daily.temp),cell+config->startgrid,day);
            daily.temp=-273.15;
          }
          if(config->with_radiation)
          {
            if(daily.swdown<0)
            {
              if(!config->isanomaly)
                fail(INVALID_CLIMATE_ERR,TRUE,FALSE,"Short wave radiation=%g W/m2 less than zero for cell %d at day %d",daily.swdown,cell+config->startgrid,day);
              daily.swdown=0;
            }
          }
          else
          {
            if(daily.sun<-1e-5 || daily.sun>100)
              fail(INVALID_CLIMATE_ERR,TRUE,FALSE,"Cloudiness=%g%% not in [0,100] for cell %d at day %d",daily.sun,cell+config->startgrid,day);
            getoutput(&grid[cell].output,SUN,config)+=daily.sun;
          }
          if(config->with_nitrogen && daily.windspeed<0)
            fail(INVALID_CLIMATE_ERR,TRUE,FALSE,"Wind speed=%g less than zero for cell %d at day %d",daily.windspeed,cell+config->startgrid,day);
#endif
          if(config->with_radiation==CLOUDINESS && daily.sun<0)
            daily.sun=0;
          /* get daily values for temperature, precipitation and sunshine */
          getoutput(&grid[cell].output,TEMP,config)+=daily.temp;
          grid[cell].balance.temp+=daily.temp;
          getoutput(&grid[cell].output,PREC,config)+=daily.prec;

#ifdef DEBUG
          printf("day=%d cell=%d\n",day,cell);
          fflush(stdout);
#endif
          update_daily(grid+cell,co2,ch4,popdens,daily,day,npft,
              ncft,year,month,intercrop,config);
        }
#ifdef CHECK_BALANCE2
        fluxes_out.carbon=(grid[cell].balance.arh+grid[cell].balance.fire.carbon+grid[cell].balance.flux_firewood.carbon+grid[cell].balance.neg_fluxes.carbon
            +grid[cell].balance.flux_harvest.carbon+grid[cell].balance.biomass_yield.carbon+grid[cell].balance.deforest_emissions.carbon)-fluxes_out.carbon; //outfluxes
        fluxes_in.carbon=(grid[cell].balance.anpp+grid[cell].balance.flux_estab.carbon+grid[cell].balance.influx.carbon)-fluxes_in.carbon;
        water_after=(grid[cell].discharge.dmass_lake+grid[cell].discharge.dmass_river)/grid[cell].coord.area;
        CH4_fluxes-=(grid[cell].balance.aCH4_em+grid[cell].balance.aCH4_sink)*WC/WCH4;                                 //will be negative, because emissions at the end are higher, thus we have to substract
        end=0;
        foreachstand(stand,s,grid[cell].standlist)
        {
          water_after+=soilwater(&stand->soil)*stand->frac;
          end+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac ;
        }
        end+=grid[cell].ml.product.fast.carbon+grid[cell].ml.product.slow.carbon+
            grid[cell].balance.estab_storage_grass[0].carbon+grid[cell].balance.estab_storage_tree[0].carbon+grid[cell].balance.estab_storage_grass[1].carbon+grid[cell].balance.estab_storage_tree[1].carbon;
        if(grid[cell].ml.dam)
        {
          water_after+=grid[cell].ml.resdata->dmass/grid[cell].coord.area;/*+grid[cell].resdata->dfout_irrigation/grid[cell].coord.area; */
          for(i=0;i<NIRRIGDAYS;i++)
            water_after+=grid[cell].ml.resdata->dfout_irrigation_daily[i]/grid[cell].coord.area;
        }

        balanceW=water_after-water_before-daily.prec+
            ((grid[cell].balance.awater_flux+grid[cell].balance.atransp+grid[cell].balance.aevap+grid[cell].balance.ainterc+grid[cell].balance.aevap_lake+grid[cell].balance.aevap_res-grid[cell].balance.airrig-grid[cell].balance.aMT_water)-wfluxes_old)
            +((grid[cell].balance.excess_water+grid[cell].lateral_water)-exess_old);
        if (fabs(end-start.carbon-CH4_fluxes+fluxes_out.carbon-fluxes_in.carbon)>0.001)
        {
          fprintf(stderr, "C_ERROR-iterateyear after update_annual: day: %d    %g start: %g  end: %g CH4_fluxes: %g flux_estab.carbon: %g flux_harvest.carbon: %g dcflux: %g fluxes_in.carbon: %g "
              "fluxes_out.carbon: %g neg_fluxes: %g bm_inc: %g rh: %g aCH4_sink: %g aCH4_em : %g \n",
              day,end-start.carbon-CH4_fluxes-fluxes_in.carbon+fluxes_out.carbon,start.carbon,end,CH4_fluxes,grid[cell].balance.flux_estab.carbon,grid[cell].balance.flux_harvest.carbon,
              grid[cell].output.dcflux, fluxes_in.carbon,fluxes_out.carbon, grid[cell].balance.neg_fluxes.carbon,grid[cell].output.bm_inc,grid[cell].balance.arh,grid[cell].balance.aCH4_sink*WC/WCH4,grid[cell].balance.aCH4_em*WC/WCH4);
          //    foreachstand(stand,s,grid[cell].standlist)
          //             fprintf(stderr,"update_daily: standfrac: %g standtype: %s s= %d iswetland: %d cropfraction_rf: %g cropfraction_irr: %g grasfrac_rf: %g grasfrac_irr: %g\n",
          //                     stand->frac, stand->type->name,s,stand->soil.iswetland, crop_sum_frac(grid[cell].ml.landfrac,12,config->nagtree,grid[cell].ml.reservoirfrac+grid[cell].lakefrac,FALSE),
          //                     cr.carbonop_sum_frac(grid[cell].ml.landfrac,12,config->nagtree,grid[cell].ml.reservoirfrac+grid[cell].lakefrac,TRUE),
          //                     grid[cell].ml.landfrac[0].grass[0]+grid[cell].ml.landfrac[0].grass[1],grid[cell].ml.landfrac[1].grass[0]+grid[cell].ml.landfrac[1].grass[1]);
        }
        fluxes_out.nitrogen=(grid[cell].balance.fire.nitrogen+grid[cell].balance.flux_firewood.nitrogen+grid[cell].balance.n_outflux+grid[cell].balance.neg_fluxes.nitrogen
            +grid[cell].balance.flux_harvest.nitrogen+grid[cell].balance.biomass_yield.nitrogen+grid[cell].balance.deforest_emissions.nitrogen)-fluxes_out.nitrogen;
        fluxes_in.nitrogen=(grid[cell].balance.flux_estab.nitrogen+grid[cell].balance.influx.nitrogen)-fluxes_in.nitrogen;
        end=0;
        foreachstand(stand,s,grid[cell].standlist)
        end+=standstocks(stand).nitrogen*stand->frac;
        end+=grid[cell].ml.product.fast.nitrogen+grid[cell].ml.product.slow.nitrogen+
            grid[cell].balance.estab_storage_grass[0].nitrogen+grid[cell].balance.estab_storage_tree[0].nitrogen+grid[cell].balance.estab_storage_grass[1].nitrogen+grid[cell].balance.estab_storage_tree[1].nitrogen;
        if (fabs(end-start.nitrogen+fluxes_out.nitrogen-fluxes_in.nitrogen)>0.005)
        {
          fprintf(stderr, "N_ERROR-iterateyear after update_daily: day: %d    %g start: %g  end: %g flux_estab.nitrogen: %g flux_harvest.nitrogen: %g "
              "influx: %g outflux: %g neg_fluxes: %g\n",
              day,end-start.nitrogen-fluxes_in.nitrogen+fluxes_out.nitrogen,start.nitrogen, end,grid[cell].balance.flux_estab.nitrogen,grid[cell].balance.flux_harvest.nitrogen,
              fluxes_in.nitrogen,fluxes_out.nitrogen, grid[cell].balance.neg_fluxes.nitrogen);
        }
        if(fabs(balanceW)>0.1)
          fprintf(stderr,"W-BALANCE-ERROR in iterateyear after update_daily: day %d balanceW: %g  exess_old: %g balance.excess_water: %g water_after: %g water_before: %g prec: %g"
              "atransp: %g  aevap %g ainterc %g aevap_lake  %g aevap_res: %g    airrig : %g aMT_water : %g flux_bal: %g runoff %g awater_flux %g lateral_water %g mfin-mfout : %g dmass_lake : %g  dmassriver : %g  \n",day,balanceW,exess_old,grid[cell].balance.excess_water,
              water_after,water_before,daily.prec,grid[cell].balance.atransp,grid[cell].balance.aevap,grid[cell].balance.ainterc,grid[cell].balance.aevap_lake,grid[cell].balance.aevap_res,grid[cell].balance.airrig,grid[cell].balance.aMT_water,
              ((grid[cell].balance.awater_flux+grid[cell].balance.atransp+grid[cell].balance.aevap+grid[cell].balance.ainterc+grid[cell].balance.aevap_lake+grid[cell].balance.aevap_res-grid[cell].balance.airrig-grid[cell].balance.aMT_water)-wfluxes_old),
              grid[cell].discharge.drunoff,grid[cell].balance.awater_flux,grid[cell].lateral_water,((grid[cell].discharge.mfout-grid[cell].discharge.mfin)/grid[cell].coord.area),grid[cell].discharge.dmass_lake/grid[cell].coord.area,grid[cell].discharge.dmass_river/grid[cell].coord.area);
#endif
      }

      if(config->river_routing)
      {
        if(config->withlanduse)
          withdrawal_demand(grid,config);
        if(config->extflow)
        {
          if(getextflow(input.extflow,grid,day-1,year))
            fail(INVALID_EXTFLOW_ERR,TRUE,FALSE,"Cannot read external flow data");
        }
        drain(grid,month,config);

        if(config->withlanduse)
          wateruse(grid,npft,ncft,month,config);
      }

      if(config->withdailyoutput && year>=config->outputyear)
        fwriteoutput(output,grid,year,day-1,DAILY,npft,ncft,config);

      day++;
    } /* of 'foreachdayofmonth */
    /* Calculate resdata->mdemand as sum of ddemand to reservoir, instead of the sum of evaporation deficits per cell*/
    for(cell=0;cell<config->ngridcell;cell++)
    {
      if(!grid[cell].skip)
        update_monthly(grid+cell,getmtemp(input.climate,&grid[cell].climbuf,
            cell,month),getmprec(input.climate,&grid[cell].climbuf,
                cell,month),month,config);
    } /* of 'for(cell=0;...)' */

    if(year>=config->outputyear)
      /* write out monthly output */
      fwriteoutput(output,grid,year,month,MONTHLY,npft,ncft,config);

  } /* of 'foreachmonth */

  for(cell=0;cell<config->ngridcell;cell++)
  {
#ifdef CHECK_BALANCE2
    int i;
    Real end=0;
    Stocks start={0,0};
    Stocks fluxes_in,fluxes_out;
    Real water_before=(grid[cell].discharge.dmass_lake+grid[cell].discharge.dmass_river)/grid[cell].coord.area;
    Real water_after=0;
    Real balanceW=0;
    Real wfluxes_old=grid[cell].balance.awater_flux+grid[cell].balance.atransp+grid[cell].balance.aevap+grid[cell].balance.ainterc+grid[cell].balance.aevap_lake+grid[cell].balance.aevap_res-grid[cell].balance.airrig-grid[cell].balance.aMT_water;
    Real exess_old=grid[cell].balance.excess_water+grid[cell].lateral_water;
    Real CH4_fluxes=(grid[cell].balance.aCH4_em+grid[cell].balance.aCH4_sink)*WC/WCH4;
    fluxes_in.carbon=grid[cell].balance.anpp+grid[cell].balance.flux_estab.carbon+grid[cell].balance.influx.carbon; //influxes
    fluxes_out.carbon=grid[cell].balance.arh+grid[cell].balance.fire.carbon+grid[cell].balance.flux_firewood.carbon+
        grid[cell].balance.neg_fluxes.carbon+grid[cell].balance.flux_harvest.carbon+grid[cell].balance.biomass_yield.carbon+grid[cell].balance.deforest_emissions.carbon+grid[cell].balance.prod_turnover.fast.carbon
        +grid[cell].balance.prod_turnover.slow.carbon+grid[cell].balance.trad_biofuel.carbon; //outfluxes
    fluxes_in.nitrogen=grid[cell].balance.flux_estab.nitrogen+grid[cell].balance.influx.nitrogen; //influxes

    fluxes_out.nitrogen=grid[cell].balance.fire.nitrogen+grid[cell].balance.flux_firewood.nitrogen+grid[cell].balance.n_outflux+grid[cell].balance.neg_fluxes.nitrogen
        +grid[cell].balance.flux_harvest.nitrogen+grid[cell].balance.biomass_yield.nitrogen+grid[cell].balance.deforest_emissions.nitrogen+grid[cell].balance.prod_turnover.fast.nitrogen
        +grid[cell].balance.prod_turnover.slow.nitrogen+grid[cell].balance.trad_biofuel.nitrogen; //outfluxes
    foreachstand(stand,s,grid[cell].standlist)
    {
      water_before+=soilwater(&stand->soil)*stand->frac;
      start.carbon+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac;
      start.nitrogen+=standstocks(stand).nitrogen*stand->frac;
    }
    start.carbon+=grid[cell].ml.product.fast.carbon+grid[cell].ml.product.slow.carbon+
        grid[cell].balance.estab_storage_grass[0].carbon+grid[cell].balance.estab_storage_tree[0].carbon+grid[cell].balance.estab_storage_grass[1].carbon+grid[cell].balance.estab_storage_tree[1].carbon;
    start.nitrogen+=grid[cell].ml.product.fast.nitrogen+grid[cell].ml.product.slow.nitrogen+
        grid[cell].balance.estab_storage_grass[0].nitrogen+grid[cell].balance.estab_storage_tree[0].nitrogen+grid[cell].balance.estab_storage_grass[1].nitrogen+grid[cell].balance.estab_storage_tree[1].nitrogen;
    if(grid[cell].ml.dam)
    {
      water_before+=grid[cell].ml.resdata->dmass/grid[cell].coord.area;/*+grid[cell].resdata->dfout_irrigation/grid[cell].coord.area; */
      for(i=0;i<NIRRIGDAYS;i++)
        water_before+=grid[cell].ml.resdata->dfout_irrigation_daily[i]/grid[cell].coord.area;
    }

#endif
    if(!grid[cell].skip)
    {
      grid[cell].landcover=(config->prescribe_landcover!=NO_LANDCOVER) ? getlandcover(input.landcover,cell) : NULL;
      update_annual(grid+cell,npft,ncft,popdens,year,daily.isdailytemp,intercrop,config);
#ifdef SAFE
      if(config->withlanduse) check_fluxes(grid+cell,year,cell,config);
#endif

#ifdef DEBUG
      if(year>config->firstyear)
      {
        printf("year=%d\n",year);
        printf("cell=%d\n",cell+config->startgrid);
        printcell(grid+cell,1,npft,ncft,config);
      }
#endif


      /*     if(config->nspinup>param.veg_equil_year &&
         (year==config->firstyear-config->nspinup+param.veg_equil_year))
        equilveg(grid+cell);
      //if(!config->from_restart)
        if(config->nspinup>soil_equil_year &&
           (year==config->firstyear-config->nspinup+cshift_year))
          equilsom(grid+cell,npft+ncft,config->pftpar,TRUE);
      if(config->equilsoil)
      {
         if(config->nspinup>soil_equil_year &&
            (year==config->firstyear-config->nspinup+soil_equil_year) && !config->from_restart)
           equilsom(grid+cell,npft+ncft,config->pftpar,FALSE);
      }*/
      if(config->equilsoil)
      {
        if((year-(config->firstyear-config->nspinup+param.veg_equil_year-param.equisoil_years))%param.equisoil_interval==0 && 
            (year-(config->firstyear-config->nspinup+param.veg_equil_year-param.equisoil_years))/param.equisoil_interval>=0 &&
            (year-(config->firstyear-config->nspinup+param.veg_equil_year-param.equisoil_years))/param.equisoil_interval<param.nequilsoil)
          equilveg(grid+cell,npft+ncft);

        if(year==(config->firstyear-config->nspinup+param.veg_equil_year))
          equilsom(grid+cell,npft+ncft,config->pftpar,TRUE);

        if((year-(config->firstyear-config->nspinup+param.veg_equil_year))%param.equisoil_interval==0 && 
            (year-(config->firstyear-config->nspinup+param.veg_equil_year))/param.equisoil_interval>0 &&
            (year-(config->firstyear-config->nspinup+param.veg_equil_year))/param.equisoil_interval<param.nequilsoil)
          equilsom(grid+cell,npft+ncft,config->pftpar,FALSE);

        if(param.equisoil_fadeout>0)
        {
          if(year==(config->firstyear-config->nspinup+param.veg_equil_year+param.equisoil_interval*param.nequilsoil))
            equilveg(grid+cell,npft+ncft);

          if(year==(config->firstyear-config->nspinup+param.veg_equil_year+param.equisoil_interval*param.nequilsoil+param.equisoil_fadeout))
            equilsom(grid+cell,npft+ncft,config->pftpar,FALSE);
        }
      }

      if(config->withlanduse)
      {
        getnsoil_agr(&norg_soil_agr,&nmin_soil_agr,&nveg_soil_agr,grid+cell);
        getoutput(&grid[cell].output,DELTA_NORG_SOIL_AGR,config)+=norg_soil_agr;
        getoutput(&grid[cell].output,DELTA_NMIN_SOIL_AGR,config)+=nmin_soil_agr;
        getoutput(&grid[cell].output,DELTA_NVEG_SOIL_AGR,config)+=nveg_soil_agr;
        foreachstand(stand,s,(grid+cell)->standlist)
        if(stand->type->landusetype==GRASSLAND)
          getoutput(&grid[cell].output,DELTAC_MGRASS,config)+=standstocks(stand).carbon*stand->frac;
      }
    }
    if(config->river_routing)
    {
#ifdef IMAGE
      grid[cell].balance.surface_storage = grid[cell].discharge.dmass_lake + grid[cell].discharge.dmass_river + grid[cell].discharge.dmass_gw;
#else
      grid[cell].balance.surface_storage=grid[cell].discharge.dmass_lake+grid[cell].discharge.dmass_river;
#endif
      if(grid[cell].ml.dam)
        grid[cell].balance.surface_storage+=reservoir_surface_storage(grid[cell].ml.resdata);
    }
#ifdef CHECK_BALANCE2
    fluxes_out.carbon=(grid[cell].balance.arh+grid[cell].balance.fire.carbon+grid[cell].balance.flux_firewood.carbon+grid[cell].balance.neg_fluxes.carbon
        +grid[cell].balance.flux_harvest.carbon+grid[cell].balance.biomass_yield.carbon+grid[cell].balance.deforest_emissions.carbon+grid[cell].balance.prod_turnover.fast.carbon
        +grid[cell].balance.prod_turnover.slow.carbon+grid[cell].balance.trad_biofuel.nitrogen)-fluxes_out.carbon; //outfluxes
    fluxes_in.carbon=(grid[cell].balance.anpp+grid[cell].balance.flux_estab.carbon+grid[cell].balance.influx.carbon)-fluxes_in.carbon;

    fluxes_out.nitrogen=(grid[cell].balance.fire.nitrogen+grid[cell].balance.flux_firewood.nitrogen+grid[cell].balance.n_outflux+grid[cell].balance.neg_fluxes.nitrogen
        +grid[cell].balance.flux_harvest.nitrogen+grid[cell].balance.biomass_yield.nitrogen+grid[cell].balance.deforest_emissions.nitrogen+grid[cell].balance.prod_turnover.fast.nitrogen
        +grid[cell].balance.prod_turnover.slow.nitrogen+grid[cell].balance.trad_biofuel.nitrogen)-fluxes_out.nitrogen;
    fluxes_in.nitrogen=(grid[cell].balance.flux_estab.nitrogen+grid[cell].balance.influx.nitrogen)-fluxes_in.nitrogen;
    water_after=(grid[cell].discharge.dmass_lake+grid[cell].discharge.dmass_river)/grid[cell].coord.area;
    CH4_fluxes-=(grid[cell].balance.aCH4_em+grid[cell].balance.aCH4_sink)*WC/WCH4;                                 //will be negative, because emissions at the end are higher, thus we have to substract
    end=0;
    foreachstand(stand,s,grid[cell].standlist)
    {
      water_after+=soilwater(&stand->soil)*stand->frac;
      end+=(standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4)*stand->frac ;
    }
    end+=grid[cell].ml.product.fast.carbon+grid[cell].ml.product.slow.carbon+
        grid[cell].balance.estab_storage_grass[0].carbon+grid[cell].balance.estab_storage_tree[0].carbon+grid[cell].balance.estab_storage_grass[1].carbon+grid[cell].balance.estab_storage_tree[1].carbon;
    if(grid[cell].ml.dam)
    {
      water_after+=grid[cell].ml.resdata->dmass/grid[cell].coord.area;/*+grid[cell].resdata->dfout_irrigation/grid[cell].coord.area; */
      for(i=0;i<NIRRIGDAYS;i++)
        water_after+=grid[cell].ml.resdata->dfout_irrigation_daily[i]/grid[cell].coord.area;
    }

    balanceW=water_after-water_before+
        (grid[cell].balance.awater_flux-wfluxes_old)                                    // other fluxes added to awater_flux in check_fluxes
        +((grid[cell].balance.excess_water+grid[cell].lateral_water)-exess_old);
    if (fabs(end-start.carbon-CH4_fluxes+fluxes_out.carbon-fluxes_in.carbon)>0.001)
    {
      fprintf(stderr, "C_ERROR-iterateyear after update_annual: day: %d    %g start: %g  end: %g CH4_fluxes: %g flux_estab.carbon: %g flux_harvest.carbon: %g dcflux: %g fluxes_in.carbon: %g "
          "fluxes_out.carbon: %g neg_fluxes: %g bm_inc: %g rh: %g aCH4_sink: %g aCH4_em : %g \n",
          day,end-start.carbon-CH4_fluxes-fluxes_in.carbon+fluxes_out.carbon,start.carbon,end,CH4_fluxes,grid[cell].balance.flux_estab.carbon,grid[cell].balance.flux_harvest.carbon,
          grid[cell].output.dcflux, fluxes_in.carbon,fluxes_out.carbon, grid[cell].balance.neg_fluxes.carbon,grid[cell].output.bm_inc,grid[cell].balance.arh,grid[cell].balance.aCH4_sink*WC/WCH4,grid[cell].balance.aCH4_em*WC/WCH4);
      //    foreachstand(stand,s,grid[cell].standlist)
      //             fprintf(stderr,"update_daily: standfrac: %g standtype: %s s= %d iswetland: %d cropfraction_rf: %g cropfraction_irr: %g grasfrac_rf: %g grasfrac_irr: %g\n",
      //                     stand->frac, stand->type->name,s,stand->soil.iswetland, crop_sum_frac(grid[cell].ml.landfrac,12,config->nagtree,grid[cell].ml.reservoirfrac+grid[cell].lakefrac,FALSE),
      //                     cr.carbonop_sum_frac(grid[cell].ml.landfrac,12,config->nagtree,grid[cell].ml.reservoirfrac+grid[cell].lakefrac,TRUE),
      //                     grid[cell].ml.landfrac[0].grass[0]+grid[cell].ml.landfrac[0].grass[1],grid[cell].ml.landfrac[1].grass[0]+grid[cell].ml.landfrac[1].grass[1]);
    }
    end=0;
    foreachstand(stand,s,grid[cell].standlist)
    end+=standstocks(stand).nitrogen*stand->frac;
    end+=grid[cell].ml.product.fast.nitrogen+grid[cell].ml.product.slow.nitrogen+
        grid[cell].balance.estab_storage_grass[0].nitrogen+grid[cell].balance.estab_storage_tree[0].nitrogen+grid[cell].balance.estab_storage_grass[1].nitrogen+grid[cell].balance.estab_storage_tree[1].nitrogen;
    if (fabs(end-start.nitrogen+fluxes_out.nitrogen-fluxes_in.nitrogen)>0.001)
    {
      fprintf(stderr, "N_ERROR-iterateyear after update_annual: day: %d    %g start: %g  end: %g flux_estab.nitrogen: %g flux_harvest.nitrogen: %g "
          "influx: %g outflux: %g neg_fluxes: %g\n",
          day,end-start.nitrogen-fluxes_in.nitrogen+fluxes_out.nitrogen,start.nitrogen, end,grid[cell].balance.flux_estab.nitrogen,grid[cell].balance.flux_harvest.nitrogen,
          fluxes_in.nitrogen,fluxes_out.nitrogen, grid[cell].balance.neg_fluxes.nitrogen);
    }
    if(fabs(balanceW)>0.001)
      fprintf(stderr,"W-BALANCE-ERROR in iterateyear after update_annual: day %d balanceW: %g  wfluxes_old: %g exess_old: %g balance.excess_water: %g water_after: %g water_before: %g"
          "atransp: %g  aevap %g ainterc %g aevap_lake  %g aevap_res: %g    airrig : %g aMT_water : %g flux_bal: %g runoff %g awater_flux %g lateral_water %g mfin-mfout : %g dmass_lake : %g  dmassriver : %g  \n",day,balanceW,wfluxes_old,exess_old,grid[cell].balance.excess_water,
          water_after,water_before,grid[cell].balance.atransp,grid[cell].balance.aevap,grid[cell].balance.ainterc,grid[cell].balance.aevap_lake,grid[cell].balance.aevap_res,grid[cell].balance.airrig,grid[cell].balance.aMT_water,
          ((grid[cell].balance.awater_flux+grid[cell].balance.atransp+grid[cell].balance.aevap+grid[cell].balance.ainterc+grid[cell].balance.aevap_lake+grid[cell].balance.aevap_res-grid[cell].balance.airrig-grid[cell].balance.aMT_water)-wfluxes_old),
          grid[cell].discharge.drunoff,grid[cell].balance.awater_flux,grid[cell].lateral_water,((grid[cell].discharge.mfout-grid[cell].discharge.mfin)/grid[cell].coord.area),grid[cell].discharge.dmass_lake/grid[cell].coord.area,grid[cell].discharge.dmass_river/grid[cell].coord.area);
#endif
  } /* of for(cell=0,...) */

  fwriteoutput_ch4(output,ch4,co2,config);
  if(year>=config->outputyear)
  {
    /* write out annual output */
    fwriteoutput(output,grid,year,0,ANNUAL,npft,ncft,config);
  }
} /* of 'iterateyear' */

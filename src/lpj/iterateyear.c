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
                 int npft,            /**< number of natural PFTs */
                 int ncft,            /**< number of crop PFTs */
                 int year,            /**< simulation year (AD) */
                 const Config *config /**< LPJ configuration */
                )
{
  Dailyclimate daily;
  Bool intercrop;
  int month,dayofmonth,day;
  int cell;
  Real popdens=0; /* population density (capita/km2) */
  intercrop=getintercrop(input.landuse);
  for(cell=0;cell<config->ngridcell;cell++)
  {
#ifdef IMAGE
    grid[cell].output.ydischarge=0;
#endif
    grid[cell].output.adischarge=0;
    grid[cell].output.surface_storage=0;
    if(!grid[cell].skip)
    {
      init_annual(grid+cell,npft,ncft,config);
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
            landusechange_for_reservoir(grid+cell,config->pftpar,npft,config->istimber,
                                        intercrop,ncft,year);
        }
#if defined IMAGE && defined COUPLED
        setoutput_image(grid+cell,ncft);
#endif
      }
      initgdd(grid[cell].gdd,npft);
    } /*gridcell skipped*/
  } /* of for(cell=...) */

  day=1;
  foreachmonth(month)
  {
    for(cell=0;cell<config->ngridcell;cell++)
    {
      grid[cell].discharge.mfin=grid[cell].discharge.mfout=grid[cell].output.mdischarge=grid[cell].output.mwateramount=grid[cell].ml.mdemand=0.0;
      if(!grid[cell].skip)
      {
        initoutput_monthly(&grid[cell].output,npft,config->nbiomass,config->nwft,ncft);
        initclimate_monthly(input.climate,&grid[cell].climbuf,cell,month,grid[cell].seed);

#if defined IMAGE && defined COUPLED
        monthlyoutput_image(&grid[cell].output,input.climate,cell,month);
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
        if(!grid[cell].skip)
        {
          if(config->ispopulation)
            popdens=getpopdens(input.popdens,cell);
          grid[cell].output.dcflux=0;
          initoutput_daily(&(grid[cell].output.daily));
          /* get daily values for temperature, precipitation and sunshine */
          dailyclimate(&daily,input.climate,&grid[cell].climbuf,cell,day,
                       month,dayofmonth);
#ifdef SAFE
          if(degCtoK(daily.temp)<0)
            fail(INVALID_CLIMATE_ERR,FALSE,"Temperature=%g K less than zero for cell %d at day %d",degCtoK(daily.temp),cell+config->startgrid,day);
          if(config->with_radiation)
          {
            if(daily.swdown<0)
              fail(INVALID_CLIMATE_ERR,FALSE,"Short wave radiation=%g W/m2 less than zero for cell %d at day %d",daily.swdown,cell+config->startgrid,day);
          }
          else
          {
            if(daily.sun<0 || daily.sun>100)
              fail(INVALID_CLIMATE_ERR,FALSE,"Cloudiness=%g%% not in [0,100] for cell %d at day %d",daily.sun,cell+config->startgrid,day);
          }
          if(config->with_nitrogen && daily.windspeed<0)
            fail(INVALID_CLIMATE_ERR,FALSE,"Wind speed=%g less than zero for cell %d at day %d",daily.windspeed,cell+config->startgrid,day);
#endif
          /* get daily values for temperature, precipitation and sunshine */
          grid[cell].output.daily.temp=daily.temp;
          grid[cell].output.daily.prec=daily.prec;
          grid[cell].output.daily.sun=daily.sun;

#ifdef DEBUG
          printf("day=%d cell=%d\n",day,cell);
          fflush(stdout);
#endif
          update_daily(grid+cell,co2,popdens,daily,day,npft,
                       ncft,year,month,intercrop,config);
        }
      }

      if(config->river_routing)
      {
        if(config->withlanduse)
          withdrawal_demand(grid,config);

        drain(grid,month,config);

        if(config->withlanduse)
          wateruse(grid,npft,ncft,config);
      }
      if(config->withdailyoutput && year>=config->outputyear)
        fwriteoutput_daily(output,grid,day-1,year,config);

      day++;
    } /* of 'foreachdayofmonth */
    /* Calculate resdata->mdemand as sum of ddemand to reservoir, instead of the sum of evaporation deficits per cell*/
    for(cell=0;cell<config->ngridcell;cell++)
    {
      if(config->river_routing)
      {
#ifdef IMAGE
        grid[cell].output.ydischarge += grid[cell].output.mdischarge;
#endif
        if(grid[cell].discharge.next<0)
          grid[cell].output.adischarge+=grid[cell].output.mdischarge;           /* only endcell outflow */
        grid[cell].output.mdischarge*=1e-9;                    /* monthly mean discharge per month in 1.000.000 m3 per cell */
        grid[cell].output.mres_storage*=1e-9*ndaymonth1[month];                  /* mean monthly reservoir storage in 1.000.000 m3 per cell */
        grid[cell].output.mwateramount*=1e-9*ndaymonth1[month];                  /* mean wateramount per month in 1.000.000 m3 per cell */
      }
      if(!grid[cell].skip)
        update_monthly(grid+cell,getmtemp(input.climate,&grid[cell].climbuf,
                       cell,month),getmprec(input.climate,&grid[cell].climbuf,
                       cell,month),npft,config->nbiomass,config->nwft,ncft,month);
    } /* of 'for(cell=0;...)' */

    if(year>=config->outputyear)
      /* write out monthly output */
      fwriteoutput_monthly(output,grid,npft,ncft,month,year,config);

  } /* of 'foreachmonth */

  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(!grid[cell].skip)
    {
      grid[cell].landcover=(config->prescribe_landcover!=NO_LANDCOVER) ? getlandcover(input.landcover,cell) : NULL;
      update_annual(grid+cell,npft,ncft,popdens,year,daily.isdailytemp,intercrop,config);
#ifdef SAFE
      check_fluxes(grid+cell,year,cell,config);
#endif

#ifdef DEBUG
      if(year>config->firstyear)
      {
        printf("year=%d\n",year);
        printf("cell=%d\n",cell+config->startgrid);
        printcell(grid+cell,1,npft,ncft,config);
      }
#endif

      if(config->equilsoil)
      {
        if(config->nspinup>veg_equil_year &&
           (year==config->firstyear-config->nspinup+veg_equil_year) && !config->from_restart)
          equilveg(grid+cell);

        if(config->nspinup>soil_equil_year &&
           (year==config->firstyear-config->nspinup+cshift_year) && !config->from_restart)
          equilsom(grid+cell,npft+ncft,config->pftpar,TRUE);

        if(config->nspinup>soil_equil_year &&
           (year==config->firstyear-config->nspinup+120) && !config->from_restart)
          equilsom(grid+cell,npft+ncft,config->pftpar,TRUE);

        if(config->nspinup>soil_equil_year &&
           (year==config->firstyear-config->nspinup+soil_equil_year) && !config->from_restart)
          equilsom(grid+cell,npft+ncft,config->pftpar,FALSE);
      }
    }
    if(config->river_routing)
    {
#ifdef IMAGE
      grid[cell].output.surface_storage = grid[cell].discharge.dmass_lake + grid[cell].discharge.dmass_river + grid[cell].discharge.dmass_gw;
#else
      grid[cell].output.surface_storage=grid[cell].discharge.dmass_lake+grid[cell].discharge.dmass_river;
#endif
      if(grid[cell].ml.dam)
        grid[cell].output.surface_storage+=reservoir_surface_storage(grid[cell].ml.resdata);
    }
  } /* of for(cell=0,...) */

  if(year>=config->outputyear)
  {
    /* write out annual output */
    fwriteoutput_annual(output,grid,year,config);
    fwriteoutput_pft(output,grid,npft,ncft,year,config);
  }
} /* of 'iterateyear' */

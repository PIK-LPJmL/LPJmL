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
  Bool intercrop,istimber;
  int month,dayofmonth,day;
  int cell;
  Real popdens=0; /* population density (capita/km2) */
#ifdef IMAGE
  istimber=(config->start_imagecoupling!=INT_MAX);
#else
  istimber=FALSE;
#endif
  intercrop=getintercrop(input.landuse);
  for(cell=0;cell<config->ngridcell;cell++)
  {
    grid[cell].output.adischarge=0;
    grid[cell].output.surface_storage=0;
    if(!grid[cell].skip)
    {
      init_annual(grid+cell,npft,config->nbiomass,ncft);
      if(input.landuse!=NULL)
      {
        if(grid[cell].lakefrac<1)
        {
          /* calculate landuse change */
          if(!config->isconstlai)
            laimax_manage(&grid[cell].ml.manage,config->pftpar+npft,npft,ncft,year);
          if(year>config->firstyear-config->nspinup)
            landusechange(grid+cell,config->pftpar,npft,ncft,config->ntypes,
                          intercrop,istimber,year,config->pft_output_scaled);
          else if(grid[cell].ml.dam)
            landusechange_for_reservoir(grid+cell,config->pftpar,npft,istimber,
                                        intercrop,ncft,year);
        }
#ifdef IMAGE
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
        initoutput_monthly(&((grid+cell)->output));
        /* Initialize random seed */
        if(israndomprec(input.climate))
          srand48(config->seed+(config->startgrid+cell)*year*month);
        initclimate_monthly(input.climate,&grid[cell].climbuf,cell,month);

#ifdef IMAGE
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
          /* get daily values for temperature, precipitation and sunshine */
          grid[cell].output.daily.temp=daily.temp;
          grid[cell].output.daily.prec=daily.prec;
          grid[cell].output.daily.sun=daily.sun;

#ifdef DEBUG
          printf("day=%d cell=%d\n",day,cell);
#endif
          update_daily(grid+cell,co2,popdens,daily,day,npft,
                       ncft,year,month,output->withdaily,intercrop,config);
        }
      }

      if(config->river_routing)
      {
        if(input.landuse!=NULL || input.wateruse!=NULL)
          withdrawal_demand(grid,config);

        drain(grid,month,config);

        if(input.landuse!=NULL || input.wateruse!=NULL)
          wateruse(grid,npft,ncft,config);
      }

      if(output->withdaily && year>=config->firstyear)
        fwriteoutput_daily(output,grid,day-1,year,config);

      day++;
    } /* of 'foreachdayofmonth */
    /* Calculate resdata->mdemand as sum of ddemand to reservoir, instead of the sum of evaporation deficits per cell*/
    for(cell=0;cell<config->ngridcell;cell++)
    {
      if(config->river_routing)
      {
        if(grid[cell].discharge.next<0)
          grid[cell].output.adischarge+=grid[cell].output.mdischarge;           /* only endcell outflow */
        grid[cell].output.mdischarge*=1e-9/ndaymonth[month];                    /* daily mean discharge per month in 1.000.000 m3 per cell */
        grid[cell].output.mres_storage*=1e-9/ndaymonth[month];                  /* mean monthly reservoir storage in 1.000.000 m3 per cell */
        grid[cell].output.mwateramount*=1e-9/ndaymonth[month];                  /* mean wateramount per month in 1.000.000 m3 per cell */
      }
      if(!grid[cell].skip)
        update_monthly(grid+cell,getmtemp(input.climate,&grid[cell].climbuf,
                       cell,month),getmprec(input.climate,&grid[cell].climbuf,
                       cell,month),month);
    } /* of 'for(cell=0;...)' */

#ifdef IMAGE
    if(year>=config->firstyear-istimber*10)
#else
    if(year>=config->firstyear)
#endif
      /* write out monthly output */
      fwriteoutput_monthly(output,grid,month,year,config);

  } /* of 'foreachmonth */

  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(!grid[cell].skip)
    {
      update_annual(grid+cell,npft,ncft,popdens,year,
                    (config->prescribe_landcover!=NO_LANDCOVER) ? getlandcover(input.landcover,cell) : NULL,daily.isdailytemp,intercrop,config);
#ifdef SAFE
      check_fluxes(grid+cell,year,cell,config);
#endif

#ifdef DEBUG
      if(year>config->firstyear)
      {
        printf("year=%d\n",year);
        printf("cell=%d\n",cell);
        printcell(grid+cell,1,ncft,input.landuse!=NULL,TRUE);
      }
#endif
      if(config->nspinup>veg_equil_year &&
         year==config->firstyear-config->nspinup+veg_equil_year && !config->from_restart)
        equilveg(grid+cell);

      if(config->nspinup>soil_equil_year &&
         year==config->firstyear-config->nspinup+soil_equil_year && !config->from_restart)
        equilsom(grid+cell,npft+ncft,config->pftpar);

    }
    if(config->river_routing)
    {
      grid[cell].output.surface_storage=grid[cell].discharge.dmass_lake+grid[cell].discharge.dmass_river;
      if(grid[cell].ml.dam)
        grid[cell].output.surface_storage+=reservoir_surface_storage(grid[cell].ml.resdata);
    }
  } /* of for(cell=0,...) */

#ifdef IMAGE
  if(year>=config->firstyear-istimber*10)
#else
  if(year>=config->firstyear)
#endif
  {
    /* write out annual output */
    fwriteoutput_annual(output,grid,year,config);
    fwriteoutput_pft(output,grid,npft,ncft,year,config);
  }
} /* of 'iterateyear_river' */

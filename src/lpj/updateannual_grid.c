/**************************************************************************************/
/**                                                                                \n**/
/**              u p d a t e a n n u a l _ g r i d . c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function performs necessary updates after iteration over one               \n**/
/**     year                                                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define isequilyear(year,config,equisoil_years) \
   ((year-(config->firstyear-config->nspinup+param.veg_equil_year-equisoil_years))%param.equisoil_interval==0 && \
   (year-(config->firstyear-config->nspinup+param.veg_equil_year-equisoil_years))/param.equisoil_interval>=0 &&  \
   (year-(config->firstyear-config->nspinup+param.veg_equil_year-equisoil_years))/param.equisoil_interval<param.nequilsoil)

void updateannual_grid(Outputfile *output,  /**< Output file data */
                       Cell grid[],         /**< cell array */
                       Landcover landcover, /**< pointer to landcover data */
                       Real co2,            /**< atmospheric CO2 (ppm) */
                       Real *ch4,           /**< CH4 (gC) */
                       Real *pch4,          /**< CH4 concentration (ppm) */
                       int year,            /**< simulation year (AD) */
                       int npft,            /**< number of natural PFTs */
                       int ncft,            /**< number of crop PFTs */
                       Bool intercrop,      /**< intercropping enabled (TRUE/FALSE) */
                       Bool isdailytemp,    /**< temperature data is daily data (TRUE/FALSE) */
                       const Config *config /**< LPJ configuration */
                      )
{
  Stand *stand;
  Real norg_soil_agr,nmin_soil_agr,nveg_soil_agr;
  Real cflux_total;
  Flux flux;
  int s,cell;
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(!grid[cell].skip)
    {
      grid[cell].landcover=(config->prescribe_landcover!=NO_LANDCOVER) ? getlandcover(landcover,cell) : NULL;
      update_annual_cell(grid+cell,npft,ncft,year,isdailytemp,intercrop,config);
#ifdef SAFE
      if(config->withlanduse)
        check_fluxes(grid+cell,year,cell,config);
      else if(year>(config->firstyear-config->nspinup+param.veg_equil_year+param.equisoil_interval*param.nequilsoil+param.equisoil_fadeout+1))
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
        if(isequilyear(year,config,param.equisoil_years))
          equilveg(grid+cell,npft+ncft);

        if(year==(config->firstyear-config->nspinup+param.veg_equil_year))
          equilsom(grid+cell,npft+ncft,config->pftpar,TRUE);

        if(isequilyear(year,config,0))
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
    } /* if(!grid[cell].skip) */
    if(config->river_routing)
    {
      grid[cell].balance.surface_storage = grid[cell].discharge.dmass_lake + grid[cell].discharge.dmass_river + grid[cell].discharge.dmass_gw+grid[cell].lateral_water*grid[cell].coord.area;
      if(grid[cell].ml.dam)
        grid[cell].balance.surface_storage+=reservoir_surface_storage(grid[cell].ml.resdata);
    }
  } /* of for(cell=0,...) */

  fwriteoutput_ch4(output,*pch4,co2,year,config);
  if(year>=config->outputyear)
  {
    /* write last monthly/daily output timestep after annual processes */
    if(fwriteoutput(output,grid,year,NMONTH-1,MONTHLY,npft,ncft,config))
    {
      if(isroot(*config))
        printfailerr(WRITE_OUTPUT_ERR,TRUE,"Cannot write output");
      exit(WRITE_OUTPUT_ERR);
    }
    if(config->withdailyoutput)
    {
      if(fwriteoutput(output,grid,year,NDAYYEAR-1,DAILY,npft,ncft,config))
      {
          if(isroot(*config))
            printfailerr(WRITE_OUTPUT_ERR,TRUE,"Cannot write output");
          exit(WRITE_OUTPUT_ERR);
        }
    }
    /* write out annual output */
    if(fwriteoutput(output,grid,year,0,ANNUAL,npft,ncft,config))
    {
      if(isroot(*config))
        printfailerr(WRITE_OUTPUT_ERR,TRUE,"Cannot write output");
      exit(WRITE_OUTPUT_ERR);
    }
  }
  if(year>=config->outputyear)
    closeoutput_yearly(output,config);
  /* calculating total carbon and water fluxes collected from all tasks */
  cflux_total=flux_sum(&flux,grid,config);
  if(isroot(*config))
  {
    /* output of total carbon flux and water on stdout on root task */
    printflux(flux,cflux_total,year,config);
    if(output->files[GLOBALFLUX].isopen)
      fprintcsvflux(output->files[GLOBALFLUX].fp.file,flux,cflux_total,
                    config->outnames[GLOBALFLUX].scale,year,config);
    if(output->files[GLOBALFLUX].issocket)
      send_flux_coupler(&flux,config->outnames[GLOBALFLUX].scale,year,config);
    fflush(stdout); /* force output to console */
    if (config->with_dynamic_ch4==DYNAMIC_CH4)
    {
      *ch4 += flux.CH4_emissions + flux.CH4_fire - 1 / tau_CH4* *ch4;
      *pch4 = *ch4*1e-15 / 2.123;
    }
#ifdef SAFE
    check_balance(flux,year,config);
#endif
  }
#ifdef USE_MPI
    if (config->with_dynamic_ch4==DYNAMIC_CH4)
      MPI_Bcast(pch4, sizeof(Real), MPI_BYTE, 0, config->comm);
#endif
  if(iswriterestart(config) && year==config->restartyear)
    fwriterestart(grid,npft,ncft,year,config->write_restart_filename,FALSE,config); /* write restart file */
} /* of 'updateannual_grid' */

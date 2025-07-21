/**************************************************************************************/
/**                                                                                \n**/
/**              s e t u p a n n u a l _ g r i d . c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool setupannual_grid(Outputfile *output,  /**< Output file data */
                      Cell grid[],         /**< cell array */
                      Input *input,        /**< input data */
                      int year,            /**< simulation year (AD) */
                      int npft,            /**< number of natural PFTs */
                      int ncft,            /**< number of crop PFTs */
                      Bool intercrop,      /**< intercropping enabled (TRUE/FALSE) */
                      const Config *config /**< LPJ configuration */
                     )                     /** \return TRUE on error */
{
  Stand *stand;
  Real norg_soil_agr,nmin_soil_agr,nveg_soil_agr;
  int cell,s,landuse_year,rc;
#ifndef COUPLED
  int wateruse_year;
#endif
  if(input->landuse!=NULL)
  {
    calc_seasonality(grid,npft,ncft,config);
    if(config->withlanduse==CONST_LANDUSE || config->withlanduse == ONLY_CROPS) /* constant landuse? */
      landuse_year=config->landuse_year_const;
    else if(config->fix_landuse && year>config->fix_landuse_year)
      landuse_year=config->fix_landuse_year;
    else
      landuse_year=year;
#ifndef COUPLED
    /* under constant landuse also keep wateruse at landuse_year_const */
    if(config->withlanduse==CONST_LANDUSE)
      wateruse_year=config->landuse_year_const;
    else if(config->fix_landuse && year>config->fix_landuse_year)
      wateruse_year=config->fix_landuse_year;
    else
      wateruse_year=year;
#endif
#if defined IMAGE && defined COUPLED
    if(year>=config->start_coupling)
    {
      if(receive_image_data(grid,npft,ncft,config))
      {
        fprintf(stderr,"ERROR104: Simulation stopped in receive_image_data().\n");
        fflush(stderr);
        return TRUE; /* leave time loop */
      }
    }
    else
#endif
    /* read landuse pattern from file */
    rc=getlanduse(input->landuse,grid,landuse_year,year,npft,ncft,config);
    if(iserror(rc,config))
    {
      if(isroot(*config))
      {
        fprintf(stderr,"ERROR104: Simulation stopped in getlanduse().\n");
        fflush(stderr);
      }
      return TRUE; /* leave time loop */
    }
    if(config->reservoir)
      allocate_reservoir(grid,year,config);
#ifndef COUPLED
    if(config->wateruse)
    {
      /* read wateruse data from file */
      rc=getwateruse(input->wateruse,grid,wateruse_year,config);
      if(iserror(rc,config))
      {
        if(isroot(*config))
        {
          fprintf(stderr,"ERROR104: Simulation stopped in getwateruse().\n");
          fflush(stderr);
        }
        return TRUE; /* leave time loop */
      }
    }
#ifdef IMAGE
    if (input->wateruse_wd!= NULL && input->landuse!=NULL)
    {
      /* read wateruse data from file */
      rc=getwateruse_wd(input->wateruse_wd, grid, wateruse_year, config);
      if(iserror(rc,config))
      {
        if(isroot(*config))
        {
          fprintf(stderr, "ERROR104: Simulation stopped in getwateruse_wd().\n");
          fflush(stderr);
        }
        return TRUE; /* leave time loop */
      }
    }
#endif
#endif
  }
  if(config->ispopulation)
  {
    rc=readpopdens(input->popdens,year,grid,config);
    if(iserror(rc,config))
    {
      if(isroot(*config))
      {
        fprintf(stderr,"ERROR104: Simulation stopped in readpopdens().\n");
        fflush(stderr);
      }
      return TRUE; /* leave time loop */
    }
  }
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    rc=gethumanignition(input->human_ignition,year,grid,config);
    if(iserror(rc,config))
    {
      if(isroot(*config))
      {
        fprintf(stderr,"ERROR104: Simulation stopped in gethumanignition().\n");
        fflush(stderr);
      }
      return TRUE; /* leave time loop */
    }
  }
  if (config->prescribe_landcover != NO_LANDCOVER)
  {
    rc=readlandcover(input->landcover,grid,year,config);
    if(iserror(rc,config))
    {
      if(isroot(*config))
      {
        fprintf(stderr,"ERROR104: Simulation stopped in readlandcover().\n");
        fflush(stderr);
      }
      return TRUE; /* leave time loop */
    }
  }
  if(year>=config->outputyear)
    openoutput_yearly(output,year,config);
  for(cell=0;cell<config->ngridcell;cell++)
  {
    initoutputdata(&grid[cell].output,ANNUAL,year,config);
    grid[cell].balance.surface_storage=grid[cell].balance.adischarge=0;
    grid[cell].discharge.afin_ext=0;
    if(!grid[cell].skip)
    {
      init_annual(grid+cell,ncft,config);
      if (config->with_glaciers)
      {   
        grid[cell].icefrac = geticefrac(input->icefrac, cell);
        grid[cell].is_glaciated=grid[cell].icefrac>=1-epsilon;
      }   
      if (grid[cell].is_glaciated)
        check_glaciated(grid + cell,config);
      else
      {
        if(config->withlanduse)
        {
          if(grid[cell].lakefrac<1)
          {
            /* calculate landuse change */
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
      grid[cell].was_glaciated=grid[cell].is_glaciated;
      initgdd(grid[cell].gdd,npft);
    } /*gridcell skipped*/
  } /* of for(cell=...) */
  return FALSE;
} /* of 'setupannual_grid' */

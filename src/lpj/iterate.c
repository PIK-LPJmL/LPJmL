/**************************************************************************************/
/**                                                                                \n**/
/**                      i  t  e  r  a  t  e  .  c                                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function iterate provides main time loop for LPJ                           \n**/
/**     In each year iterateyear/iterateyear_river is called                       \n**/
/**                                                                                \n**/
/**     Prinicpal structure                                                        \n**/
/**                                                                                \n**/
/**     for(year=...)                                                              \n**/
/**     {                                                                          \n**/
/**       co2=getco2();                                                            \n**/
/**       getclimate();                                                            \n**/
/**       if(landuse) getlanduse();                                                \n**/
/**       if(wateruse) getwateruse();                                              \n**/
/**       iterateyear();                                                           \n**/
/**       flux_sum();                                                              \n**/
/**       if(year==config->restartyear)                                            \n**/
/**         fwriterestart();                                                       \n**/
/**     }                                                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifndef _WIN32
#include <unistd.h>
#include <signal.h>
#endif

static Bool ischeckpoint;

static void handler(int UNUSED(num))
{
  ischeckpoint=TRUE; /* SIGTERM received, set global flag to TRUE */
} /* of 'handler' */

int iterate(Outputfile *output,  /**< Output file data */
            Cell grid[],         /**< cell grid array */
            Input input,         /**< input data: climate, land use, water use */
            int npft,            /**< Number of natural PFT's */
            int ncft,            /**< Number of crop PFT's */
            const Config *config /**< LPJ configuration data */
           )                     /** \return last year+1 on success */
{
  Real co2,cflux_total;
  Flux flux;
  int year,landuse_year,wateruse_year,startyear,firstspinupyear;
  Bool rc;

  firstspinupyear=(config->isfirstspinupyear) ?  config->firstspinupyear : input.climate->firstyear;
#ifdef STORECLIMATE
  Climatedata store,data_save;
  if(config->nspinup)
  {
    /* climate for the first nspinyear years is stored in memory
       to avoid reading repeatedly from disk */
    rc=storeclimate(&store,input.climate, grid,firstspinupyear,config->nspinyear,config);
    failonerror(config,rc,STORE_CLIMATE_ERR,"Storage of climate failed");

    data_save=input.climate->data;
  }
#endif
  if(config->initsoiltemp)
  {
    rc=initsoiltemp(input.climate,grid,config);
    failonerror(config,rc,INITSOILTEMP_ERR,"Initialization of soil temperature failed");
  }
  ischeckpoint=FALSE;
#ifndef _WIN32
  if(ischeckpointrestart(config)) 
    signal(SIGTERM,handler); /* enable checkpointing by setting signal handler */
#endif
  startyear=(config->ischeckpoint) ? config->checkpointyear+1 : config->firstyear-config->nspinup;
  /* main loop over spinup + simulation years  */
  if(isroot(*config) && config->ischeckpoint)
    printf("Starting from checkpoint file '%s'.\n",config->checkpoint_restart_filename);
  for(year=startyear;year<=config->lastyear;year++)
  {
#ifdef IMAGE
    if(year>=config->start_imagecoupling)
      co2=receive_image_co2(config);
    else
#endif
    co2=getco2(input.climate,year); /* get atmospheric CO2 concentration */
    if(year<input.climate->firstyear) /* are we in spinup phase? */
      /* yes, let climate data point to stored data */
#ifdef STORECLIMATE
      moveclimate(input.climate,&store,
                  (year-config->firstyear+config->nspinup) % config->nspinyear);
#else
      getclimate(input.climate,grid,firstspinupyear+(year-config->firstyear+config->nspinup) % config->nspinyear,config);
#endif
    else
    {
#ifdef STORECLIMATE
      if(year==input.climate->firstyear && config->nspinup)
      {
        /* restore climate data pointers to initial data */
        input.climate->data=data_save;
        freeclimatedata(&store); /* free data not used anymore */
      }
#endif
      /* read climate from files */
#ifdef IMAGE
      if(year>=config->start_imagecoupling)
      {
        if(receive_image_climate(input.climate,grid,year,config))
        {
          fprintf(stderr,"ERROR104: Simulation stopped in receive_image_climate().\n");
          fflush(stderr);
          break; /* leave time loop */
        }
      }
      else
#endif
      if(getclimate(input.climate,grid,year,config))
      {
        fprintf(stderr,"ERROR104: Simulation stopped in getclimate().\n");
        fflush(stderr);
        break; /* leave time loop */
      }
    }
    if(input.landuse!=NULL)
    {
      calc_seasonality(grid,npft,ncft,config);
      if(config->withlanduse==CONST_LANDUSE || config->withlanduse==ALL_CROPS) /* constant landuse? */
        landuse_year=config->landuse_year_const;
      else
        landuse_year=year;
      /* under constand landuse also keep wateruse at landuse_year_const */
      if(config->withlanduse==CONST_LANDUSE)
        wateruse_year=config->landuse_year_const;
      else
        wateruse_year=year;
#ifdef IMAGE
      if(year>=config->start_imagecoupling)
      {
        if(receive_image_data(grid,npft,ncft,config))
        {
          fprintf(stderr,"ERROR104: Simulation stopped in receive_image_data().\n");
          fflush(stderr);
          break; /* leave time loop */
        }
      }
      else
#endif
      /* read landuse pattern from file */
      if(getlanduse(input.landuse,grid,landuse_year,ncft,config))
      {
        fprintf(stderr,"ERROR104: Simulation stopped in getlanduse().\n");
        fflush(stderr);
        break; /* leave time loop */
      }
      if(config->reservoir)
        allocate_reservoir(grid,year,config);
    }
    if(input.wateruse!=NULL && input.landuse!=NULL)
    {
      /* read wateruse data from file */
      if(getwateruse(input.wateruse,grid,wateruse_year,config))
      {
        fprintf(stderr,"ERROR104: Simulation stopped in getwateruse().\n");
        fflush(stderr);
        break; /* leave time loop */
      }
    }
    if(config->ispopulation)
    {
      if(readpopdens(input.popdens,year,grid,config))
      {
        fprintf(stderr,"ERROR104: Simulation stopped in getpopdens().\n");
        fflush(stderr);
        break; /* leave time loop */
      }
    }
    if (config->prescribe_landcover != NO_LANDCOVER)
    {
      if (readlandcover(input.landcover,grid,year,config))
      {
        fprintf(stderr,"ERROR104: Simulation stopped in readlandcover().\n");
        fflush(stderr);
        break; /* leave time loop */
      }
    }
    /* perform iteration for one year */
    if(year>=config->outputyear)
      openoutput_yearly(output,year,config);
    iterateyear(output,grid,input,co2,npft,ncft,year,config);
    if(year>=config->outputyear)
      closeoutput_yearly(output,config);
    /* calculating total carbon and water fluxes collected from all tasks */
    cflux_total=flux_sum(&flux,grid,config);
    if(isroot(*config))
    {
      /* output of total carbon flux and water on stdout on root task */
      printflux(flux,cflux_total,year,config);
      if(output->method==LPJ_SOCKET && output->socket!=NULL &&
         year>=config->outputyear)
        output_flux(output,flux);
      fflush(stdout); /* force output to console */
#ifdef SAFE
      check_balance(flux,year,config);
#endif
    }
#ifdef IMAGE
    if(year>=config->start_imagecoupling)
    {
      /* send data to IMAGE */
#ifdef DEBUG_IMAGE
      if(isroot(*config))
      {
        printf("sending data to image? year %d startimagecoupling %d\n",
               year,config->start_imagecoupling);
        fflush(stdout);
      }
#endif
      if(send_image_data(grid,input.climate,npft,config))
        fail(SEND_IMAGE_ERR,FALSE,
             "Problem with writing maps for transfer to IMAGE");
    }
#endif
    if(iswriterestart(config) && year==config->restartyear)
      fwriterestart(grid,npft,ncft,year,config->write_restart_filename,config); /* write restart file */
    if(year<config->lastyear && ischeckpointrestart(config))
    {
#ifdef USE_MPI
      MPI_Bcast(&ischeckpoint,1,MPI_INT,0,config->comm);
#endif
      if(ischeckpoint)
      {
        if(isroot(*config))
          printf("SIGTERM catched, restart file '%s' written.\n",config->checkpoint_restart_filename);
        fwriterestart(grid,npft,ncft,year,config->checkpoint_restart_filename,config); /* write restart file */
        fcloseoutput(output,config);
#ifdef USE_MPI
        MPI_Finalize();
#endif
        exit(EXIT_SUCCESS);
      }
    }
  } /* of 'for(year=...)' */
#ifdef STORECLIMATE
  if(config->nspinup && (config->lastyear<input.climate->firstyear || year<input.climate->firstyear))
  {
    /* restore climate data pointers to initial data */
    input.climate->data=data_save;
    freeclimatedata(&store); /* free data not used anymore */
  }
#endif
  if(year>config->lastyear && config->ischeckpoint)
    unlink(config->checkpoint_restart_filename); /* delete checkpoint file */
  return year;
} /* of 'iterate' */

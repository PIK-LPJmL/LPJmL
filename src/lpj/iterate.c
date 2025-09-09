/**************************************************************************************/
/**                                                                                \n**/
/**                      i  t  e  r  a  t  e  .  c                                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function iterate provides main time loop for LPJ                           \n**/
/**     In each year iterateyear is called                                         \n**/
/**                                                                                \n**/
/**     Prinicpal structure                                                        \n**/
/**                                                                                \n**/
/**     for(year=...)                                                              \n**/
/**     {                                                                          \n**/
/**       co2=getco2();                                                            \n**/
/**       getclimate();                                                            \n**/
/**       iterateyear();                                                           \n**/
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

int iterate(Outputfile *output, /**< Output file data */
            Cell grid[],        /**< cell grid array */
            Input input,        /**< input data: climate, land use, water use */
            int npft,           /**< Number of natural PFTs */
            int ncft,           /**< Number of crop PFTs */
            Config *config      /**< LPJ configuration data */
           )                    /** \return last year+1 on success */
{
#ifdef USE_TIMING
  double t;
#endif
  Real co2;
  int year,startyear,firstspinupyear,spinup_year,climate_year,year_co2,depos_year;
  Bool rc;
  Climatedata store,data_save;


  firstspinupyear=(config->isfirstspinupyear) ?  config->firstspinupyear : input.climate->firstyear;
  if(isroot(*config) && config->nspinup && !config->isfirstspinupyear)
    printf("Spinup using climate starting from year %d\n",input.climate->firstyear);
  if(config->storeclimate && config->nspinup)
  {
    /* climate for the first nspinyear years is stored in memory
       to avoid reading repeatedly from disk */
#ifdef USE_TIMING
    timing_start(t);
#endif
    rc=storeclimate(&store,input.climate, grid,firstspinupyear,config->nspinyear,config);
#ifdef USE_TIMING
    timing_stop(STORECLIMATE_FCN,t);
#endif
    failonerror(config,rc,STORE_CLIMATE_ERR,"Storage of climate failed, re-run with \"store_climate\" : false setting");

    data_save=input.climate->data;
  }
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
#if defined IMAGE && defined COUPLED
    if(year>=config->start_coupling)
      co2=receive_image_co2(config);
    else
#endif
    {
      if(config->fix_co2 && year>config->fix_co2_year)
        year_co2=config->fix_co2_year;
      else
        year_co2=year;
      if(getco2(input.climate,&co2,year_co2,config)) /* get atmospheric CO2 concentration */
        break;
    }
    climate_year=year;
    if(year<input.climate->firstyear) /* are we in spinup phase? */
    {
      /* yes, let climate data point to stored data */
      if(config->shuffle_spinup_climate)
      {
        if(isroot(*config))
          spinup_year=(int)(erand48(config->seed)*config->nspinyear);
#ifdef USE_MPI
        MPI_Bcast(&spinup_year,1,MPI_INT,0,config->comm);
#endif
      }
      else
        spinup_year=(year-config->firstyear+config->nspinup) % config->nspinyear;
      if(config->storeclimate)
        moveclimate(input.climate,&store,spinup_year);
      else
      {
#ifdef USE_TIMING
        timing_start(t);
#endif
        getclimate(input.climate,grid,firstspinupyear+spinup_year,config);
#ifdef USE_TIMING
        timing_stop(GETCLIMATE_FCN,t);
#endif
      }
    }
    else
    {
      if(config->storeclimate && year==input.climate->firstyear && config->nspinup)
      {
        /* restore climate data pointers to initial data */
        input.climate->data=data_save;
        freeclimatedata(&store); /* free data not used anymore */
      }
      /* read climate from files */
#if defined IMAGE && defined COUPLED
      if(year>=config->start_coupling)
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
      {
        if(config->fix_climate && year>config->fix_climate_year)
        {
          if(config->fix_climate_shuffle)
          {
            if(isroot(*config))
              climate_year=config->fix_climate_interval[0]+(int)((config->fix_climate_interval[1]-config->fix_climate_interval[0]+1)*erand48(config->seed));
#ifdef USE_MPI
            MPI_Bcast(&climate_year,1,MPI_INT,0,config->comm);
#endif
          }
          else
            climate_year=config->fix_climate_interval[0]+(year-config->fix_climate_year) % (config->fix_climate_interval[1]-config->fix_climate_interval[0]+1);
        }

#ifdef USE_TIMING
        timing_start(t);
#endif
        rc=getclimate(input.climate,grid,climate_year,config);
#ifdef USE_TIMING
        timing_stop(GETCLIMATE_FCN,t);
#endif
        if(iserror(rc,config))
        {
          if(isroot(*config))
          {
            fprintf(stderr,"ERROR104: Simulation stopped in getclimate().\n");
            fflush(stderr);
          }
          break; /* leave time loop */
        }
      }
    }
    if(config->fix_deposition)
    {
      if(config->fix_deposition_with_climate)
        depos_year=climate_year;
      else if(year>config->fix_deposition_year)
      {
        if(config->fix_deposition_shuffle)
        {
          if(isroot(*config))
            depos_year=config->fix_deposition_interval[0]+(int)((config->fix_deposition_interval[1]-config->fix_deposition_interval[0]+1)*erand48(config->seed));
#ifdef USE_MPI
          MPI_Bcast(&depos_year,1,MPI_INT,0,config->comm);
#endif
        }
        else
          depos_year=config->fix_deposition_interval[0]+(year-config->fix_deposition_year) % (config->fix_deposition_interval[1]-config->fix_deposition_interval[0]+1);
      }
      else
        depos_year=year;
    }
    else
      depos_year=year;
    rc=getdeposition(input.climate,grid,depos_year,config);
    if(iserror(rc,config))
    {
      if(isroot(*config))
      {
        fprintf(stderr,"ERROR104: Simulation stopped in getdeposition().\n");
        fflush(stderr);
      }
      break; /* leave time loop */
    }
#ifdef USE_TIMING
    timing_start(t);
#endif
    rc=iterateyear(output,grid,input,co2,npft,ncft,year,config);
#ifdef USE_TIMING
    timing_stop(ITERATEYEAR_FCN,t);
#endif
    if(rc)
      break;
#if defined IMAGE && defined COUPLED
    if(year>=config->start_coupling)
    {
      /* send data to IMAGE */
#ifdef DEBUG_IMAGE
      if(isroot(*config))
      {
        printf("sending data to image? year %d startimagecoupling %d\n",
               year,config->start_coupling);
        fflush(stdout);
      }
#endif
      if(send_image_data(grid,input.climate,npft,ncft,config))
        fail(SEND_IMAGE_ERR,FALSE,
             "Problem with writing maps for transfer to IMAGE");
    }
#endif
    if(year<config->lastyear && ischeckpointrestart(config))
    {
#ifdef USE_MPI
      MPI_Bcast(&ischeckpoint,1,MPI_INT,0,config->comm);
#endif
      if(ischeckpoint)
      {
        if(isroot(*config))
          printf("SIGTERM catched, checkpoint file '%s' written.\n",config->checkpoint_restart_filename);
        fwriterestart(grid,npft,ncft,year,config->checkpoint_restart_filename,TRUE,config); /* write checkpoint file */
        fcloseoutput(output,config);
#ifdef USE_MPI
        MPI_Finalize();
#endif
        exit(EXIT_SUCCESS);
      }
    }
  } /* of 'for(year=...)' */
  if(config->storeclimate && config->nspinup && (config->lastyear<input.climate->firstyear || year<input.climate->firstyear))
  {
    /* restore climate data pointers to initial data */
    input.climate->data=data_save;
    freeclimatedata(&store); /* free data not used anymore */
  }
  if(year>config->lastyear && config->ischeckpoint)
    unlink(config->checkpoint_restart_filename); /* delete checkpoint file */

#if defined IMAGE && defined COUPLED
  /* wait for IMAGE to finish before closing TDT-connections by LPJ */
  if(config->sim_id==LPJML_IMAGE)
    receive_image_finish(config);
#endif
  return year;
} /* of 'iterate' */

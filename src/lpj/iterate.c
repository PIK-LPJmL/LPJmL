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
  Real co2, ch4, pch4;
  int year,startyear,firstspinupyear,spinup_year,climate_year,year_co2,index,data_index,rnd_year,depos_year;
#ifdef USE_TIMING
  double t;
#endif
  Bool rc;
  Climatedata store,data_save;
  if (config->isanomaly)
    data_index = (config->delta_year>1) ? 3 : 1;
  else
    data_index = 0;
  index=0;

  firstspinupyear=(config->isfirstspinupyear) ?  config->firstspinupyear : input.climate->firstyear;
  if(isroot(*config) && config->nspinup && !config->isfirstspinupyear)
    printf("Spinup using climate starting from year %d\n",input.climate->firstyear);
  if((config->storeclimate && config->nspinup) || (config->storeclimate && config->isanomaly))
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

    data_save=input.climate->data[data_index];
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
  pch4 = param.pch4*1e-3;
  ch4 = pch4*1e15*2.123; /* convert ppm to gC */
  if(isroot(*config) && config->ischeckpoint)
    printf("Starting from checkpoint file '%s'.\n",config->checkpoint_restart_filename);
  for(year=startyear;year<=config->lastyear;year++)
  {
#if defined IMAGE && defined COUPLED
    if(year>=config->start_coupling)
      co2=receive_image_co2(config);
    else
#endif

    if(config->fix_co2 && year>config->fix_co2_year)
      year_co2=config->fix_co2_year;
    else
      year_co2=year;
    if(getco2(input.climate,&co2,year_co2,config)) /* get atmospheric CO2 concentration */
      break;

    if(config->isanomaly)
      year_co2+=config->time_shift; //CLIMBER's year zero= year 2000

    if(config->with_dynamic_ch4==PRESCRIBED_CH4)
    {
      if(getch4(input.climate,&ch4,year_co2,config)) /* get atmospheric CH4 concentration */
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR015: Invalid year %d in getch4().\n",year);
        break;
      }
      pch4=ch4*1e-3; /*convert to ppm*/
    }
    climate_year=year;
    if(year<input.climate->firstyear) /* are we in spinup phase? */
    {
      /* yes, let climate data point to stored data */
      if(config->shuffle_spinup_climate || config->isanomaly)
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
        moveclimate(input.climate,&store,data_index,spinup_year);
      else
      {
#ifdef USE_TIMING
        timing_start(t);
#endif
        getclimate(input.climate,grid,data_index,firstspinupyear+spinup_year,config);
#ifdef USE_TIMING
        timing_stop(GETCLIMATE_FCN,t);
#endif
      }
      if (config->isanomaly)
      {
#ifdef USE_TIMING
        t=mrun();
#endif
        getclimate(input.climate,grid,0,config->firstyear,config);
#ifdef USE_TIMING
        timing[GETCLIMATE_FCN]+=mrun()-t;
#endif
        if(config->with_glaciers)
          readicefrac(input.icefrac,grid,0,config->firstyear,config);
        addanomaly_climate(input.climate,data_index);
      }
    }
    else
    {
      if(config->storeclimate && !config->isanomaly && year==config->firstyear && config->nspinup)
      {
        /* restore climate data pointers to initial data */
        input.climate->data[0]=data_save;
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
      if(config->isanomaly)
      {
        rnd_year = (int)(erand48(config->seed)*config->nspinyear);
#ifdef USE_MPI
        MPI_Bcast(&rnd_year, 1, MPI_INT, 0, config->comm);
#endif
        moveclimate(input.climate,&store,data_index,rnd_year);
        if (config->delta_year>1)
        {
          if (year==config->firstyear || (config->ischeckpoint && year==startyear))
          {
#ifdef USE_TIMING
            t=mrun();
#endif
            if (getclimate(input.climate,grid,1,year,config))
            {
              fprintf(stderr, "ERROR104: Simulation stopped in getclimate().\n");
              fflush(stderr);
              break; /* leave time loop */
            }
            if (getclimate(input.climate, grid, 2, year + config->delta_year, config))
            {
              fprintf(stderr, "ERROR104: Simulation stopped in getclimate().\n");
              fflush(stderr);
              break; /* leave time loop */
            }
#ifdef USE_TIMING
            timing[GETCLIMATE_FCN]+=mrun()-t;
#endif
            if(config->with_glaciers)
            {
              if (readicefrac(input.icefrac, grid, 1, year, config))
              {
                fprintf(stderr, "ERROR104: Simulation stopped in readicefrac().\n");
                fflush(stderr);
                break; /* leave time loop */
              }
              if (readicefrac(input.icefrac, grid, 2, year + config->delta_year, config))
              {
                fprintf(stderr, "ERROR104: Simulation stopped in readicefrac().\n");
                fflush(stderr);
                break; /* leave time loop */
              }
            }
            index = 0;
          }
          else if (year != config->lastyear && (year - config->firstyear) % config->delta_year == 0)
          {
#ifdef USE_TIMING
            t=mrun();
#endif
            if (getclimate(input.climate, grid, index + 1, year, config))
            {
              fprintf(stderr, "ERROR104: Simulation stopped in getclimate().\n");
              fflush(stderr);
              break; /* leave time loop */
            }
#ifdef USE_TIMING
            timing[GETCLIMATE_FCN]+=mrun()-t;
#endif
            if(config->with_glaciers)
            {
              if (readicefrac(input.icefrac, grid, index + 1, year, config))
              {
                fprintf(stderr, "ERROR104: Simulation stopped in readicefrac().\n");
                fflush(stderr);
                break; /* leave time loop */
              }
            }
            index = (index + 1) % 2;
          }
          interpolate_climate(input.climate, index, (Real)((year - config->firstyear) % config->delta_year) / config->delta_year);
          if(config->with_glaciers)
            interpolate_icefrac(input.icefrac, index, (Real)((year - config->firstyear) % config->delta_year) / config->delta_year);
        }
        else
        {
#ifdef USE_TIMING
          t=mrun();
#endif
          if(getclimate(input.climate,grid,0,year,config))
          {
            fputs("ERROR104: Simulation stopped in getclimate().\n",stderr);
            fflush(stderr);
            break; /* leave time loop */
          }
#ifdef USE_TIMING
          timing[GETCLIMATE_FCN]+=mrun()-t;
#endif
          if(config->with_glaciers)
          {
            if (readicefrac(input.icefrac,grid,0,year,config))
            {
              fprintf(stderr, "ERROR104: Simulation stopped in readicefrac().\n");
              fflush(stderr);
              break; /* leave time loop */
            }
          }
        }
        addanomaly_climate(input.climate,data_index);
      }
      else
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
        else
          climate_year=year;
#ifdef USE_TIMING
        timing_start(t);
#endif
        rc=getclimate(input.climate,grid,0,climate_year,config);
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
    rc=iterateyear(output,grid,&input,co2,&ch4,&pch4,npft,ncft,year,config);
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
        fail(SEND_IMAGE_ERR,FALSE,FALSE,
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
  if(config->isanomaly || (config->storeclimate && config->nspinup && (config->lastyear<input.climate->firstyear || year<input.climate->firstyear)))
  {
    /* restore climate data pointers to initial data */
    input.climate->data[data_index]=data_save;
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

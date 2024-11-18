/**************************************************************************************/
/**                                                                                \n**/
/**                        l  p  j  m  l  .  c                                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**     Version with managed land and river routing                                \n**/
/**                                                                                \n**/
/**     The model simulates vegetation dynamics, hydrology and soil                \n**/
/**     organic matter dynamics on an area-averaged grid cell basis using          \n**/
/**     one-year time step. Input parameters are monthly mean air                  \n**/
/**     temperature, total precipitation and percentage of full sunshine,          \n**/
/**     annual atmospheric CO2 concentration and soil texture class. The           \n**/
/**     simulation for each grid cell begins from "bare ground",                   \n**/
/**     requiring a "spin up" (under non-transient climate) of c. 1000             \n**/
/**     years to develop equilibrium vegetation and soil structure.                \n**/
/**                                                                                \n**/
/**     Main program:                                                              \n**/
/**                                                                                \n**/
/**     {                                                                          \n**/
/**       Cell *grid;                                                              \n**/
/**       readconfig();                                                            \n**/
/**       if(config.sim_id==LPJML_IMAGE) open_image();                             \n**/
/**       initinput();                                                             \n**/
/**       printconfig();                                                           \n**/
/**       grid=newgrid();                                                          \n**/
/**       if(river_routing) initdrain(grid);                                       \n**/
/**       fopenoutput();                                                           \n**/
/**       iterate(grid);                                                           \n**/
/**       fcloseoutput();                                                          \n**/
/**       freegrid(grid);                                                          \n**/
/**       freeinput();                                                             \n**/
/**       freeconfig();                                                            \n**/
/**     }                                                                          \n**/
/**                                                                                \n**/
/**     Basic datatype Cell has the following hierarchy:                           \n**/
/**                                                                                \n**/
/**     --Cell                                                                     \n**/
/**       +-Coord                                                                  \n**/
/**       +-Managed_land                                                           \n**/
/**       | +-Landfrac                                                             \n**/
/**       | +-Resdata                                                              \n**/
/**       | | +-Resource                                                           \n**/
/**       | +-Cropdates                                                            \n**/
/**       | +-Manage                                                               \n**/
/**       | +-Image_data (only for version with -DIMAGE set)                       \n**/
/**       |   +-Pool                                                               \n**/
/**       +-Ignition                                                               \n**/
/**       +-Discharge                                                              \n**/
/**       | +-Queue                                                                \n**/
/**       +-Output                                                                 \n**/
/**       +-Climbuf                                                                \n**/
/**       | +-Buffer                                                               \n**/
/**       +-Balance                                                                \n**/
/**       +-Standlist                                                              \n**/
/**         +-Stand                                                                \n**/
/**           +-Pftlist                                                            \n**/
/**           | +-Pft-+-Pftgrass                                                   \n**/
/**           |       +-Pfttree                                                    \n**/
/**           |       +-Pftcrop                                                    \n**/
/**           +-Soil                                                               \n**/
/**             +-Pool                                                             \n**/
/**             +-Litter                                                           \n**/
/**               +-Litteritem                                                     \n**/
/**                 +-Trait                                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <time.h>
#include "lpj.h"
#include "grass.h"
#include "tree.h"
#include "crop.h"
#include "natural.h"
#include "grassland.h"
#include "biomass_tree.h"
#include "biomass_grass.h"
#include "agriculture.h"
#include "agriculture_grass.h"
#include "agriculture_tree.h"
#include "wetland.h"

#define NTYPES 3 /* number of plant functional types: grass, tree, annual_crop */
#define NSTANDTYPES 15  /* number of stand types / land use types as defined in landuse.h*/

int main(int argc,char **argv)
{
  Outputfile *output; /* Output file array */
  const char *progname;
  int year,rc;
  Cell *grid;         /* cell array */
  Input input;        /* input data */
  time_t tstart,tend,tbegin,tfinal;   /* variables for timing */
  Standtype standtype[NSTANDTYPES];
  String s;
  Config config;         /* LPJ configuration */

  /* Create array of functions, uses the typedef of Pfttype in config.h */
  Pfttype scanfcn[NTYPES]=
  {
    {name_grass,fscanpft_grass},
    {name_tree,fscanpft_tree},
    {name_crop,fscanpft_crop}
  };
  time(&tbegin);         /* Start timing for total wall clock time */
#ifdef USE_MPI
  MPI_Init(&argc,&argv); /* Initialize MPI */
/*
 * Use default communicator containing all processors. In defining your own
 * communicator it is possible to run LPJ on a subset of processors
 */
  initmpiconfig(&config,MPI_COMM_WORLD);
#else
  /* sequential version of LPJ */
  initconfig(&config);
#endif
  progname=strippath(argv[0]); /* strip path from program name */
  if(argc>1)
  {
    if(!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")) /* check for help option */
    {
      if(isroot(config))
      {
        help(progname);
      }
#ifdef USE_MPI
      MPI_Finalize();
#endif
      return EXIT_SUCCESS;
    }
    if(!strcmp(argv[1],"-l") || !strcmp(argv[1],"--license")) /* check for license option */
    {
      if(isroot(config))
        printlicense();
#ifdef USE_MPI
      MPI_Finalize();
#endif
      return EXIT_SUCCESS;
    }
    else if(!strcmp(argv[1],"-v") || !strcmp(argv[1],"--version")) /* check for version option */
    {
      if(isroot(config))
        printflags(progname);
#ifdef USE_MPI
      MPI_Finalize();
#endif
      return EXIT_SUCCESS;
    }
    else if(!strcmp(argv[1],"-hash")) /* check for hash option */
    {
      if(isroot(config))
        puts(gethash());
#ifdef USE_MPI
      MPI_Finalize();
#endif
      return EXIT_SUCCESS;
    }
    else if(!strcmp(argv[1],"-repo")) /* check for repo option */
    {
      if(isroot(config))
        puts(getrepo());
#ifdef USE_MPI
      MPI_Finalize();
#endif
      return EXIT_SUCCESS;
    }
  }
  if(isroot(config))
  { /* Output only for the root task 0 */
    copyright(progname);
    printf("\nRunning for user %s on %s at %s",getuser(),gethost(),
           ctime(&tbegin));
    fflush(stdout);
  }

  /* NTYPES is set to natural vegetation must be considered
   * in light and establishment
   * crops must have last id-number */
  /* Read configuration file */
  rc=readconfig(&config,scanfcn,NTYPES,NOUT,&argc,&argv,lpj_usage);
  failonerror(&config,rc,READ_CONFIG_ERR,"Cannot read configuration");
  if(isroot(config) && argc)
    fputs("WARNING018: Arguments listed after configuration filename, will be ignored.\n",stderr);
  if(config.ofiles)
  {
    if(isroot(config))
      fprintoutputvar(stdout,config.outnames,NOUT,config.npft[GRASS]+config.npft[TREE],config.npft[CROP],&config);
#ifdef USE_MPI
    MPI_Finalize();
#endif
    return EXIT_SUCCESS;
  }
  if(isroot(config))
  {
    createconfig(&config);
    printconfig(config.npft[GRASS]+config.npft[TREE],
                config.npft[CROP],&config);
  }
  if(config.sim_id==LPJML_FMS)
  {
    if(isroot(config))
      fputs("ERROR032: FMS coupler not supported in standalone version.\n",stderr);
    return NO_FMS_ERR;
  }
#if defined IMAGE && defined COUPLED
  if(config.sim_id==LPJML_IMAGE)
  {
    rc=open_image(&config);
    failonerror(&config,rc,OPEN_IMAGE_ERR,"Cannot open IMAGE coupler");
  }
#endif
  standtype[NATURAL]=natural_stand;
  standtype[WETLAND]=wetland_stand;
  standtype[SETASIDE_RF]=setaside_rf_stand;
  standtype[SETASIDE_IR]=setaside_ir_stand;
  standtype[SETASIDE_WETLAND]=setaside_wetland_stand;
  standtype[AGRICULTURE]=agriculture_stand;
  standtype[MANAGEDFOREST]=managedforest_stand;
  standtype[GRASSLAND]=grassland_stand;
  standtype[OTHERS]=others_stand;
  standtype[BIOMASS_TREE]=biomass_tree_stand;
  standtype[BIOMASS_GRASS]=biomass_grass_stand;
  standtype[AGRICULTURE_TREE]=agriculture_tree_stand;
  standtype[AGRICULTURE_GRASS]=agriculture_grass_stand;
  standtype[WOODPLANTATION]=woodplantation_stand;
  standtype[KILL]=kill_stand;
  /* Allocation and initialization of grid */
  rc=((grid=newgrid(&config,standtype,NSTANDTYPES,config.npft[GRASS]+config.npft[TREE],config.npft[CROP]))==NULL);
  failonerror(&config,rc,INIT_GRID_ERR,"Initialization of LPJ grid failed");
  if(iscoupled(config))
  {
    rc=open_coupler(&config);
    snprintf(s,STRING_LEN,"Cannot couple to %s model",config.coupled_model);
    failonerror(&config,rc,OPEN_COUPLER_ERR,s);
  }
  rc=initinput(&input,grid,config.npft[GRASS]+config.npft[TREE],&config);
  failonerror(&config,rc,INIT_INPUT_ERR,
              "Initialization of input data failed");
  if(config.check_climate)
  {
    rc=checkvalidclimate(input.climate,grid,&config);
    failonerror(&config,rc,INIT_INPUT_ERR,"Check of climate data failed");
  }
  /* open output files */  
  output=fopenoutput(grid,NOUT,&config);
  rc=(output==NULL);
  failonerror(&config,rc,INIT_OUTPUT_ERR,
              "Initialization of output data failed");
  rc=initoutput(output,grid,config.npft[GRASS]+config.npft[TREE],config.npft[CROP],&config);
  failonerror(&config,rc,INIT_OUTPUT_ERR,
              "Initialization of output data failed");
  if(iscoupled(config))
  {
    rc=check_coupler(&config);
    snprintf(s,STRING_LEN,"Cannot initialize %s model",config.coupled_model);
    failonerror(&config,rc,OPEN_COUPLER_ERR,s);
  }
  if(isopen(output,GRID))
    writecoords(output,GRID,grid,&config);
  if(isopen(output,TERR_AREA))
    writearea(output,TERR_AREA,grid,&config);
  if(isopen(output,LAKE_AREA))
    writearea(output,LAKE_AREA,grid,&config);
  if(isopen(output,COUNTRY) && config.withlanduse)
    writecountrycode(output,COUNTRY,grid,&config);
  if(isroot(config))
    puts("Simulation begins...");
  time(&tstart); /* Start timing */
  /* Starting simulation */
  year=iterate(output,grid,input,
               config.npft[GRASS]+config.npft[TREE],config.npft[CROP],
               &config);
  /* Simulation has finished */
  time(&tend); /* Stop timing */
  fcloseoutput(output,&config);
  if(isroot(config))
    puts((year>config.lastyear) ? "Simulation ended." : "Simulation stopped.");
  /* free memory */
  freeinput(input,&config);
  freegrid(grid,config.npft[GRASS]+config.npft[TREE],&config);
  if(isroot(config))
  {
    printf( (year>config.lastyear) ? "%s successfully" : "%s errorneously",progname);
    printf(" terminated, %d grid cells processed.\n"
           "Wall clock time:\t%d sec, %.2g sec/cell/year.\n",
           config.total,(int)(tend-tstart),
           (double)(tend-tstart)/config.total/max(year-config.firstyear+
                                                   config.nspinup,1));
#ifdef USE_TIMING
    if(iscoupled(config))
      printf("Time spent in communication to %s model: %.2g sec.\n",
             config.coupled_model,timing);
#endif
  }
#if defined IMAGE && defined COUPLED
  if(config.sim_id==LPJML_IMAGE)
    close_image(&config);
#endif
  if(iscoupled(config))
    close_coupler(year<=config.lastyear,&config);
  freeconfig(&config);
#ifdef USE_MPI
  /* Wait until all tasks have finished to measure total wall clock time */
  MPI_Barrier(MPI_COMM_WORLD);
  /* Exit MPI */
  MPI_Finalize();
#endif
  time(&tfinal);
  if(isroot(config))
  {
    printf("Total wall clock time:\t%d sec (",(int)(tfinal-tbegin));
    printtime((int)(tfinal-tbegin));
    puts(").");
  }
  return EXIT_SUCCESS;
} /* of 'main' */

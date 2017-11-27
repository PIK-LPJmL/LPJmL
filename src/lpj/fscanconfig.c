/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  c  o  n  f  i  g  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads configuration from an ascii file                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define fscanint2(file,var,name) if(fscanint(file,var,name,verbose)) return TRUE;
#define fscanname(file,var,name) {              \
    if(fscanstring(file,var,verbose!=NO_ERR)) {                 \
      if(verbose) readstringerr(name);  \
      return TRUE;                              \
    }                                              \
    if(verbose>=VERB)    \
      printf("%s %s\n", name, var);                     \
  }

#define scanfilename(file,var,path,what) {                              \
    if(readfilename2(file,var,path,verbose)) {                  \
      if(verbose) fprintf(stderr,"ERROR209: Cannot read input filename for '%s'.\n",what); \
      return TRUE;                                                      \
    }                                                                   \
    if(verbose>=VERB)                      \
      printf("%s %s\n", what, (var)->name);                             \
  }

#define scanclimatefilename(file,var,path,isfms,what) {                 \
    if(readclimatefilename(file,var,path,isfms,verbose)) {      \
      if(verbose) fprintf(stderr,"ERROR209: Cannot read input filename for '%s'.\n",what); \
      return TRUE;                                                      \
    }                                                                   \
    if(verbose>=VERB)                      \
      printf("%s %s\n", what, (var)->name);                             \
  }

static Bool readfilename2(FILE *file,Filename *name,const char *path,Verbosity verbose)
{
  if(readfilename(file,name,path,FALSE,verbose))
    return TRUE;
  if(name->fmt==CDF)
  {
    if(verbose)
      fprintf(stderr,"ERROR197: NetCDF is not supported for input '%s' in line %d of '%s' in this version of LPJmL.\n",name->name,getlinecount(),getfilename());
    return TRUE;
  }
  else if(name->fmt==TXT)
  {
    if(verbose)
      fprintf(stderr,"ERROR197: text file is not supported for input '%s' in line %d of '%s' in this version of LPJmL.\n",name->name,getlinecount(),getfilename());
    return TRUE;
  }
  return FALSE;
} /* of 'readfilename2' */

static Bool readclimatefilename(FILE *file,Filename *name,const char *path,Bool isfms,Verbosity verbose)
{
  if(readfilename(file,name,path,TRUE,verbose))
    return TRUE;
  if(!isfms && name->fmt==FMS)
  {
    if(verbose)
      fprintf(stderr,"ERROR197: FMS coupler not allowed for input in line %d of '%s'.\n",getlinecount(),getfilename());
    return TRUE;
  }
  if(name->fmt==TXT)
  {
    if(verbose)
      fprintf(stderr,"ERROR197: text file is not supported for input '%s' in line %d of '%s' in this version of LPJmL.\n",name->name,getlinecount(),getfilename());
    return TRUE;
  }
  return FALSE;
} /* of 'readfilename2' */


static void divide(int *start, /**< index of first grid cell */
                   int *end,   /**< index of last grid cell */
                   int rank,   /**< my rank id */
                   int ntask   /**< total number of tasks */
                  )
{
/*
 * Function is used in the parallel MPI version to distribute the cell grid
 * equally on ntask tasks
 * On return start and end are set to the local boundaries of each task
 */
  int i,lo,hi,n;
  n=*end-*start+1;
  lo=*start;
  hi=*start+n/ntask-1;
  if(n % ntask)
    hi++;
  for(i=1;i<=rank;i++)
  {
    lo=hi+1;
    hi=lo+n/ntask-1;
    if(n % ntask>i)
      hi++;
  }
  *start=lo;
  *end=hi;
} /* of 'divide' */

Bool fscanconfig(Config *config,    /**< LPJ configuration */
                 FILE *file,        /**< File pointer to text file */
                 Fscanpftparfcn scanfcn[], /**< array of PFT-specific scan
                                              functions */
                 int ntypes,        /**< Number of PFT classes */
                 int nout_max       /**< maximum number of output files */
                )                   /** \return TRUE on error */
 {
  String name;
  int restart,endgrid,israndom,wateruse,grassfix;
  Verbosity verbose;

  verbose=(isroot(*config)) ? config->scan_verbose : NO_ERR;

  /*=================================================================*/
  /* I. Reading type section                                         */
  /*=================================================================*/

  if (verbose>=VERB) puts("// I. type section");
  fscanint2(file,&israndom,"prec");
  if(israndom==RANDOM_PREC)
  {
    fscanint2(file,&config->seed,"random seed");
    if(config->seed==RANDOM_SEED)
      config->seed=time(NULL);
  }
  else
    config->seed=0;
  fscanint2(file,&config->with_radiation,"with_radiation");
  if(config->with_radiation<CLOUDINESS || config->with_radiation>RADIATION_LWDOWN)
  {
    if(verbose)
      fprintf(stderr,"ERROR219: Invalid radiation model %d in line %d of '%s'.\n",config->with_radiation,getlinecount(),getfilename());
    return TRUE;
  }
#ifdef IMAGE
  if(config->sim_id==LPJML_IMAGE && config->with_radiation)
  {
    if(verbose)
      fputs("ERROR208: Radiation data not supported by IMAGE coupler.\n",
            stderr);
    return TRUE;
  }
#endif
  fscanint2(file,&config->fire,"fire");
  if(config->sim_id==LPJ)
    config->firewood=NO_FIREWOOD;
  else
  {
    fscanint2(file,&config->firewood,"firewood");
  }
  fscanint2(file,&config->ispopulation,"population");
  fscanint2(file,&config->prescribe_burntarea,"prescribe burntarea");
  fscanint2(file,&config->prescribe_landcover,"prescribe landcover");
  if(config->prescribe_landcover<NO_LANDCOVER || config->prescribe_landcover>LANDCOVERFPC)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR166: Invalid value for prescribe landcover=%d in line %d of '%s'.\n",
              config->prescribe_landcover,getlinecount(),getfilename());
    return TRUE;
  }
  fscanint2(file,&config->new_phenology,"new phenology");
  fscanint2(file,&config->river_routing,"river routing");
  config->reservoir=FALSE;
  fscanint2(file,&config->permafrost,"permafrost");
  config->sdate_option=NO_FIXED_SDATE;
  if(config->sim_id!=LPJ)
  {
    fscanint2(file,&config->withlanduse,"withlanduse");
    if(config->withlanduse!=NO_LANDUSE)
    {
      fscanint2(file,&config->sdate_option,"sowing date option");
      if(config->sdate_option<0 || config->sdate_option>PRESCRIBED_SDATE)
      {
        if(verbose)
          fprintf(stderr,"ERROR166: Invalid value for sowing date option=%d in line %d of '%s'.\n",
                  config->sdate_option,getlinecount(),getfilename());
        return TRUE;
      }
      fscanint2(file,&config->irrig_scenario,"irrigation scenario");
      if(config->irrig_scenario<0 || config->irrig_scenario>ALL_IRRIGATION)
      {
        if(verbose)
          fprintf(stderr,"ERROR166: Invalid value for irrigation scenario=%d in line %d of '%s'.\n",
                  config->irrig_scenario,getlinecount(),getfilename());
        return TRUE;
      }
      fscanint2(file,&config->laimax_interpolate,"laimax_interpolate");
      config->isconstlai=(config->laimax_interpolate==CONST_LAI_MAX);
      if(config->river_routing)
        fscanint2(file,&config->reservoir,"reservoir");
      fscanint2(file,&grassfix,"grassland_fixed_pft");
    }
    fscanint2(file,&wateruse,"wateruse");
    if(wateruse==WATERUSE && config->withlanduse==NO_LANDUSE)
    {
      if(isroot(*config))
        fputs("ERROR224: Wateruse without landuse set.\n",stderr);
      return TRUE;
    }
  }
  else
  {
    config->withlanduse=NO_LANDUSE;
    wateruse=NO_WATERUSE;
  }
  /*=================================================================*/
  /* II. Reading input parameter section                             */
  /*=================================================================*/

  if (verbose>=VERB) puts("// II. input parameter section");

  /* ntypes is set to natural vegetation must be considered
   * in light and establishment
   * crops must have last id-number */
  /* Read PFT parameter array */
  if(fscanparam(file,config))
    return TRUE;
  if((config->nsoil=fscansoilpar(file,&config->soilpar,verbose))==0)
    return TRUE;
  if((config->npft=fscanpftpar(file,&config->pftpar,scanfcn,ntypes,verbose))==NULL)
    return TRUE;
  config->ntypes=ntypes;
  config->nbiomass=getnbiomass(config->pftpar,config->npft[GRASS]+config->npft[TREE]);
  /* Read soil paramater array */
  if(config->withlanduse!=NO_LANDUSE)
  {
    /* landuse enabled */
    if((config->ncountries=fscancountrypar(file,&config->countrypar,(config->laimax_interpolate==LAIMAX_CFT) ? config->npft[CROP] : 0,verbose))==0)
      return TRUE;
    if((config->nregions=fscanregionpar(file,&config->regionpar,verbose))==0)
      return TRUE;
  }
  else
  {
    config->countrypar=NULL;
    config->regionpar=NULL;
  }
  fscanint2(file,&config->compress,"compress");
#ifdef USE_NETCDF
  if(config->compress)
    fputs("WARNING403: Compression of NetCDF files is not supported in this version of NetCDF.\n",stderr);
#endif
  if(fscanfloat(file,&config->missing_value,"missing value",verbose))
    return TRUE;
  config->outnames=fscanoutputvar(file,NOUT,verbose);
  if(config->outnames==NULL)
    return TRUE;
  /*=================================================================*/
  /* III. Reading input data section                                 */
  /*=================================================================*/

  if (verbose>=VERB) puts("// III. input data section");
  scanclimatefilename(file,&config->soil_filename,config->inputdir,FALSE,"soil");
  if(config->soil_filename.fmt!=CDF)
  {
    scanfilename(file,&config->coord_filename,config->inputdir,"coord");
  }
  if(config->withlanduse!=NO_LANDUSE)
  {
    scanclimatefilename(file,&config->countrycode_filename,config->inputdir,FALSE,"countrycode");
    if(config->countrycode_filename.fmt==CDF)
    {
      scanclimatefilename(file,&config->regioncode_filename,config->inputdir,FALSE,"regioncode");
    }
    scanclimatefilename(file,&config->landuse_filename,config->inputdir,FALSE,"landuse");
    if(config->sdate_option==PRESCRIBED_SDATE)
    {
      scanclimatefilename(file,&config->sdate_filename,config->inputdir,FALSE,"sdate");
    }
    if(grassfix == GRASS_FIXED_PFT)
    {
      scanclimatefilename(file,&config->grassfix_filename,config->inputdir,FALSE,"Grassland fixed pft");
    }
    else
      config->grassfix_filename.name = NULL;
  }
  else
  {
    config->grassfix_filename.name = NULL;
  }
  if(config->river_routing)
  {
    scanclimatefilename(file,&config->lakes_filename,config->inputdir,FALSE,"lakes");
    scanclimatefilename(file,&config->drainage_filename,config->inputdir,FALSE,"drainage");
    if(config->drainage_filename.fmt==CDF)
    {
      scanclimatefilename(file,&config->river_filename,config->inputdir,FALSE,"river");
    }
    if(config->withlanduse!=NO_LANDUSE)
    {
      scanclimatefilename(file,&config->neighb_irrig_filename,config->inputdir,FALSE,"neighb_irrig");
      if(config->reservoir)
      {
        scanclimatefilename(file,&config->elevation_filename,config->inputdir,FALSE,"elevation");
        scanfilename(file,&config->reservoir_filename,config->inputdir,"reservoir");
      }
    }
    if(config->sim_id==LPJML_FMS)
    {
      scanfilename(file,&config->runoff2ocean_filename,config->inputdir,"runoff2ocean_map");
    }
  }
  scanclimatefilename(file,&config->temp_filename,config->inputdir,config->sim_id==LPJML_FMS,"temp");
  scanclimatefilename(file,&config->prec_filename,config->inputdir,config->sim_id==LPJML_FMS,"prec");
  switch(config->with_radiation)
  {
    case RADIATION:
      scanclimatefilename(file,&config->lwnet_filename,config->inputdir,config->sim_id==LPJML_FMS,"lwnet");
      scanclimatefilename(file,&config->swdown_filename,config->inputdir,config->sim_id==LPJML_FMS,"swdown");
      break;
    case CLOUDINESS:
      scanclimatefilename(file,&config->cloud_filename,config->inputdir,config->sim_id==LPJML_FMS,"cloud");
      break;
    case SWDOWN:
      scanclimatefilename(file,&config->swdown_filename,config->inputdir,config->sim_id==LPJML_FMS,"swdown");
      break;
    default:
      fprintf(stderr,"ERROR213: Invalid setting %d for radiation in line %d of '%s'.\n",config->with_radiation,getlinecount(),getfilename());
      return TRUE;
  }
  if(config->fire==SPITFIRE)
  {
    scanclimatefilename(file,&config->wind_filename,config->inputdir,config->sim_id==LPJML_FMS,"wind");
    scanclimatefilename(file,&config->tamp_filename,config->inputdir,config->sim_id==LPJML_FMS,"tamp");
#ifdef TMAX_SPITFIRE
    if(config->tamp_filename.fmt==CDF)
    {
      scanclimatefilename(file,&config->tmax_filename,config->inputdir,config->sim_id==LPJML_FMS,"tmax");
    }
    else
#endif
      config->tmax_filename.name=NULL;
    scanclimatefilename(file,&config->lightning_filename,config->inputdir,FALSE,"lightning");
    scanclimatefilename(file,&config->human_ignition_filename,
                        config->inputdir,FALSE,"human_ignition");
  }
  if(config->ispopulation)
  {
    scanclimatefilename(file,&config->popdens_filename,config->inputdir,FALSE,"popdens");
  }
  if(config->prescribe_burntarea)
  {
    scanclimatefilename(file,&config->burntarea_filename,config->inputdir,FALSE,"burntarea");
  }
  if(config->prescribe_landcover!=NO_LANDCOVER)
  {
    scanclimatefilename(file,&config->landcover_filename,config->inputdir,FALSE,"landcover");
  }
  if(readfilename(file,&config->co2_filename,config->inputdir,FALSE,verbose))
  {
    if(verbose)
      fputs("ERROR209: Cannot read input filename for 'co2'.\n",stderr);
    return TRUE;
  }
  if(config->co2_filename.fmt!=TXT &&  (config->sim_id!=LPJML_FMS || config->co2_filename.fmt!=FMS))
  {
    if(verbose)
      fputs("ERROR197: Only text file is supported for CO2 input in this version of LPJmL.\n",stderr);
    return TRUE;
  }
  if (verbose>=VERB) printf("co2 %s\n", config->co2_filename.name);

  if(israndom==RANDOM_PREC)
  {
    scanclimatefilename(file,&config->wet_filename,config->inputdir,config->sim_id==LPJML_FMS,"wetdays");
  }
#ifdef IMAGE
  else if(config->sim_id==LPJML_IMAGE)
  {
    if(verbose)
      fputs("ERROR175: Number of wet days must be supplied for IMAGE coupler.\n",stderr);
    return TRUE;
  }
#endif
  else
    config->wet_filename.name=NULL;
  if(wateruse==WATERUSE)
  {
    scanclimatefilename(file,&config->wateruse_filename,config->inputdir,FALSE,"wateruse");
  }
  else
    config->wateruse_filename.name=NULL;
#ifdef IMAGE
  if(config->sim_id==LPJML_IMAGE)
  {
    /* reading IMAGE-coupling specific information */
    scanclimatefilename(file,&config->temp_var_filename,config->inputdir,FALSE,"temp_var");
    scanclimatefilename(file,&config->prec_var_filename,config->inputdir,FALSE,"prec_var");
    scanclimatefilename(file,&config->prodpool_init_filename,config->inputdir,FALSE,"prodpool_init");
  }
#endif

  /*=================================================================*/
  /* IV. Reading output data section                                 */
  /*=================================================================*/

  if (verbose>=VERB) puts("// IV. output data section");
  if(fscanoutput(file,config,nout_max))
    return TRUE;

  /*=================================================================*/
  /* V. Reading run settings section                                 */
  /*=================================================================*/

  if (verbose>=VERB) puts("// V. run settings");
  fscanint2(file,&config->startgrid,"startgrid");
  if(config->startgrid==ALL)
  {
    config->startgrid=0;
    endgrid=getnsoilcode(&config->soil_filename,config->nsoil,isroot(*config));
    if(endgrid==-1)
      return TRUE;
    else if(endgrid==0)
    {
      if(verbose)
        fputs("ERROR135: No soil code found.\n",stderr);
      return TRUE;
    }
    else
      endgrid--;
  }
  else
    fscanint2(file,&endgrid,"endgrid");
  if(endgrid<config->startgrid)
  {
    if(verbose)
      fprintf(stderr,"ERROR136: Endgrid=%d less than startgrid=%d.\n",
              endgrid,config->startgrid);
    return TRUE;
  }
  config->nall=endgrid-config->startgrid+1;
  config->firstgrid=config->startgrid;
  if(config->nall<config->ntask)
  {
    if(verbose)
      fprintf(stderr,"ERROR198: Number of cells %d less than number of tasks.\n",config->nall);
    return TRUE;
  }
  if(config->ntask>1) /* parallel mode? */
    divide(&config->startgrid,&endgrid,config->rank,
           config->ntask);
  config->ngridcell=endgrid-config->startgrid+1;
  fscanint2(file,&config->nspinup,"nspinup");
  if(config->nspinup)
    fscanint2(file,&config->nspinyear,"nspinyear");
  fscanint2(file,&config->firstyear,"firstyear");
  fscanint2(file,&config->lastyear,"lastyear");
#ifdef IMAGE
  if(config->sim_id==LPJML_IMAGE)
  {
    fscanint2(file,&config->start_imagecoupling,"start_imagecoupling");
  }
  else
    config->start_imagecoupling=INT_MAX;
#endif
  if(config->firstyear-config->nspinup>config->lastyear)
  {
    if(verbose)
      fprintf(stderr,"ERROR105: First simulation year=%d greater than last simulation year=%d.\n",
            config->firstyear-config->nspinup,config->lastyear);
    return TRUE;
  }
  fscanint2(file,&restart,"restart");
  if(restart==RESTART)
  {
    fscanname(file,name,"restart filename");
    config->restart_filename=addpath(name,config->restartdir);
    config->from_restart=TRUE;
  }
  else
  {
    config->restart_filename=NULL;
    config->from_restart=FALSE;
  }
  fscanint2(file,&restart,"write_restart");
  if(restart==RESTART)
  {
    fscanname(file,name,"write_restart filename");
    config->write_restart_filename=addpath(name,config->restartdir);
    fscanint2(file,&config->restartyear,"restart year");
    if(config->restartyear>config->lastyear || config->restartyear<config->firstyear-config->nspinup)
    {
      if(verbose)
        fprintf(stderr,"WARNING014: Restart year=%d not in simulation years, no restart file written.\n",config->restartyear);
      free(config->write_restart_filename);
      config->write_restart_filename=NULL;
    }
  }
  else
    config->write_restart_filename=NULL;
  return FALSE;
} /* of 'fscanconfig' */

/**************************************************************************************/
/**                                                                                \n**/
/**                   f  i  l  e  s  e  x  i  s  t  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions checks whether input files exist and have the correct            \n**/
/**     format                                                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <sys/stat.h>
#include "lpj.h"

#ifndef S_ISDIR /* macro is not defined on Windows */
#define S_ISDIR(a) (a & _S_IFDIR)
#endif

static int checkfile(const char *filename)
{
  if(getfilesize(filename)==-1)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",filename,strerror(errno));
    return 1;
  }
  else
    return 0;
} /* of 'checkfile' */

static int checkinputfile(const Config *config,const Filename *filename,const char *unit,size_t len)
{
  FILE *file;
  Header header;
  String headername;
  int version;
  Bool swap;
  Input_netcdf input;
  size_t offset;
  if(filename->fmt==CDF)
  {
    input=openinput_netcdf(filename,unit,len,config);
    if(input==NULL)
      return 1;
    closeinput_netcdf(input);
  }
  else
  {
    file=openinputfile(&header,&swap,filename,headername,&version,&offset,FALSE,config);
    if(file==NULL)
      return 1;
    fclose(file);
    if(len==0)
      len=1;
    if(header.nbands!=len)
    {
      fprintf(stderr,"ERROR228: Number of bands %d in '%s' is not '%d'.\n",header.nbands,filename->name,(int)len);
      return 1;
    }
  }
  return 0;
} /* of 'checkinputfile' */

static int checkdatafile(const Config *config,const Filename *filename,const char *unit)
{
  FILE *file;
  Header header;
  String headername;
  int version;
  Bool swap;
  Climatefile input;
  size_t offset;
  if(filename->fmt==CDF)
  {
    if(openfile_netcdf(&input,filename,unit,config))
      return 1;
    closeclimate_netcdf(&input,TRUE);
  }
  else
  {
    file=openinputfile(&header,&swap,filename,headername,&version,&offset,FALSE,config);
    if(file==NULL)
      return 1;
    fclose(file);
  }
  return 0;
} /* of 'checkdatafile' */

static int checkclmfile(const Config *config,const Filename *filename,const char *unit,Bool check)
{
  FILE *file;
  Header header;
  String headername;
  int version;
  Bool swap;
  char *s,*name;
  Climatefile input;
  size_t offset;
  int first,last,year,count;
  if(filename->fmt==FMS)
    return 0;
  if(filename->fmt==CDF)
  {
    input.oneyear=FALSE;
    s=strchr(filename->name,'[');
    if(s!=NULL && sscanf(s,"[%d-%d]",&first,&last)==2)
    {
      s=mkfilename(filename->name);
      if(s==NULL)
      {
        fprintf(stderr,"ERROR225: Cannot parse filename '%s'.\n",filename->name);
        return 0;
      }
      name=malloc(strlen(s)+8);
      count=0;
      for(year=first;year<=last;year++)
      {
        sprintf(name,s,year);
        if(openclimate_netcdf(&input,name,filename->time,filename->var,filename->unit,unit,config))
        {
          count++;
        }
        else
          closeclimate_netcdf(&input,TRUE);
      }
      free(s);
      free(name);
      return count;
    }
    else
    {
      if(check)
      {
        if(openclimate_netcdf(&input,filename->name,filename->time,filename->var,filename->unit,unit,config))
          return 1;
        closeclimate_netcdf(&input,TRUE);
        if(input.firstyear>config->firstyear)
        {
          fprintf(stderr,"ERROR237: First year=%d in '%s' is greater than first simulation year %d.\n",input.firstyear,filename->name,config->firstyear);
          return 1;
        }
        if(!config->fix_climate && input.firstyear+input.nyear-1<config->lastyear)
        {
          fprintf(stderr,"ERROR237: Last year=%d in '%s' is less than last simulation year %d.\n",input.firstyear+input.nyear-1,filename->name,config->lastyear);
          return 1;
        }
        else if(config->fix_climate && input.firstyear+input.nyear-1<config->fix_climate_year+config->fix_climate_cycle/2)
        {
          fprintf(stderr,"ERROR237: Last year=%d in '%s' is less than last simulation year %d.\n",input.firstyear+input.nyear-1,filename->name,config->fix_climate_year+config->fix_climate_cycle/2);
          return 1;
        }
      }
    }
  }
  else
  {
    file=openinputfile(&header,&swap,filename,headername,&version,&offset,FALSE,config);
    if(file==NULL)
      return 1;
    fclose(file);
    if(check)
    {
      if(header.firstyear>config->firstyear)
      {
        fprintf(stderr,"ERROR237: First year=%d in '%s' is greater than first simulation year %d.\n",header.firstyear,filename->name,config->firstyear);
        return 1;
      }
      if(!config->fix_climate && header.firstyear+header.nyear-1<config->lastyear)
      {
        fprintf(stderr,"ERROR237: Last year=%d in '%s' is less than last simulation year %d.\n",header.firstyear+header.nyear-1,filename->name,config->lastyear);
        return 1;
      }
        else if(config->fix_climate && header.firstyear+header.nyear-1<config->fix_climate_year+config->fix_climate_cycle/2)
        {
          fprintf(stderr,"ERROR237: Last year=%d in '%s' is less than last simulation year %d.\n",header.firstyear+header.nyear-1,filename->name,config->fix_climate_year+config->fix_climate_cycle/2);
          return 1;
        }
    }
  }
  return 0;
} /* of 'checkclmfile' */

static int checkrestartfile(Config *config, const char *filename)
{
  FILE *file;
  Bool swap;
  file=openrestart(filename,config,
                   config->npft[GRASS]+config->npft[TREE]+config->npft[CROP],
                   &swap);
  if(file==NULL)
    return 1;
  fclose(file);
  return 0;
} /* of 'checkrestartfile' */

static int checksoilfile(Config *config,const Filename *filename)
{
  int size;
  size=getnsoilcode(filename,config->nsoil,TRUE);
  if(size==-1)
    return 1;
  if(size<config->nall+config->firstgrid)
  {
    fprintf(stderr,"File '%s' too short.\n",filename->name);
    return 1;
  }
  return 0;
} /* of 'checksoilfile' */

static int checkdir(const char *path)
{
  struct stat filestat;
  if(stat(path,&filestat))
  {
    fprintf(stderr,"Directory '%s' cannot be openend: %s.\n",path,
            strerror(errno));
    return 1;
  }
  else if(!S_ISDIR(filestat.st_mode))
  {
    fprintf(stderr,"'%s' is not a directory.\n",path);
    return 1;
  }
  else
    return 0;
} /* of 'checkdir' */

static int checkcoordfileclm(Config *config,const Filename *filename)
{
  float lon,lat;
  Coordfile coordfile;
  coordfile=opencoord(filename,isroot(*config));
  if(coordfile==NULL)
  {
    config->resolution.lon=config->resolution.lat=0.5;
    return 1;
  }
  getcellsizecoord(&lon,&lat,coordfile);
  config->resolution.lon=lon;
  config->resolution.lat=lat;
  closecoord(coordfile);
  return 0;
} /* of 'checkcoordfileclm' */

static int checkcoordfile(Config *config,const Filename *filename)
{
  Coord_netcdf coord_netcdf;
  coord_netcdf=opencoord_netcdf(filename->name,filename->var,TRUE);
  if(coord_netcdf==NULL)
    return 1;
  getresolution_netcdf(coord_netcdf,&config->resolution);
  closecoord_netcdf(coord_netcdf);
  return 0;
} /* of 'checkcoordfile' */

Bool filesexist(Config config, /**< LPJmL configuration */
                Bool isout     /**< write output on stdout (TRUE/FALSE) */
               )               /** \return TRUE on error */
{
  int i,bad,badout;
  long long size;
  char *path,*oldpath;
  bad=0;
  if(config.soil_filename.fmt!=CDF)
  {
    bad+=checkcoordfileclm(&config,&config.coord_filename);
    bad+=checksoilfile(&config,&config.soil_filename);
  }
  else
    bad+=checkcoordfile(&config,&config.soil_filename);
  if(config.river_routing)
  {
    bad+=checkinputfile(&config,&config.drainage_filename,NULL,(config.drainage_filename.fmt==CDF) ? 0 : 2);
    bad+=checkinputfile(&config,&config.lakes_filename,"1",0);
    if(config.withlanduse!=NO_LANDUSE)
      bad+=checkinputfile(&config,&config.neighb_irrig_filename,NULL,0);
  }
  if(config.ispopulation)
    bad+=checkdatafile(&config,&config.popdens_filename,"km-2");
  if(config.with_nitrogen)
  {
    if(config.with_nitrogen==LIM_NITROGEN && !config.no_ndeposition)
    {
      bad+=checkclmfile(&config,&config.no3deposition_filename,"g/m2/day",TRUE);
      bad+=checkclmfile(&config,&config.nh4deposition_filename,"g/m2/day",TRUE);
    }
    bad+=checkinputfile(&config,&config.soilph_filename,NULL,0);
  }
  if(config.grassfix_filename.name!=NULL)
    bad+=checkinputfile(&config,&config.grassfix_filename,NULL,0);
  if(config.grassharvest_filename.name!=NULL)
    bad+=checkinputfile(&config,&config.grassharvest_filename,NULL,0);
  if(config.with_nitrogen || config.fire==SPITFIRE || config.fire==SPITFIRE_TMAX)
    bad+=checkclmfile(&config,&config.wind_filename,"m/s",TRUE);
  if(config.fire==SPITFIRE || config.fire==SPITFIRE_TMAX)
  {
    if(config.fdi==WVPD_INDEX)
      bad+=checkclmfile(&config,&config.humid_filename,NULL,TRUE);
    bad+=checkdatafile(&config,&config.lightning_filename,NULL);
    bad+=checkinputfile(&config,&config.human_ignition_filename,NULL,0);
  }
  if(config.cropsheatfrost || config.fire==SPITFIRE_TMAX)
  {
    bad+=checkclmfile(&config,&config.tmin_filename,"celsius",TRUE);
    bad+=checkclmfile(&config,&config.tmax_filename,"celsius",TRUE);
  }
  if(config.fire==SPITFIRE)
  {
    bad+=checkclmfile(&config,&config.tamp_filename,NULL,TRUE);
  }
  if(config.wateruse)
    bad+=checkdatafile(&config,&config.wateruse_filename,"dm3/yr");
  bad+=checkclmfile(&config,&config.temp_filename,"celsius",TRUE);
  bad+=checkclmfile(&config,&config.prec_filename,"kg/m2/day",TRUE);
#ifdef IMAGE
  if (config.wateruse_wd_filename.name != NULL)
    bad += checkdatafile(&config, &config.wateruse_wd_filename,"dm3/yr");
#endif
  if(config.with_radiation)
  {
    if(config.with_radiation==RADIATION || config.with_radiation==RADIATION_LWDOWN)
      bad+=checkclmfile(&config,&config.lwnet_filename,"W/m2",TRUE);
    bad+=checkclmfile(&config,&config.swdown_filename,"W/m2",TRUE);
  }
  else
    bad+=checkclmfile(&config,&config.cloud_filename,"%",TRUE);
   if(config.co2_filename.fmt!=FMS)
    bad+=checkfile(config.co2_filename.name);
  if(config.wet_filename.name!=NULL)
    bad+=checkclmfile(&config,&config.wet_filename,"day",FALSE);
#ifdef IMAGE
  if(config.sim_id==LPJML_IMAGE)
  {
    bad+=checkclmfile(&config,&config.temp_var_filename,NULL,FALSE);
    bad+=checkclmfile(&config,&config.prec_var_filename,NULL,FALSE);
    bad+=checkclmfile(&config,&config.prodpool_init_filename,NULL,FALSE);
  }
#endif
  if(ischeckpointrestart(&config) && getfilesize(config.checkpoint_restart_filename)!=-1)
  {
    config.ischeckpoint=TRUE;
    if(checkrestartfile(&config,config.checkpoint_restart_filename))
      bad++;
    else
      printf("Starting from checkpoint file '%s' in year %d.\n",
             config.checkpoint_restart_filename,config.checkpointyear);
  }
  else if(config.restart_filename!=NULL)
  {
    config.ischeckpoint=FALSE;
    bad+=checkrestartfile(&config,config.restart_filename);
  }
  if(config.prescribe_landcover!=NO_LANDCOVER)
    bad+=checkinputfile(&config,&config.landcover_filename,"1",config.npft[GRASS]+config.npft[TREE]-config.nbiomass);
  if(config.withlanduse!=NO_LANDUSE)
  {
    bad+=checkdatafile(&config,&config.landuse_filename,"1");
    if(config.nagtree)
    {
      bad+=checkinputfile(&config,&config.sowing_cotton_rf_filename,NULL,0);
      bad+=checkinputfile(&config,&config.harvest_cotton_rf_filename,NULL,0);
      bad+=checkinputfile(&config,&config.sowing_cotton_ir_filename,NULL,0);
      bad+=checkinputfile(&config,&config.harvest_cotton_ir_filename,NULL,0);
    }
    if(config.sdate_option==PRESCRIBED_SDATE)
      bad+=checkinputfile(&config,&config.sdate_filename,NULL,2*config.npft[CROP]);
    if(config.crop_phu_option==PRESCRIBED_CROP_PHU)
      bad+=checkclmfile(&config,&config.crop_phu_filename,NULL,FALSE);
    if(config.countrycode_filename.fmt==CDF)
    {
      bad+=checkinputfile(&config,&config.countrycode_filename,NULL,0);
      bad+=checkinputfile(&config,&config.regioncode_filename,NULL,0);
    }
    else
      bad+=checkinputfile(&config,&config.countrycode_filename,NULL,2);
    if(config.reservoir)
    {
      bad+=checkinputfile(&config,&config.elevation_filename,"m",0);
      bad+=checkinputfile(&config,&config.reservoir_filename,NULL,10);
    }
    if(config.with_nitrogen&& config.fertilizer_input==FERTILIZER &&!config.fix_fertilization)
      bad+=checkclmfile(&config,&config.fertilizer_nr_filename,"g/m2",FALSE);
#ifdef IMAGE
    if(config.aquifer_irrig==AQUIFER_IRRIG)
    {
      bad+=checkinputfile(&config,&config.aquifer_filename,NULL,0);
    }
#endif
    if (config.with_nitrogen&&config.manure_input&&!config.fix_fertilization)
      bad+=checkclmfile(&config,&config.manure_nr_filename,"g/m2",FALSE);
  }
  badout=0;
  oldpath=strdup("");
  if(config.n_out)
    size=outputfilesize(&config);
  for(i=0;i<config.n_out;i++)
  {
    path=getpath(config.outputvars[i].filename.name);
    if(strcmp(path,oldpath))
    {
      if(checkdir(path))
        badout++;
      else if(diskfree(path)<size)
        fprintf(stderr,"Warning: Disk space on '%s' is insufficient for output files.\n",path);
      free(oldpath);
      oldpath=path;
    }
    else
      free(path);
  }
  free(oldpath);
  if(config.write_restart_filename!=NULL)
  {
    path=getpath(config.write_restart_filename);
    badout+=checkdir(path);
    free(path);
  }
  if((config.n_out || config.write_restart_filename!=NULL) && badout==0)
  {
    if(isout)
      puts("All output directories exist.");
  }
  else
    fprintf(stderr,"%d output directories do not exist.\n",badout);
  if(bad)
    fprintf(stderr,"%d input files do not exist.\n",bad);
  else if(isout)
    puts("All input files exist.");
  return bad>0 || badout>0;
} /* of 'filesexist' */

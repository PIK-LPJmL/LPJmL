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

#include "lpj.h"

static int checksoilcode(Config *config)
{
  FILE *file;
  Bool swap;
  Bool *exist;
  size_t offset;
  Type type;
  int cell,ncell;
  unsigned int i,soilcode;
  char *name;
  List *map;
  int *soilmap;
  if(config->soil_filename.fmt!=CDF)
  {
    file=fopensoilcode(&config->soil_filename,&map,&swap,&offset,&type,config->nsoil,TRUE);
    if(file==NULL)
      return 1;
    ncell=getnsoilcode(&config->soil_filename,config->nsoil,TRUE);
    if(map!=NULL)
    {
      soilmap=getsoilmap(map,config);
      if(soilmap!=NULL)
      {
        if(config->soilmap!=NULL)
          cmpsoilmap(soilmap,getlistlen(map),config);
        free(config->soilmap);
        config->soilmap=soilmap;
        config->soilmap_size=getlistlen(map);
      }
      freemap(map);
    }
    if(config->soilmap==NULL)
    {
      config->soilmap=defaultsoilmap(&config->soilmap_size,config);
      if(config->soilmap==NULL)
        return 0;
    }
    exist=newvec(Bool,config->soilmap_size);
    if(exist==NULL)
    {
      printallocerr("exist");
      fclose(file);
      return 0;
    }
    for(i=0;i<config->soilmap_size;i++)
      exist[i]=FALSE;
    name=getrealfilename(&config->soil_filename);
    if(name==NULL)
    {
      printallocerr("name");
      fclose(file);
      return 0;
    }
    for(cell=0;cell<ncell;cell++)
    {
      if(freadsoilcode(file,&soilcode,swap,type))
      {
        fprintf(stderr,"ERROR190: Unexpected end of file in '%s' for cell %d.\n",
                name,cell);
        free(name);
        fclose(file);
        return 1;
      }
      if(soilcode>=config->soilmap_size)
      {
        fprintf(stderr,"ERROR250: Invalid soilcode %u of cell %d in '%s', must be in [0,%d].\n",
                soilcode,cell,name,config->soilmap_size-1);
        free(name);
        fclose(file);
        return 1;
      }
      exist[soilcode]=TRUE;
    }
    fclose(file);
    for(i=0;i<config->soilmap_size;i++)
      if(!exist[i] && config->soilmap[i]!=0)
        fprintf(stderr,"WARNING035: Soilcode %u ('%s') not found in '%s'.\n",
                i,config->soilpar[config->soilmap[i]-1].name,name);
    free(name);
  }
  return 0;
} /* of 'checksoilcode' */

static int checkfile(const Config *config,const char *name,const Filename *filename)
{
  if(filename->fmt==FMS)
    return 0;
  if(filename->fmt==SOCK)
  {
    if(config->start_coupling<=config->firstyear-config->nspinup)
      return 0;
    fprintf(stderr,"ERROR149: No filename specified for %s data required for socket connection before coupling year %d, first simulatiomn year=%d.\n",
            name,config->start_coupling,config->firstyear-config->nspinup);
    return 1;
  }
  if(getfilesize(filename->name)==-1)
  {
    printfopenerr(filename->name);
    return 1;
  }
  else
    return 0;
} /* of 'checkfile' */

static int checkinputdata(const Config *config,const Filename *filename,const char *name,const char *unit,Type datatype)
{
  Infile file;
  if(openinputdata(&file,filename,name,unit,datatype,1,config))
    return 1;
  closeinput(&file);
  return 0;
} /* of 'checkinputdata' */

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
      fprintf(stderr,"ERROR218: Number of bands %d in '%s' is not %zu.\n",header.nbands,filename->name,len);
      return 1;
    }
  }
  return 0;
} /* of 'checkinputfile' */

static int checklanduse(const Config *config)
{
  Climatefile landuse;
  if(config->landuse_filename.fmt==SOCK)
  {
    if(config->start_coupling<=config->firstyear-config->nspinup)
      return 0;
    fprintf(stderr,"ERROR149: No filename specified for landuse data required for socket connection before coupling year %d, first simulatiomn year=%d.\n",
            config->start_coupling,config->firstyear-config->nspinup);
    return 1;
  }
 /* open landuse input data */
  if(opendata_seq(&landuse,&config->landuse_filename,"landuse","1",LPJ_SHORT,0.001,2*config->landusemap_size,FALSE,config))
  {
    return 1;
  }
  if(landuse.var_len!=2*config->landusemap_size && landuse.var_len!=4*config->landusemap_size)
  {
    fprintf(stderr,
            "ERROR147: Invalid number of bands=%zu in landuse data file, must be %d or %d.\n",
            landuse.var_len,2*config->landusemap_size,4*config->landusemap_size);
    closeclimatefile(&landuse,TRUE);
    return 1;
  }
  if(landuse.var_len!=4*config->landusemap_size)
    fputs("WARNING024: Land-use input does not include irrigation systems, suboptimal country values are used.\n",stderr);
  closeclimatefile(&landuse,TRUE);
  return 0;
} /* of 'checklanduse' */

static int checkdatafile(const Config *config,const Filename *filename,char *name,char *unit,Type datatype,int nbands)
{
  Climatefile input;
  /* open input data */
  if(filename->fmt==SOCK)
    return 0;
  if(opendata_seq(&input,filename,name,unit,datatype,1,nbands,TRUE,config))
    return 1;
  closeclimatefile(&input,TRUE);
  return 0;
} /* of 'checkdatafile' */

static int checkclmfile(const Config *config,const char *data_name,const Filename *filename,const char *unit,Bool check)
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
  if(filename->fmt==SOCK)
  {
    if(config->start_coupling<=config->firstyear-config->nspinup)
      return 0;
    fprintf(stderr,"ERROR149: No filename specified for %s data required for socket connection before coupling year %d, first simulatiomn year=%d.\n",
            data_name,config->start_coupling,config->firstyear-config->nspinup);
    return 1;
  }
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
      if(openclimate_netcdf(&input,filename->name,filename->time,filename->var,filename->unit,unit,config))
        return 1;
      closeclimate_netcdf(&input,TRUE);
      if(check)
      {
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
        else if(config->fix_climate && input.firstyear+input.nyear-1<max(config->fix_climate_year,config->fix_climate_interval[1]))
        {
          fprintf(stderr,"ERROR237: Last year=%d in '%s' is less than last simulation year %d.\n",input.firstyear+input.nyear-1,filename->name,max(config->fix_climate_year,config->fix_climate_interval[1]));
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
      if(!config->fix_climate && header.firstyear+(header.nyear-1)*config->delta_year<config->lastyear)
      {
        fprintf(stderr,"ERROR237: Last year=%d in '%s' is less than last simulation year %d.\n",header.firstyear+(header.nyear-1)*config->delta_year,filename->name,config->lastyear);
        return 1;
      }
      else if(config->fix_climate && header.firstyear+header.nyear-1<max(config->fix_climate_year,config->fix_climate_interval[1]))
      {
        fprintf(stderr,"ERROR237: Last year=%d in '%s' is less than last simulation year %d.\n",header.firstyear+header.nyear-1,filename->name,max(config->fix_climate_year,config->fix_climate_interval[1]));
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
    fprintf(stderr,"ERROR240: Number of cells=%d in soil code file '%s' too short, must be %d.\n",
            size,filename->name,config->nall+config->firstgrid);
    return 1;
  }
  return 0;
} /* of 'checksoilfile' */

static int checkdir(const char *path)
{
  if(getfilesize(path)==-1)
  {
    fprintf(stderr,"ERROR100: Directory '%s' cannot be openend: %s.\n",path,
            strerror(errno));
    return 1;
  }
  else if(!isdir(path))
  {
    fprintf(stderr,"ERROR241: '%s' is not a directory.\n",path);
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
    if(checksoilfile(&config,&config.soil_filename))
      bad++;
    else
      bad+=checksoilcode(&config);
  }
  else
    bad+=checkcoordfile(&config,&config.soil_filename);
  bad+=checkinputfile(&config, &config.kbf_filename,NULL,0);
  bad+=checkinputfile(&config, &config.slope_filename,NULL,0);
  bad+=checkinputfile(&config, &config.slope_min_filename,NULL,0);
  bad+=checkinputfile(&config, &config.slope_max_filename,NULL,0);
  if(config.landfrac_from_file)
    bad+=checkinputdata(&config,&config.landfrac_filename,"landfrac","1",LPJ_SHORT);
  if(config.with_lakes)
    bad+=checkinputdata(&config,&config.lakes_filename,"lakes","1",LPJ_SHORT);
  if(config.river_routing)
  {
    if(config.extflow)
      bad+=checkclmfile(&config,"extflow",&config.extflow_filename,NULL,0);
    bad+=checkinputfile(&config,&config.drainage_filename,NULL,(config.drainage_filename.fmt==CDF) ? 0 : 2);
    if(config.withlanduse!=NO_LANDUSE)
      bad+=checkinputdata(&config,&config.neighb_irrig_filename,"neigbour irrigation",NULL,LPJ_INT);
  }
  if(config.ispopulation)
    bad+=checkdatafile(&config,&config.popdens_filename,"popdens","km-2",LPJ_SHORT,1);
  if(config.with_nitrogen)
  {
    if(config.with_nitrogen==LIM_NITROGEN && !config.no_ndeposition)
    {
      bad+=checkclmfile(&config,"NO3 deposition",&config.no3deposition_filename,"g/m2/day",FALSE);
      bad+=checkclmfile(&config,"NH4 deposition",&config.nh4deposition_filename,"g/m2/day",FALSE);
    }
    bad+=checkinputdata(&config,&config.soilph_filename,"soilPH",NULL,LPJ_SHORT);
  }
  if(config.grassfix_filename.name!=NULL)
    bad+=checkinputdata(&config,&config.grassfix_filename,"grassfix",NULL,LPJ_SHORT);
  if(config.grassharvest_filename.name!=NULL)
    bad+=checkinputdata(&config,&config.grassharvest_filename,"grassharvest",NULL,LPJ_SHORT);
  if(config.with_nitrogen || config.fire==SPITFIRE || config.fire==SPITFIRE_TMAX)
    bad+=checkclmfile(&config,"wind speed",&config.wind_filename,"m/s",TRUE);
  if(config.fire==SPITFIRE || config.fire==SPITFIRE_TMAX)
  {
    if(config.fdi==WVPD_INDEX)
      bad+=checkclmfile(&config,"humidity",&config.humid_filename,NULL,TRUE);
    bad+=checkdatafile(&config,&config.lightning_filename,"lightning",NULL,LPJ_INT,12);
    bad+=checkclmfile(&config,"human ignition",&config.human_ignition_filename,"yr-1",TRUE);
  }
  if(config.cropsheatfrost || config.fire==SPITFIRE_TMAX)
  {
    bad+=checkclmfile(&config,"tmin",&config.tmin_filename,"celsius",TRUE);
    bad+=checkclmfile(&config,"tmax",&config.tmax_filename,"celsius",TRUE);
  }
  if(config.fire==SPITFIRE)
  {
    bad+=checkclmfile(&config,"tamp",&config.tamp_filename,NULL,TRUE);
  }
  if(config.wateruse)
    bad+=checkdatafile(&config,&config.wateruse_filename,"wateruse","dm3/yr",LPJ_INT,1);
  bad+=checkclmfile(&config,"temp",&config.temp_filename,"celsius",TRUE);
  bad+=checkclmfile(&config,"precipitation",&config.prec_filename,"kg/m2/day",TRUE);
#ifdef IMAGE
  if (config.wateruse_wd_filename.name != NULL)
    bad += checkdatafile(&config, &config.wateruse_wd_filename,"wateruse_wd","dm3/yr",LPJ_INT,1);
#endif
  if(config.with_radiation)
  {
    if(config.with_radiation==RADIATION || config.with_radiation==RADIATION_LWDOWN)
      bad+=checkclmfile(&config,"lwnet",&config.lwnet_filename,"W/m2",TRUE);
    bad+=checkclmfile(&config,"swdown",&config.swdown_filename,"W/m2",TRUE);
  }
  else
    bad+=checkclmfile(&config,"cloudiness",&config.cloud_filename,"%",TRUE);
  bad+=checkfile(&config,"co2",&config.co2_filename);
  if(config.wet_filename.name!=NULL)
    bad+=checkclmfile(&config,"wet days",&config.wet_filename,"day",FALSE);
#ifdef IMAGE
  if(config.sim_id==LPJML_IMAGE)
  {
    bad+=checkclmfile(&config,"temp var",&config.temp_var_filename,NULL,FALSE);
    bad+=checkclmfile(&config,"prec var",&config.prec_var_filename,NULL,FALSE);
    bad+=checkclmfile(&config,"prodpool",&config.prodpool_init_filename,NULL,FALSE);
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
    bad+=checklanduse(&config);
    if(config.sdate_option==PRESCRIBED_SDATE)
      bad+=checkdatafile(&config,&config.sdate_filename,"sowing",NULL,LPJ_SHORT,2*config.cftmap_size);
    if(config.iscotton)
    {
      bad+=checkinputdata(&config,&config.sowing_cotton_rf_filename,"sowing cotton rf",NULL,LPJ_SHORT);
      bad+=checkinputdata(&config,&config.harvest_cotton_rf_filename,"harvest cotton rf",NULL,LPJ_SHORT);
      bad+=checkinputdata(&config,&config.sowing_cotton_ir_filename,"sowing cotton ir",NULL,LPJ_SHORT);
      bad+=checkinputdata(&config,&config.harvest_cotton_ir_filename,"harvest cotton ir",NULL,LPJ_SHORT);
    }
    if(config.sdate_option==PRESCRIBED_SDATE)
      bad+=checkinputfile(&config,&config.sdate_filename,NULL,TRUE);
    if(config.crop_phu_option==PRESCRIBED_CROP_PHU)
      bad+=checkdatafile(&config,&config.crop_phu_filename,"crop phu",NULL,LPJ_SHORT,2*config.cftmap_size);
    if(config.countrycode_filename.fmt==CDF)
    {
      bad+=checkinputfile(&config,&config.countrycode_filename,NULL,0);
      bad+=checkinputfile(&config,&config.regioncode_filename,NULL,0);
    }
    else
      bad+=checkinputfile(&config,&config.countrycode_filename,NULL,2);
    if(config.reservoir)
    {
      bad+=checkinputdata(&config,&config.elevation_filename,"elevation","m",LPJ_SHORT);
      bad+=checkinputfile(&config,&config.reservoir_filename,NULL,10);
    }
    if(config.with_nitrogen&& config.fertilizer_input==FERTILIZER &&!config.fix_fertilization)
      bad+=checkdatafile(&config,&config.fertilizer_nr_filename,"fertilizer","g/m2",LPJ_SHORT,2*config.fertilizermap_size);
#ifdef IMAGE
    if(config.aquifer_irrig==AQUIFER_IRRIG)
    {
      bad+=checkinputfile(&config,&config.aquifer_filename,NULL,0);
    }
#endif
    if (config.with_nitrogen&&config.manure_input&&!config.fix_fertilization)
      bad+=checkdatafile(&config,&config.manure_nr_filename,"manure","g/m2",LPJ_SHORT,2*config.fertilizermap_size);
    if(config.tillage_type==READ_TILLAGE)
      bad+=checkdatafile(&config,&config.with_tillage_filename,"tillage",NULL,LPJ_SHORT,1);
    if(config.residue_treatment==READ_RESIDUE_DATA)
      bad+=checkdatafile(&config,&config.residue_data_filename,"residue extraction",NULL,LPJ_SHORT,config.fertilizermap_size);
    if(config.prescribe_lsuha)
      bad+=checkinputdata(&config,&config.lsuha_filename,"livestock density","lsu/ha",LPJ_SHORT);
  }
  badout=0;
  oldpath=strdup("");
  if(config.nall!=-1 && config.n_out)
    size=outputfilesize(&config);
  for(i=0;i<config.n_out;i++)
  {
    if(config.outputvars[i].filename.fmt!=SOCK)
    {
      path=getpath(config.outputvars[i].filename.name);
      if(strcmp(path,oldpath))
      {
        if(checkdir(path))
          badout++;
        else if(config.nall!=-1 && diskfree(path)<size)
          fprintf(stderr,"WARNING033: Disk space on '%s' is insufficient for output files.\n",path);
        free(oldpath);
        oldpath=path;
      }
      else
        free(path);
    }
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

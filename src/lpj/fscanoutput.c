/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  o  u  t  p  u  t  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads output file names and types from a configuration            \n**/
/*      file                                                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return TRUE;}
#define fscanint2(file,var,name) if(fscanint(file,var,name,FALSE,verbosity)) return TRUE;
#define fscanbool2(file,var,name) if(fscanbool(file,var,name,FALSE,verbosity)) return TRUE;

static Bool isopenoutput(int id,const Outputvar output[],int n)
{
  /* checks whether output file has already been specified */
  int i;
  for(i=0;i<n;i++)
    if(id==output[i].id) /* id already set? */
      return TRUE; /* yes, return TRUE */
  return FALSE; /* not found */
} /* of 'isopenoutput' */

static int findid(const char *name,const Variable var[],int size)
{
  int i;
  for(i=0;i<size;i++)
    if(!strcmp(name,var[i].name))
      return i;
  return NOT_FOUND;
} /* of 'findid' */

Bool fscanoutput(LPJfile *file,  /**< pointer to LPJ file */
                 int npft,       /**< number of natural PFTs */
                 int ncft,       /**< number of crop PFTs */
                 Config *config, /**< LPJ configuration */
                 int nout_max    /**< maximum number of output files */
                )                /** \return TRUE on error */
{
  LPJfile *arr,*item;
  int count,flag,size,index,version,default_fmt;
  Bool metafile,b;
  const char *outpath,*name;
  Verbosity verbosity;
  String s,s2;
  char *default_suffix;
  verbosity=isroot(*config) ? config->scan_verbose : NO_ERR;
  name=fscanstring(file,NULL,"compress_cmd",verbosity);
  if(name==NULL)
    return TRUE;
  config->compress_cmd=strdup(name);
  checkptr(config->compress_cmd);
  name=fscanstring(file,NULL,"compress_suffix",verbosity);
  if(name==NULL)
    return TRUE;
  config->compress_suffix=strdup(name);
  checkptr(config->compress_suffix);
  config->coupler_out=0;
  if(config->compress_suffix[0]!='.')
  {
    if(verbosity)
      fprintf(stderr,"ERROR251: Suffix '%s' must start with '.'.\n",config->compress_suffix);
    return TRUE;
  }
  name=fscanstring(file,NULL,"csv_delimit",verbosity);
  if(name==NULL)
    return TRUE;
  if(strlen(name)!=1)
  {
    if(verbosity)
      fprintf(stderr,"ERROR252: Delimiter '%s' must be one character.\n",name);
    return TRUE;
  }
  config->csv_delimit=name[0];
  config->outputvars=newvec(Outputvar,nout_max);
  checkptr(config->outputvars);
  count=index=0;
  config->withdailyoutput=FALSE;
  size=nout_max;
  config->json_filename=NULL;
  if(iskeydefined(file,"outpath") && !isnull(file,"outpath"))
  {
    outpath=fscanstring(file,NULL,"outpath",verbosity);
    if(outpath==NULL)
      return TRUE;
    free(config->outputdir);
    config->outputdir=strdup(outpath);
    checkptr(config->outputdir);
  }
  if(!config->nopp)
  {
    if(iskeydefined(file,"json_config_filename") && !isnull(file,"json_config_filename"))
    {
      name=fscanstring(file,NULL,"json_config_filename",verbosity);
      if(name==NULL)
        return TRUE;
      config->json_filename=addpath(name,config->outputdir);
      checkptr(config->json_filename);
      if(!strcmp(config->filename,config->json_filename))
      {
        if(verbosity)
          fprintf(stderr,"ERROR262: Filename of processed JSON file '%s' is identical to configuration filename, no file written.\n",
                  config->json_filename);
        if(config->pedantic)
          return TRUE;
        free(config->json_filename);
        config->json_filename=NULL;
      }
    }
  }
  if(!iskeydefined(file,"output") && !isnull(file,"output"))
  {
    config->pft_output_scaled=FALSE;
    config->n_out=0;
    config->json_suffix=NULL;
    return FALSE;
  }
  arr=fscanarray(file,&size,"output",verbosity);
  if(arr==NULL)
  {
    config->n_out=0;
    return TRUE;
  }
  metafile=FALSE;
  if(fscanbool(file,&metafile,"output_metafile",!config->pedantic,verbosity))
    return TRUE;
  default_fmt=RAW;
  if(fscankeywords(file,&default_fmt,"default_fmt",fmt,N_FMT,!config->pedantic,verbosity))
    return TRUE;
  name=fscanstring(file,"","default_suffix",verbosity);
  if(name==NULL)
    return TRUE;
  default_suffix=strdup(name);
  version=0;
  if(iskeydefined(file,"output_version"))
  {
    if(fscanint(file,&version,"output_version",FALSE,verbosity))
      return TRUE;
    if(version<1 || version>CLM_MAX_VERSION)
    {
      if(verbosity)
        fprintf(stderr,"ERROR229: Invalid version %d, must be in [1,%d].\n",
                version,CLM_MAX_VERSION);
      return TRUE;
    }
  }
  fscanbool2(file,&config->nofill,"nofill");
  config->isnetcdf4=FALSE;
  if(fscanbool(file,&config->isnetcdf4,"netcdf4",!config->pedantic,verbosity))
    return TRUE;
  config->global_netcdf=FALSE;
  if(iskeydefined(file,"global_netcdf"))
  {
    if(fscanbool(file,&config->global_netcdf,"global_netcdf",FALSE,verbosity))
      return TRUE;
  }
  config->flush_output=FALSE;
  if(fscanbool(file,&config->flush_output,"flush_output",!config->pedantic,verbosity))
    return TRUE;
  config->rev_lat=FALSE;
  if(fscanbool(file,&config->rev_lat,"rev_lat",!config->pedantic,verbosity))
    return TRUE;
  config->with_days=TRUE;
  if(fscanbool(file,&config->with_days,"with_days",!config->pedantic,verbosity))
    return TRUE;
  config->grid_type=LPJ_SHORT;
  if(iskeydefined(file,"float_grid"))
  {
    if(fscanbool(file,&b,"float_grid",FALSE,verbosity))
      return TRUE;
    if(b)
      config->grid_type=LPJ_FLOAT;
  }
  if(iskeydefined(file,"grid_type"))
  {
    if(fscankeywords(file,(int *)&config->grid_type,"grid_type",typenames,5,FALSE,verbosity))
      return TRUE;
    if(config->grid_type==LPJ_BYTE || config->grid_type==LPJ_INT)
    {
      if(verbosity)
        fprintf(stderr,"ERROR229: Invalid datatype %s for grid, must be short, float or double.\n",
                typenames[config->grid_type]);
      return TRUE;
    }
  }
  config->absyear=FALSE;
  if(fscanbool(file,&config->absyear,"absyear",!config->pedantic,verbosity))
    return TRUE;
  if(iskeydefined(file,"grid_scaled"))
  {
    fscanbool2(file,&config->pft_output_scaled,"grid_scaled");
  }
  else
  {
    fscanint2(file,&config->pft_output_scaled,"pft_output_scaled");
  }
  name=fscanstring(file,NULL,"json_suffix",verbosity);
  if(name==NULL)
    return TRUE;
  config->json_suffix=strdup(name);
  checkptr(config->json_suffix);
  for(index=0;index<size;index++)
  {
    item=fscanarrayindex(arr,index);
    if(isstring(item,"id"))
    {
      name=fscanstring(item,NULL,"id",verbosity);
      flag=findid(name,config->outnames,nout_max);
      if(flag==NOT_FOUND)
      {
        if(verbosity)
          fprintf(stderr,"ERROR166: Id '%s' not defined for output file, output is ignored.\n",name);
        if(config->pedantic)
          return TRUE;
        continue;
      }
    }
    else
    {
      fscanint2(item,&flag,"id");
    }
    config->outputvars[count].filename.meta=metafile;
    config->outputvars[count].filename.issocket=FALSE;
    config->outputvars[count].filename.id=flag;
    if(version>0)
      config->outputvars[count].filename.version=version;
    else
      config->outputvars[count].filename.version=(flag==GRID) ? LPJGRID_VERSION : LPJOUTPUT_VERSION;
    config->outputvars[count].filename.fmt=default_fmt;
    if(readfilename(item,&config->outputvars[count].filename,"file",config->outputdir,FALSE,FALSE,FALSE,verbosity))
    {
      if(verbosity)
        fprintf(stderr,"ERROR231: Cannot read filename for output '%s'.\n",
                config->outnames[flag].name);
      return TRUE;
    }
    if(strlen(default_suffix)>0 && !hasanysuffix(config->outputvars[count].filename.name))
    {
      config->outputvars[count].filename.name=realloc(config->outputvars[count].filename.name,strlen(config->outputvars[count].filename.name)+strlen(default_suffix)+1);
      checkptr(config->outputvars[count].filename.name);
      strcat(config->outputvars[count].filename.name,default_suffix);
    }
    if(isopenoutput(flag,config->outputvars,count))
    {
      if(verbosity)
        fprintf(stderr,"WARNING006: Output file for '%s' is opened twice, will be ignored.\n",
                config->outnames[flag].name);
      if(config->pedantic)
        return TRUE;
      freefilename(&config->outputvars[count].filename);
    }
    else if(outputsize(flag,npft,ncft,config)==0)
    {
      if(verbosity)
        fprintf(stderr,"WARNING006: Number of bands in output file for '%s' is zero, will be ignored.\n",
                config->outnames[flag].name);
      freefilename(&config->outputvars[count].filename);
    }
    else if(!config->with_nitrogen && isnitrogen_output(flag))
    {
      if(verbosity)
        fprintf(stderr,"WARNING006: Output file for '%s' is nitrogen output but nitrogen is not enabled, will be ignored.\n",
                config->outnames[flag].name);
      freefilename(&config->outputvars[count].filename);
    }
    else if(config->outputvars[count].filename.fmt==CLM2)
    {
      if(verbosity)
        fprintf(stderr,"ERROR223: File format \"clm2\" is not supported for output file '%s', will be ignored.\n",
                config->outputvars[count].filename.name);
      if(config->pedantic)
        return TRUE;
      freefilename(&config->outputvars[count].filename);
    }
    else if(config->outputvars[count].filename.fmt==META)
    {
      if(verbosity)
        fprintf(stderr,"ERROR223: File format \"meta\" is not supported for output file '%s', will be ignored.\n",
                config->outputvars[count].filename.name);
      if(config->pedantic)
        return TRUE;
      freefilename(&config->outputvars[count].filename);
    }
    else
    {
      config->outputvars[count].id=flag;
      if(flag==GLOBALFLUX && config->outputvars[count].filename.fmt!=TXT)
      {
        if(verbosity)
          fprintf(stderr,"ERROR224: Invalid format '%s' for 'globalflux' output, only 'txt' allowed.\n",
                  fmt[config->outputvars[count].filename.fmt]);
        return TRUE;
      }
      if(config->outputvars[count].filename.issocket)
        config->coupler_out++;
      if(config->outputvars[count].filename.var!=NULL)
      {
        free(config->outnames[flag].var);
        config->outnames[flag].var=strdup(config->outputvars[count].filename.var);
        checkptr(config->outnames[flag].var);
      }
      if(config->outputvars[count].filename.timestep!=NOT_FOUND)
      {
        if(config->outputvars[count].filename.timestep<getmintimestep(flag))
        {
          if(verbosity)
            fprintf(stderr,"ERROR246: Time step %s for '%s' output too short, must be %s.\n",
                    sprinttimestep(s,config->outputvars[count].filename.timestep),
                    config->outnames[flag].name,
                    sprinttimestep(s2,getmintimestep(flag)));
          if(config->pedantic)
            return TRUE;
        }
        else if(config->outputvars[count].filename.timestep!=ANNUAL && isannual_output(flag))
        {
          if(verbosity)
            fprintf(stderr,"ERROR246: Only annual time step allowed for '%s' output, time step is %s.\n",
                    config->outnames[flag].name,sprinttimestep(s2,config->outputvars[count].filename.timestep));
          if(config->pedantic)
            return TRUE;
        }
        config->outnames[flag].timestep=config->outputvars[count].filename.timestep;
      }

      if(config->outputvars[count].filename.unit!=NULL)
      {
        free(config->outnames[flag].unit);
        config->outnames[flag].unit=strdup(config->outputvars[count].filename.unit);
        checkptr(config->outnames[flag].unit);
        if(config->outnames[flag].unit!=NULL)
        {
          if(strstr(config->outnames[flag].unit,"/month")!=NULL)
            config->outnames[flag].time=MONTH;
          else if(strstr(config->outnames[flag].unit,"/yr")!=NULL)
            config->outnames[flag].time=YEAR;
          else if(strstr(config->outnames[flag].unit,"/day")!=NULL || strstr(config->outnames[flag].unit,"d-1")!=NULL)
            config->outnames[flag].time=DAY;
          else if(strstr(config->outnames[flag].unit,"/sec")!=NULL || strstr(config->outnames[flag].unit,"s-1")!=NULL)
            config->outnames[flag].time=SECOND;
          else
            config->outnames[flag].time=MISSING_TIME;
        }
      }
      if(config->outputvars[count].filename.isscale)
        config->outnames[flag].scale=(float)config->outputvars[count].filename.scale;
      if(config->outputvars[count].filename.fmt!=SOCK)
      {
        config->outputvars[count].oneyear=(strstr(config->outputvars[count].filename.name,"%d")!=NULL);
        if(config->outputvars[count].oneyear && checkfmt(config->outputvars[count].filename.name,'d'))
        {
          if(verbosity)
            fprintf(stderr,"ERROR224: Invalid format specifier in filename '%s'.\n",
                    config->outputvars[count].filename.name);
          if(config->pedantic)
            return TRUE;
        }
        else if(config->outputvars[count].oneyear && getnyear(config->outnames,flag)==0)
        {
          if(verbosity)
            fprintf(stderr,"ERROR225: One year output not allowed for grid, globalflux, country or region.\n");
          if(config->pedantic)
            return TRUE;
        }
      }
      else
        config->outputvars[count].oneyear=FALSE;
      if(config->outnames[flag].timestep==DAILY)
        config->withdailyoutput=TRUE;
      count++;
    }
  }
  free(default_suffix);
  config->n_out=count;
  return FALSE;
} /* of 'fscanoutput' */

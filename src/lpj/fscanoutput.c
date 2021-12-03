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

static int findpftid(const char *name,const Pftpar pftpar[],int ntotpft)
{
  int p;
  if(!strcmp(name,"allnatural"))
    return ALLNATURAL;
  else if(!strcmp(name,"allgrassland"))
    return ALLGRASSLAND;
  else if(!strcmp(name,"allstand"))
    return ALLSTAND;
  for(p=0;p<ntotpft;p++)
    if(!strcmp(name,pftpar[p].name))
      return pftpar[p].id;
  return NOT_FOUND;
} /* of 'findpftid' */

Bool fscanoutput(LPJfile *file,  /**< pointer to LPJ file */
                 int npft,       /**< number of natural PFTs */
                 int ncft,       /**< number of crop PFTs */
                 Config *config, /**< LPJ configuration */
                 int nout_max    /**< maximum number of output files */
                )                /** \return TRUE on error */
{
  LPJfile arr,item;
  int count,flag,size,index,ntotpft;
  Bool isdaily;
  String outpath,name;
  Verbosity verbosity;
  verbosity=isroot(*config) ? config->scan_verbose : NO_ERR;
  if(fscanstring(file,name,"compress_cmd",FALSE,verbosity))
    return TRUE;
  config->compress_cmd=strdup(name);
  checkptr(config->compress_cmd);
  if(fscanstring(file,name,"compress_suffix",FALSE,verbosity))
    return TRUE;
  config->compress_suffix=strdup(name);
  checkptr(config->compress_suffix);
  config->copan_out=0;
  if(config->compress_suffix[0]!='.')
  {
    if(verbosity)
      fprintf(stderr,"ERROR251: Suffix '%s' must start with '.'.\n",config->compress_suffix);
    return TRUE;
  }
  if(fscanstring(file,name,"csv_delimit",FALSE,verbosity))
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
  if(file->isjson && !iskeydefined(file,"output"))
  {
    config->pft_output_scaled=FALSE;
    config->n_out=0;
    config->crop_index=-1;
    config->crop_irrigation=-1;
    return FALSE;
  }
  if(fscanarray(file,&arr,&size,FALSE,"output",verbosity))
  {
    config->n_out=0;
    return file->isjson;
  }
  config->global_netcdf=FALSE;
  if(iskeydefined(file,"global_netcdf"))
  {
    if(fscanbool(file,&config->global_netcdf,"global_netcdf",FALSE,verbosity))
      return TRUE;
  }
  config->float_grid=FALSE;
  if(iskeydefined(file,"float_grid"))
  {
    if(fscanbool(file,&config->float_grid,"float_grid",FALSE,verbosity))
      return TRUE;
  }
  if(iskeydefined(file,"outpath"))
  {
    if(fscanstring(file,outpath,"outpath",FALSE,verbosity))
      return TRUE;
    free(config->outputdir);
    config->outputdir=strdup(outpath);
    checkptr(config->outputdir);
  }
  if(iskeydefined(file,"grid_scaled"))
  {
    fscanbool2(file,&config->pft_output_scaled,"grid_scaled");
  }
  else
  {
    fscanint2(file,&config->pft_output_scaled,"pft_output_scaled");
  }
  isdaily=FALSE;
  while(count<=nout_max && index<size)
  {
    fscanarrayindex(&arr,&item,index,verbosity);
    if(isstring(&item,"id"))
    {
      fscanstring(&item,name,"id",FALSE,verbosity);
      flag=findid(name,config->outnames,nout_max);
      if(flag==NOT_FOUND)
      {
        if(verbosity)
          fprintf(stderr,"ERROR166: Id '%s' not defined for output file, output is ignored.\n",name);
        index++;
        continue;
      }
    }
    else
    {
      fscanint2(&item,&flag,"id");
    }
    if(flag==END)  /* end marker read? */
      break;
    else if(count==nout_max)
    {
      if(verbosity)
        fprintf(stderr,"ERROR160: Maximum number %d of output files reached.\n",
                count);
      return TRUE;
    }
    else
    {
      if(readfilename(&item,&config->outputvars[count].filename,"file",config->outputdir,FALSE,verbosity))
      {
        if(verbosity)
          fprintf(stderr,"ERROR231: Cannot read filename for output '%s'.\n",
                  (flag<0 || flag>=nout_max) ?"N/A" : config->outnames[flag].name);
        return TRUE;
      }
      if(flag<0 || flag>=nout_max)
      {
        if(verbosity)
          fprintf(stderr,
                  "ERROR161: Invalid value=%d for index of output file '%s', must be in [0,%d].\n",
                  flag,config->outputvars[count].filename.name,nout_max-1);
      }
      else if(isopenoutput(flag,config->outputvars,count))
      {
        if(verbosity)
          fprintf(stderr,"WARNING006: Output file for '%s' is opened twice, will be ignored.\n",
                config->outnames[flag].name);
      }
      else if(outputsize(flag,npft,ncft,config)==0)
      {
        if(verbosity)
          fprintf(stderr,"WARNING006: Number of bands in output file for '%s' is zero, will be ignored.\n",
                config->outnames[flag].name);
      }
      else if(!config->with_nitrogen && isnitrogen_output(flag))
      {
        if(verbosity)
          fprintf(stderr,"WARNING006: Output file for '%s' is nitrogen output but nitrogen is not enabled, will be ignored.\n",
                config->outnames[flag].name);
      }
      else if(config->outputvars[count].filename.fmt==CLM2)
      {
        if(verbosity)
          fprintf(stderr,"ERROR223: File format CLM2 is not supported for output file '%s'.\n",
                  config->outputvars[count].filename.name);
        return TRUE;
      }
      else if(config->outputvars[count].filename.fmt==SOCK && config->sim_id!=LPJML_COPAN)
      {
        if(verbosity)
          fprintf(stderr,"ERROR223: File format 'sock' not allowd without COPAN coupling for outputt file '%s'.\n",
                  config->outputvars[count].filename.name);
        return TRUE;
      }
      else
      {
        if(flag>=D_LAI && flag<=D_PET)
          isdaily=TRUE;
        config->outputvars[count].id=flag;
        if(flag==GLOBALFLUX && config->outputvars[count].filename.fmt!=TXT)
        {
          if(verbosity)
            fprintf(stderr,"ERROR224: Invalid format '%s' for 'globalflux' output, only 'txt' allowed.\n",
                    fmt[config->outputvars[count].filename.fmt]);
          return TRUE;
        }
        if(config->outputvars[count].filename.var!=NULL)
        {
          free(config->outnames[flag].var);
          config->outnames[flag].var=strdup(config->outputvars[count].filename.var);
          checkptr(config->outnames[flag].var);
        }
        if(config->outputvars[count].filename.timestep!=NOT_FOUND)
          config->outnames[flag].timestep=config->outputvars[count].filename.timestep;
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
        if(config->outputvars[count].filename.fmt==SOCK)
        {
          config->outputvars[count].oneyear=FALSE;
          config->copan_out++;
        }
        else
          config->outputvars[count].oneyear=(strstr(config->outputvars[count].filename.name,"%d")!=NULL);
        if(config->outputvars[count].oneyear && checkfmt(config->outputvars[count].filename.name,'d'))
        {
          if(verbosity)
            fprintf(stderr,"ERROR224: Invalid format specifier in filename '%s'.\n",
                    config->outputvars[count].filename.name);
        }
        else if(config->outputvars[count].oneyear && (flag==GRID || flag==COUNTRY || flag==REGION || flag==GLOBALFLUX))
        {
          if(verbosity)
            fprintf(stderr,"ERROR225: One year output not allowed for grid, globalflux, country or region.\n");
        }
        else
        {
          if(config->outnames[flag].timestep==DAILY)
            config->withdailyoutput=TRUE;
          count++;
        }
      }
    }
    index++;
  }
  if(isdaily)
  {
    ntotpft=config->npft[GRASS]+config->npft[TREE]+config->npft[CROP];
    if(isstring(file,"crop_index"))
    {
      fscanstring(file,name,"crop_index",FALSE,verbosity);
      config->crop_index=findpftid(name,config->pftpar,ntotpft);
      if(config->crop_index==NOT_FOUND)
      {
        if(verbosity)
          fprintf(stderr,"ERROR166: Invalid crop index \"%s\" for daily output.\n",name);
        return TRUE;
      }
    }
    else
    {
      fscanint2(file,&config->crop_index,"crop_index");
      if(config->crop_index>=0 && config->crop_index<config->npft[CROP])
        config->crop_index+=config->npft[GRASS]+config->npft[TREE];
      else if((config->crop_index!=ALLNATURAL && config->crop_index!=ALLGRASSLAND && config->crop_index!=ALLSTAND))
      {
        if(verbosity)
          fprintf(stderr,"ERROR166: Invalid value for crop index=%d.\n",
                  config->crop_index);
        return TRUE;
      }
    }
    fscanbool2(file,&config->crop_irrigation,"crop_irrigation");
  }
  else
  {
    config->crop_index=-1;
    config->crop_irrigation=-1;
  }
  config->n_out=count;
  return FALSE;
} /* of 'fscanoutput' */

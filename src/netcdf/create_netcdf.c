/**************************************************************************************/
/**                                                                                \n**/
/**               c  r  e  a  t  e  _  n  e  t  c  d  f  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function creates NetCDF file for output                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#if defined(USE_NETCDF)
#include <netcdf.h>
#include <time.h>

static nc_type nctype[]={NC_BYTE,NC_SHORT,NC_INT,NC_FLOAT,NC_DOUBLE};
#endif

#define error(rc) if(rc) {if(cdf->state==ONEFILE || cdf->state==CLOSE) {free(lon);free(lat);free(year);}fprintf(stderr,"ERROR427: Cannot write '%s': %s.\n",filename,nc_strerror(rc)); nc_close(cdf->ncid); return TRUE;}

Bool create_netcdf(Netcdf *cdf,
                   const char *filename,     /**< filename of NetCDF file */
                   const char *name,         /**< name of output variable */
                   const char *standard_name, /**< standard name of output variable */
                   const char *long_name,    /**< long name of output variable */
                   const char *units,        /**< unit of output variable */
                   Type type,                /**< Type of output variable */
                   int n,                    /**< number of samples per year (0/1/12/365) */
                   int timestep,             /**< time step for annual output (yrs) */
                   int actualyear,           /**< actual year for oneyear output */
                   Bool oneyear,             /**< one file for each year (TRUE/FALSE) */
                   const Coord_array *array, /**< coordinate array */
                   const Config *config      /**< LPJ configuration */
                  )                          /** \return TRUE on error */
{
#if defined(USE_NETCDF)
  char *s;
  time_t t;
  int i,j,rc,nyear,imiss=MISSING_VALUE_INT,len;
  short smiss=MISSING_VALUE_SHORT;
  double *lon=NULL,*lat=NULL;
  float miss=config->missing_value;
  double *year=NULL;
  size_t chunk[3];
  int dim[3];
  if(array==NULL || name==NULL || filename==NULL)
  {
    fputs("ERROR424: Invalid array pointer in create_netcdf().\n",stderr);
    return TRUE;
  }
  cdf->missing_value=config->missing_value;
  cdf->index=array;
  if(cdf->state==APPEND || cdf->state==CLOSE)
  {
     cdf->ncid=cdf->root->ncid;
     cdf->time_dim_id=cdf->root->time_dim_id;
     cdf->lon_dim_id=cdf->root->lon_dim_id;
     cdf->lat_dim_id=cdf->root->lat_dim_id;
     cdf->time_var_id=cdf->root->time_var_id;
     cdf->lat_var_id=cdf->root->lat_var_id;
     cdf->lon_var_id=cdf->root->lon_var_id;
  }
  if(oneyear)
    nyear=1;
  else
  {
    nyear=config->lastyear-config->outputyear+1;
    if(config->ischeckpoint)
    {
      if(cdf->state==ONEFILE || cdf->state==CREATE)
      {
        /* start from checkpoint file, output files exist and have to be opened */
        rc=nc_open(filename,NC_WRITE|((config->isnetcdf4) ? NC_CLOBBER|NC_NETCDF4 : NC_CLOBBER),&cdf->ncid);
        if(rc)
        {
          fprintf(stderr,"ERROR426: Cannot open file '%s': %s.\n",
                  filename,nc_strerror(rc));
          return TRUE;
        }
      }
      if(config->nofill)
         ncsetfill(cdf->ncid,NC_NOFILL);
      /* get id of output variable */
      rc=nc_inq_varid(cdf->ncid,name,&cdf->varid);
      if(rc)
      {
        fprintf(stderr,"ERROR426: Cannot get variable '%s': %s.\n",
                name,nc_strerror(rc));
        return TRUE;
      }
      return FALSE;
    }
  }
  if(cdf->state==ONEFILE || cdf->state==CLOSE)
  {
    lon=newvec(double,array->nlon);
    if(lon==NULL)
    {
      printallocerr("lon");
      return TRUE;
    }
    lat=newvec(double,array->nlat);
    if(lat==NULL)
    {
      printallocerr("lat");
      free(lon);
      return TRUE;
    }
    for(i=0;i<array->nlon;i++)
      lon[i]=array->lon_min+i*config->resolution.lon;
    if(config->rev_lat)
      for(i=0;i<array->nlat;i++)
        lat[i]=array->lat_min+(array->nlat-1-i)*config->resolution.lat;
    else
      for(i=0;i<array->nlat;i++)
        lat[i]=array->lat_min+i*config->resolution.lat;
    if(n)
    {
      if(n==1 && oneyear)
        year=NULL;
      else
      {
        if(n==1)
          year=newvec(double,nyear/timestep);
        else
          year=newvec(double,nyear*n);
        if(year==NULL)
        {
          printallocerr("year");
          free(lon);
          free(lat);
          return TRUE;
        }
      }
    }
    else
      year=NULL;
    switch(n)
    {
      case 0:
        break;
      case 1:
        if(year!=NULL)
        {
          if(config->absyear)
            for(i=0;i<nyear/timestep;i++)
              year[i]=config->outputyear+i*timestep+timestep/2;
          else
            for(i=0;i<nyear/timestep;i++)
              year[i]=config->outputyear-config->baseyear+i*timestep+timestep/2;
        }
        break;
      case 12:
        if(config->with_days)
        {
          for(i=0;i<nyear;i++)
            for(j=0;j<12;j++)
              if(i==0 && j==0)
              {
                if(oneyear)
                  year[0]=ndaymonth[j]-1;
                else
                  year[0]=ndaymonth[j]-1+(config->outputyear-config->baseyear)*NDAYYEAR;
              }
              else
                year[i*12+j]=year[i*12+j-1]+ndaymonth[j];
        }
        else
        {
          if(oneyear)
            for(i=0;i<12;i++)
              year[i]=i;
          else
            for(i=0;i<nyear*12;i++)
              year[i]=(config->outputyear-config->baseyear)*NMONTH+i;
        }
        break;
      case NDAYYEAR:
        if(oneyear)
          for(i=0;i<n;i++)
            year[i]=i;
        else
          for(i=0;i<nyear*n;i++)
            year[i]=(config->outputyear-config->baseyear)*NDAYYEAR+i;
        break;
      default:
        fprintf(stderr,"ERROR425: Invalid value=%d for number of data points per year in '%s'.\n",n,filename);
        free(year);
        free(lon);
        free(lat);
        return TRUE;
    }
  }
  if(cdf->state==ONEFILE || cdf->state==CREATE)
  {
    rc=nc_create(filename,(config->isnetcdf4) ? NC_CLOBBER|NC_NETCDF4 : NC_CLOBBER,&cdf->ncid);
    if(rc)
    {
      fprintf(stderr,"ERROR426: Cannot create file '%s': %s.\n",
              filename,nc_strerror(rc));
      if(cdf->state==ONEFILE)
      {
        free(year);
        free(lon);
        free(lat);
      }
      return TRUE;
    }
    if(config->nofill)
      ncsetfill(cdf->ncid,NC_NOFILL);
    if(year!=NULL)
    {
      if(n==1)
        rc=nc_def_dim(cdf->ncid,TIME_DIM_NAME,nyear/timestep,&cdf->time_dim_id);
      else
        rc=nc_def_dim(cdf->ncid,TIME_DIM_NAME,nyear*n,&cdf->time_dim_id);
      error(rc);
      rc=nc_def_var(cdf->ncid,TIME_NAME,NC_DOUBLE,1,&cdf->time_dim_id,&cdf->time_var_id);
      error(rc);
      rc=nc_put_att_text(cdf->ncid, cdf->time_var_id,"standard_name",strlen(TIME_STANDARD_NAME),TIME_STANDARD_NAME);
      error(rc);
      rc=nc_put_att_text(cdf->ncid, cdf->time_var_id,"long_name",strlen(TIME_LONG_NAME),TIME_LONG_NAME);
      error(rc);
    }
    rc=nc_def_dim(cdf->ncid,LAT_DIM_NAME,array->nlat,&cdf->lat_dim_id);
    error(rc);
    rc=nc_def_dim(cdf->ncid,LON_DIM_NAME,array->nlon,&cdf->lon_dim_id);
    error(rc);
    rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"title",
                       strlen(config->sim_name),config->sim_name);
    error(rc);
    rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"source",strlen("LPJmL C Version " LPJ_VERSION),"LPJmL C Version " LPJ_VERSION);
    error(rc);
    time(&t);
    len=snprintf(NULL,0,"%s: %s",strdate(&t),config->arglist);
    s=malloc(len+1);
    sprintf(s,"%s: %s",strdate(&t),config->arglist);
    rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"history",strlen(s),s);
    free(s);
    error(rc);
    for(i=0;i<config->n_global;i++)
    {
      rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,config->global_attrs[i].name,strlen(config->global_attrs[i].value),config->global_attrs[i].value);
      error(rc);
    }
    rc=nc_def_var(cdf->ncid,LAT_NAME,NC_DOUBLE,1,&cdf->lat_dim_id,&cdf->lat_var_id);
    error(rc);
    rc=nc_def_var(cdf->ncid,LON_NAME,NC_DOUBLE,1,&cdf->lon_dim_id,&cdf->lon_var_id);
    error(rc);
    if(year!=NULL)
    {
      if(n==1)
      {
        if(config->absyear)
          s=strdup(YEARS_NAME);
        else
        {
          len=snprintf(NULL,0,"years since %d-1-1 0:0:0",(oneyear) ? actualyear : config->baseyear);
          s=malloc(len+1);
          sprintf(s,"years since %d-1-1 0:0:0",(oneyear) ? actualyear : config->baseyear);
        }
      }
      else if(n==12)
      {
        len=snprintf(NULL,0,"%s since %d-1-1 0:0:0",(config->with_days) ? "days" : "months",(oneyear) ? actualyear : config->baseyear);
        s=malloc(len+1);
        sprintf(s,"%s since %d-1-1 0:0:0",(config->with_days) ? "days" : "months",(oneyear) ? actualyear : config->baseyear);
      }
      else
      {
        len=snprintf(NULL,0,"days since %d-1-1 0:0:0",(oneyear) ? actualyear : config->baseyear);
        s=malloc(len+1);
        sprintf(s,"days since %d-1-1 0:0:0",(oneyear) ? actualyear : config->baseyear);
      }
      rc=nc_put_att_text(cdf->ncid,cdf->time_var_id,"units",strlen(s),s);
      free(s);
      error(rc);
      rc=nc_put_att_text(cdf->ncid,cdf->time_var_id,"calendar",strlen(CALENDAR),
                         CALENDAR);
      error(rc);
      rc=nc_put_att_text(cdf->ncid, cdf->time_var_id,"axis",strlen("T"),"T");
      error(rc);
    }
    rc=nc_put_att_text(cdf->ncid,cdf->lon_var_id,"units",strlen("degrees_east"),
                     "degrees_east");
    error(rc);
    rc=nc_put_att_text(cdf->ncid, cdf->lon_var_id,"long_name",strlen(LON_LONG_NAME),LON_LONG_NAME);
    error(rc);
    rc=nc_put_att_text(cdf->ncid, cdf->lon_var_id,"standard_name",strlen(LON_STANDARD_NAME),LON_STANDARD_NAME);
    error(rc);
    rc=nc_put_att_text(cdf->ncid, cdf->lon_var_id,"axis",strlen("X"),"X");
    error(rc);
    rc=nc_put_att_text(cdf->ncid,cdf->lat_var_id,"units",strlen("degrees_north"),
                     "degrees_north");
    error(rc);
    rc=nc_put_att_text(cdf->ncid, cdf->lat_var_id,"long_name",strlen(LAT_LONG_NAME),LAT_LONG_NAME);
    error(rc);
    rc=nc_put_att_text(cdf->ncid, cdf->lat_var_id,"standard_name",strlen(LAT_STANDARD_NAME),LAT_STANDARD_NAME);
    error(rc);
    rc=nc_put_att_text(cdf->ncid, cdf->lat_var_id,"axis",strlen("Y"),"Y");
    error(rc);
  }
  if(year!=NULL)
  {
    dim[0]=cdf->time_dim_id;
    dim[1]=cdf->lat_dim_id;
    dim[2]=cdf->lon_dim_id;
    chunk[0]=1;
    chunk[1]=array->nlat;
    chunk[2]=array->nlon;
  }
  else
  {
    dim[0]=cdf->lat_dim_id;
    dim[1]=cdf->lon_dim_id;
    chunk[0]=array->nlat;
    chunk[1]=array->nlon;
  }
  rc=nc_def_var(cdf->ncid,name,nctype[type],(year==NULL) ? 2 : 3,dim,&cdf->varid);
  error(rc);
  if(config->isnetcdf4)
  {
    rc=nc_def_var_chunking(cdf->ncid, cdf->varid, NC_CHUNKED,chunk);
    error(rc);
    if(config->compress)
    {
      rc=nc_def_var_deflate(cdf->ncid, cdf->varid, 0, 1, config->compress);
      error(rc);
    }
  }
  if(units!=NULL)
  {
    rc=nc_put_att_text(cdf->ncid, cdf->varid,"units",strlen(units),units);
    error(rc);
  }
  if(standard_name==NULL)
    rc=nc_put_att_text(cdf->ncid, cdf->varid,"standard_name",strlen(name),name);
  else
    rc=nc_put_att_text(cdf->ncid, cdf->varid,"standard_name",strlen(standard_name),standard_name);
  error(rc);
  if(long_name!=NULL)
  {
    rc=nc_put_att_text(cdf->ncid, cdf->varid,"long_name",strlen(long_name),long_name);
    error(rc);
  }
  switch(type)
  {
    case LPJ_FLOAT:
      nc_put_att_float(cdf->ncid, cdf->varid,"missing_value",NC_FLOAT,1,&miss);
      rc=nc_put_att_float(cdf->ncid, cdf->varid,"_FillValue",NC_FLOAT,1,&miss);
      break;
    case LPJ_SHORT:
      nc_put_att_short(cdf->ncid, cdf->varid,"missing_value",NC_SHORT,1,&smiss);
      rc=nc_put_att_short(cdf->ncid, cdf->varid,"_FillValue",NC_SHORT,1,&smiss);
      break;
    case LPJ_INT:
      nc_put_att_int(cdf->ncid, cdf->varid,"missing_value",NC_INT,1,&imiss);
      rc=nc_put_att_int(cdf->ncid, cdf->varid,"_FillValue",NC_INT,1,&imiss);
      break;
    default:
      fputs("ERROR428: Invalid data type in NetCDF file.\n",stderr);
      free(lat);
      free(lon);
      free(year);
      return TRUE;

  }
  error(rc);
  if(cdf->state==ONEFILE || cdf->state==CLOSE)
  {
    rc=nc_enddef(cdf->ncid);
    error(rc);
    if(year!=NULL)
    {
      rc=nc_put_var_double(cdf->ncid,cdf->time_var_id,year);
      error(rc);
    }
    rc=nc_put_var_double(cdf->ncid,cdf->lat_var_id,lat);
    error(rc);
    rc=nc_put_var_double(cdf->ncid,cdf->lon_var_id,lon);
    error(rc);
    free(lat);
    free(lon);
    free(year);
  }
  return FALSE;
#else
  fputs("ERROR401: NetCDF output is not supported by this version of LPJmL.\n",stderr);
  return TRUE;
#endif
} /* of 'create_netcdf' */

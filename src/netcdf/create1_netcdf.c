/**************************************************************************************/
/**                                                                                \n**/
/**              c  r  e  a  t  e  1  _  n  e  t  c  d  f  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function creates NetCDF file for yearly output                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
#include <netcdf.h>
#include <time.h>

static nc_type nctype[]={NC_BYTE,NC_SHORT,NC_INT,NC_FLOAT,NC_DOUBLE};
#endif

#define error(rc) if(rc) {if(cdf->state==ONEFILE || cdf->state==CLOSE) {free(lon);free(lat);free(days);}fprintf(stderr,"ERROR427: Cannot write '%s': %s.\n",filename,nc_strerror(rc)); nc_close(cdf->ncid); return TRUE;}

Bool create1_netcdf(Netcdf *cdf,
                    const char *filename, /**< filename of NetCDF file */
                    const char *name, /**< name of output variable */
                    const char *descr, /**< description of output variable */
                    const char *units, /**< unit of output variable */
                    Type type, /**< Type of output variable */
                    int n, /**< number of samples per year (0/1/12/365) */ 
                    const Coord_array *array, /**< coordinate array */
                    int year, /**< simulation year (AD) */
                    const Config *config /**< LPJ configuration */
                   ) /** \return TRUE on error */
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  String s;
  time_t t;
  int i,rc,imiss=MISSING_VALUE_INT;
  short smiss=MISSING_VALUE_SHORT;
  double *lon,*lat;
  float miss=config->missing_value;
  double *days;
  int dim[3];
  size_t chunk[3];
  if(array==NULL || name==NULL || filename==NULL)
  {
    fputs("ERROR424: Invalid array pointer in create1_netcdf().\n",stderr);
    return TRUE;
  }
  cdf->missing_value=config->missing_value;
  if(cdf->state==APPEND || cdf->state==CLOSE)
  {
     cdf->ncid=cdf->root->ncid;
     cdf->time_dim_id=cdf->root->time_dim_id;
     cdf->lon_dim_id=cdf->root->lon_dim_id;
     cdf->lat_dim_id=cdf->root->lat_dim_id;
     cdf->time_var_id=cdf->root->time_var_id;
     cdf->lat_var_id=cdf->root->lat_var_id;
     cdf->lon_var_id=cdf->root->lon_var_id;
     cdf->n=cdf->root->n;
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
    if(cdf->state==ONEFILE)
      cdf->n=1;
    if(max(n,cdf->n)>1)
    {
      days=newvec(double,max(n,cdf->n));
      if(days==NULL)
      {
        printallocerr("days");
        free(lon);
        free(lat);
        return TRUE;
      }
    }
    else
      days=NULL;
    switch(max(n,cdf->n))
    {
      case 1:
        break;
      case 12:
        for(i=0;i<12;i++)
          days[i]=i;
        break;
      case NDAYYEAR:
        for(i=0;i<max(n,cdf->n);i++)
          days[i]=i;
        break;
      default:
        fprintf(stderr,"ERROR425: Invalid value=%d for number of data points per year.\n",
                max(n,cdf->n));
        free(days);
        free(lon);
        free(lat);
        return TRUE;
    }
  }
  cdf->index=array;
  if(cdf->state==ONEFILE || cdf->state==CREATE)
  {
#ifdef USE_NETCDF4
    rc=nc_create(filename,NC_CLOBBER|NC_NETCDF4,&cdf->ncid);
#else
    rc=nc_create(filename,NC_CLOBBER,&cdf->ncid);
#endif
    if(rc)
    {
      fprintf(stderr,"ERROR426: Cannot create file '%s': %s.\n",
              filename,nc_strerror(rc));
      if(cdf->state==ONEFILE)
      {
        free(days);
        free(lon);
        free(lat);
      }
      return TRUE;
    }
    cdf->n=1;
  }
  if(n>cdf->n)
  {
    rc=nc_def_dim(cdf->ncid,TIME_DIM_NAME,n,&cdf->time_dim_id);
    error(rc);
    rc=nc_def_var(cdf->ncid,"time",NC_DOUBLE,1,&cdf->time_dim_id,&cdf->time_var_id);
    error(rc);
    rc=nc_put_att_text(cdf->ncid, cdf->time_var_id,"long_name",strlen(TIME_LONG_NAME),TIME_LONG_NAME);
    error(rc);
    cdf->n=n;
    if(cdf->state!=ONEFILE && cdf->state!=CREATE)
    {
      cdf->root->time_dim_id=cdf->time_dim_id;
      cdf->root->n=n;
      cdf->root->time_var_id=cdf->time_var_id;
    } 
  }
  if(cdf->state==ONEFILE || cdf->state==CREATE)
  {
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
    snprintf(s,STRING_LEN,"Created for user %s on %s at %s: %s",
             getuser(),gethost(),strdate(&t),config->arglist);
    rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"history",strlen(s),s);
    error(rc);
    for(i=0;i<config->n_global;i++)
    {
      rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,config->global_attrs[i].name,strlen(config->global_attrs[i].value),config->global_attrs[i].value);
      error(rc);
    }
  }
  if(n>1)
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
  if(cdf->state==ONEFILE || cdf->state==CREATE)
  {
    rc=nc_def_var(cdf->ncid,LAT_NAME,NC_DOUBLE,1,&cdf->lat_dim_id,&cdf->lat_var_id);
    error(rc);
    rc=nc_def_var(cdf->ncid,LON_NAME,NC_DOUBLE,1,&cdf->lon_dim_id,&cdf->lon_var_id);
    error(rc);
    if(n>1)
    {
      if(n==NMONTH)
        snprintf(s,STRING_LEN,"months since %d-1-1 0:0:0",year);
      else
        snprintf(s,STRING_LEN,"days since %d-1-1 0:0:0",year);
      rc=nc_put_att_text(cdf->ncid,cdf->time_var_id,"units",strlen(s),s);
      error(rc);
      rc=nc_put_att_text(cdf->ncid,cdf->time_var_id,"calendar",strlen("noleap"),
                         "noleap");
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
  rc=nc_def_var(cdf->ncid,name,nctype[type],(n==1) ? 2 : 3,dim,&cdf->varid);
  error(rc);
#ifdef USE_NETCDF4
  rc=nc_def_var_chunking(cdf->ncid, cdf->varid, NC_CHUNKED,chunk);
  error(rc);
  if(config->compress)
  {
    rc=nc_def_var_deflate(cdf->ncid, cdf->varid, 0, 1, config->compress);
    error(rc);
  }
#endif
  if(units!=NULL)
  {
    rc=nc_put_att_text(cdf->ncid, cdf->varid,"units",strlen(units),units);
    error(rc);
  }
  rc=nc_put_att_text(cdf->ncid, cdf->varid,"standard_name",strlen(name),name);
  error(rc);
  if(descr!=NULL)
  {
    rc=nc_put_att_text(cdf->ncid, cdf->varid,"long_name",strlen(descr),descr);
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
  }
  error(rc);
  if(cdf->state==ONEFILE || cdf->state==CLOSE)
  {
    rc=nc_enddef(cdf->ncid);
    error(rc);
    if(cdf->n>1)
    {
      rc=nc_put_var_double(cdf->ncid,cdf->time_var_id,days);
      error(rc);
    }
    rc=nc_put_var_double(cdf->ncid,cdf->lat_var_id,lat);
    error(rc);
    rc=nc_put_var_double(cdf->ncid,cdf->lon_var_id,lon);
    error(rc);
    free(lat);
    free(lon);
    free(days);
  }
  return FALSE;
#else
  fputs("ERROR401: NetCDF output is not supported by this version of LPJmL.\n",stderr);
  return TRUE;
#endif
} /* of 'create1_netcdf' */

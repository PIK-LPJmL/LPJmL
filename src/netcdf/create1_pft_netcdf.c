/**************************************************************************************/
/**                                                                                \n**/
/**      c  r  e  a  t  e  1  _  p  f  t  _  n  e  t  c  d  f  .  c                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function creates NetCDF file for PFT output                                \n**/
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

#define error(rc) if(rc) {free(lon);free(lat);free(layer);fprintf(stderr,"ERROR427: Cannot write '%s': %s.\n",filename,nc_strerror(rc)); nc_close(cdf->ncid); return TRUE;}

Bool create1_pft_netcdf(Netcdf *cdf,
                        const char *filename, /**< filename of NetCDF file */
                        int index,
                        int npft,             /**< number of natural PFTs */
                        int ncft,             /**< number of crop PFTs */
                        const char *name,     /**< name of output variable */
                        const char *descr,    /**< description of output variable */
                        const char *units,    /**< unit of output variable */
                        Type type,            /**< type of output variable */
                        int n,                /**< number of samples per year (1/12/365) */
                        int year,            /**< simulation year (AD) */
                        const Coord_array *array, /**< coordinate array */
                        const Config *config  /**< LPJ configuration */
                       )                      /** \return TRUE on error */
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  String s;
  time_t t;
  int i,rc,imiss=MISSING_VALUE_INT,size,*days;
  short smiss=MISSING_VALUE_SHORT;
  float *lon,*lat,miss=config->missing_value,*layer;
  int dim[4];
  int lon_dim_id,lat_dim_id,lon_var_id,lat_var_id,pft_dim_id,pft_var_id;
  int time_dim_id,time_var_id;
  char **pftnames;
#ifndef USE_NETCDF4
  int dimids[2],pft_len_id;
  size_t offset[2],count[2],pft_len;
#endif
  if(array==NULL || name==NULL || filename==NULL)
  {
    fputs("ERROR424: Invalid array pointer in create1_pft_netcdf().\n",stderr);
    return TRUE;
  }
  cdf->missing_value=config->missing_value;
  lon=newvec(float,array->nlon);
  if(lon==NULL)
  {
    printallocerr("lon");
    return TRUE;
  }
  lat=newvec(float,array->nlat);
  if(lat==NULL)
  {
    free(lon);
    printallocerr("lat");
    return TRUE;
  }
  size=outputsize(index,npft,ncft,config);
  if(issoil(index))
  {
    layer=newvec(float,size);
    if(layer==NULL)
    {
      free(lon);
      free(lat);
      printallocerr("layer");
      return TRUE;
    }
    layer[0]=0;
    for(i=1;i<size;i++)
     layer[i]=(float)layerbound[i-1];
  }
  else layer=NULL;
  for(i=0;i<array->nlon;i++)
    lon[i]=(float)(array->lon_min+i*config->resolution.lon);
  for(i=0;i<array->nlat;i++)
    lat[i]=(float)(array->lat_min+i*config->resolution.lat);
  if(n>1)
  {
    days=newvec(int,n);
    if(days==NULL)
    {
      free(lon);
      free(lat);
      printallocerr("time");
      return TRUE;
    }
    switch(n)
    {
      case 12:
        for(i=0;i<12;i++)
          if(i==0)
            days[0]=ndaymonth[i]-1;
          else
            days[i]=days[i-1]+ndaymonth[i];
        break;
    case NDAYYEAR:
      for(i=0;i<n;i++)
        days[i]=i;
      break;
    default:
      fprintf(stderr,"ERROR425: Invalid value=%d for number of data points per year.\n",n);
      free(days);
      free(lon);
      free(lat);
      return TRUE;
    }
  }
  else
    days=NULL;
  cdf->index=array;
#ifdef USE_NETCDF4
  rc=nc_create(filename,NC_CLOBBER|NC_NETCDF4,&cdf->ncid);
#else
  rc=nc_create(filename,NC_CLOBBER,&cdf->ncid);
#endif
  if(rc)
  {
    fprintf(stderr,"ERROR426: Cannot create file '%s': %s.\n",
            filename,nc_strerror(rc));
    free(lon);
    free(lat);
    free(layer);
    return TRUE;
  }
  error(rc);
  rc=nc_def_dim(cdf->ncid,issoil(index) ? config->layer_index : config->pft_index,size,&pft_dim_id);
  error(rc);
  rc=nc_def_dim(cdf->ncid,LAT_DIM_NAME,array->nlat,&lat_dim_id);
  error(rc);
  rc=nc_def_dim(cdf->ncid,LON_DIM_NAME,array->nlon,&lon_dim_id);
  error(rc);
  if(n>1)
  {
    rc=nc_def_dim(cdf->ncid,TIME_DIM_NAME,n,&time_dim_id);
    error(rc);
    dim[0]=time_dim_id;
    dim[1]=pft_dim_id;
    dim[2]=lat_dim_id;
    dim[3]=lon_dim_id;
  }
  else
  {
    dim[0]=pft_dim_id;
    dim[1]=lat_dim_id;
    dim[2]=lon_dim_id;
  }
  if(issoil(index))
  {
    rc=nc_def_var(cdf->ncid,"layer",NC_FLOAT,1,&pft_dim_id,&pft_var_id);
    error(rc);
    rc=nc_put_att_text(cdf->ncid,pft_var_id,"units",strlen("mm"),"mm");
    error(rc);
    rc=nc_put_att_text(cdf->ncid,pft_var_id,"long_name",strlen("soil depth"),"soil depth");
  }
  else
  {
    pftnames=createpftnames(index,npft,ncft,config);
    if(pftnames==NULL)
    {
      free(lon);
      free(lat);
      free(layer);
      printallocerr("pftnames");
      nc_close(cdf->ncid);
      return TRUE;
    }
#ifdef USE_NETCDF4
    rc=nc_def_var(cdf->ncid,"NamePFT",NC_STRING,1,&pft_dim_id,&pft_var_id);
#else
    pft_len=0;
    for(i=0;i<size;i++)
      if(pft_len<strlen(pftnames[i]))
        pft_len=strlen(pftnames[i]);
    pft_len++;
    rc=nc_def_dim(cdf->ncid,"len",pft_len,&pft_len_id);
    error(rc);
    dimids[0]=pft_dim_id;
    dimids[1]=pft_len_id;
    rc=nc_def_var(cdf->ncid,"NamePFT",NC_CHAR,2,dimids,&pft_var_id);
#endif
  }
  error(rc);
  rc=nc_def_var(cdf->ncid,LAT_NAME,NC_FLOAT,1,&lat_dim_id,&lat_var_id);
  error(rc);
  rc=nc_put_att_text(cdf->ncid,lat_var_id,"units",strlen("degrees_north"),
                     "degrees_north");
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"long_name",strlen("latitude"),"latitude");
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"standard_name",strlen("latitude"),"latitude");
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"axis",strlen("Y"),"Y");
  error(rc);
  rc=nc_def_var(cdf->ncid,LON_NAME,NC_FLOAT,1,&lon_dim_id,&lon_var_id);
  error(rc);
  rc=nc_put_att_text(cdf->ncid,lon_var_id,"units",strlen("degrees_east"),
                     "degrees_east");
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"long_name",strlen("longitude"),"longitude");
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"standard_name",strlen("longitude"),"longitude");
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"axis",strlen("X"),"X");
  error(rc);
  rc=nc_def_var(cdf->ncid,name,nctype[type],(n>1) ? 4 : 3,dim,&cdf->varid);
  error(rc);
  if(n>1)
  {
    rc=nc_def_var(cdf->ncid,TIME_NAME,NC_INT,1,&time_dim_id,&time_var_id);
    error(rc);
    snprintf(s,STRING_LEN,"days since %d-1-1 0:0:0",year);
    rc=nc_put_att_text(cdf->ncid,time_var_id,"units",strlen(s),s);
    error(rc);
    rc=nc_put_att_text(cdf->ncid,time_var_id,"calendar",strlen("noleap"),
                       "noleap");
    error(rc);
  }
#ifdef USE_NETCDF4
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
  if(descr!=NULL)
  {
    rc=nc_put_att_text(cdf->ncid, cdf->varid,"long_name",strlen(descr),descr);
    error(rc);
  }
  switch(type)
  { 
    case LPJ_FLOAT:
      nc_put_att_float(cdf->ncid, cdf->varid, "missing_value", NC_FLOAT,1,&miss);
      rc=nc_put_att_float(cdf->ncid, cdf->varid, "_FillValue", NC_FLOAT,1,&miss);
      break;
    case LPJ_SHORT:
      nc_put_att_short(cdf->ncid, cdf->varid, "missing_value", NC_SHORT,1,&smiss);
      rc=nc_put_att_short(cdf->ncid, cdf->varid, "_FillValue", NC_SHORT,1,&smiss);
      break;
    case LPJ_INT:
      nc_put_att_int(cdf->ncid, cdf->varid, "missing_value", NC_INT,1,&imiss);
      rc=nc_put_att_int(cdf->ncid, cdf->varid, "_FillValue", NC_INT,1,&imiss);
      break;
  }
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
  rc=nc_enddef(cdf->ncid);
  error(rc);
  if(n>1)
  {
    rc=nc_put_var_int(cdf->ncid,time_var_id,days);
    error(rc);
  }
  if(issoil(index))
  {
    rc=nc_put_var_float(cdf->ncid,pft_var_id,layer);
    error(rc);
  }
  else
  {
#ifdef USE_NETCDF4
    rc=nc_put_var_string(cdf->ncid,pft_var_id,(const char **)pftnames);
    error(rc);
#else
    count[0]=1;
    offset[1]=0;
    for(i=0;i<size;i++)
    {
      offset[0]=i;
      count[1]=strlen(pftnames[i])+1;
      rc=nc_put_vara_text(cdf->ncid,pft_var_id,offset,count,pftnames[i]);
      error(rc);
    }
#endif
    freepftnames(pftnames,index,npft,ncft,config);
  }
  rc=nc_put_var_float(cdf->ncid,lat_var_id,lat);
  error(rc);
  rc=nc_put_var_float(cdf->ncid,lon_var_id,lon);
  error(rc);
  free(layer);
  free(lat);
  free(lon);
  free(days);
  return FALSE;
#else
  fputs("ERROR401: NetCDF output is not supported by this version of LPJmL.\n",stderr);
  return TRUE;
#endif
} /* of 'create1_pft_netcdf' */

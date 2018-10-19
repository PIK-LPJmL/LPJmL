/**************************************************************************************/
/**                                                                                \n**/
/**      c  r  e  a  t  e  _  p  f  t  _  n  e  t  c  d  f  .  c                   \n**/
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

#define error(rc) if(rc) {free(lon);free(lat);free(year);free(layer);fprintf(stderr,"ERROR427: Cannot write '%s': %s.\n",filename,nc_strerror(rc)); nc_close(cdf->ncid); return TRUE;}

Bool create_pft_netcdf(Netcdf *cdf,
                       const char *filename, /**< filename of NetCDF file */
                       int index,            /**< output index */
                       int npft,             /**< number of natural PFTs */
                       int ncft,             /**< number of crop PFTs */
                       const char *name,     /**< name of output variable */
                       const char *descr,    /**< description of output variable */
                       const char *units,    /**< unit of output variable */
                       Type type,            /**< type of output variable */
                       int n,                /**< number of samples per year (1/12/365) */
                       const Coord_array *array, /**< coordinate array */
                       const Config *config  /**< LPJ configuration */
                      )                      /** \return TRUE on error */
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  String s;
  time_t t;
  int i,j,rc,nyear,imiss=MISSING_VALUE_INT,size;
  short smiss=MISSING_VALUE_SHORT;
  float *lon,*lat,miss=config->missing_value,*layer;
  int *year,dim[4];
  char **pftnames;
#ifndef USE_NETCDF4
  int dimids[2],pft_len_id;
  size_t offset[2],count[2],pft_len;
#endif
  int time_dim_id,lon_dim_id,lat_dim_id,time_var_id,lon_var_id,lat_var_id,pft_dim_id,pft_var_id;
  if(array==NULL || name==NULL || filename==NULL)
  {
    fputs("ERROR424: Invalid array pointer in create_pft_netcdf().\n",stderr);
    return TRUE;
  }
  cdf->missing_value=config->missing_value;
  cdf->index=array;
  if(config->ischeckpoint)
  {
    if(cdf->state==ONEFILE || cdf->state==CREATE)
    {
      /* start from checkpoint file, output files exist and have to be opened */
#ifdef USE_NETCDF4
      rc=nc_open(filename,NC_WRITE|(config->compress) ? NC_CLOBBER|NC_NETCDF4 : NC_CLOBBER,&cdf->ncid);
#else
      rc=nc_open(filename,NC_WRITE|NC_CLOBBER,&cdf->ncid);
#endif
      if(rc)
      {
        fprintf(stderr,"ERROR426: Cannot open file '%s': %s.\n",
                filename,nc_strerror(rc));
        return TRUE;
      }
    }
    rc=nc_inq_varid(cdf->ncid,name,&cdf->varid);
    if(rc)
    {
      fprintf(stderr,"ERROR426: Cannot get variable '%s': %s.\n",
              name,nc_strerror(rc));
      return TRUE;
    }
    return FALSE;
  }
  nyear=config->lastyear-config->outputyear+1;
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
  year=newvec(int,nyear*n);
  if(year==NULL)
  {
    free(lon);
    free(lat);
    printallocerr("year");
    return TRUE;
  }
  size=outputsize(index,npft,config->nbiomass,ncft);
  if(index==SOILC_LAYER || index==MSOILTEMP || index==MSWC)
  {
    layer=newvec(float,size);
    if(layer==NULL)
    {
      free(lon);
      free(lat);
      free(year);
      printallocerr("layer");
      return TRUE;
    }
    layer[0]=0;
    for(i=1;i<size;i++)
     layer[i]=(float)layerbound[i-1];
  }
  else layer=NULL;
  for(i=0;i<array->nlon;i++)
    lon[i]=array->lon_min+i*config->resolution.lon;
  for(i=0;i<array->nlat;i++)
    lat[i]=array->lat_min+i*config->resolution.lat;
  switch(n)
  {
    case 1:
      for(i=0;i<nyear;i++)
        year[i]=config->outputyear+i;
      break;
    case 12:
      for(i=0;i<nyear;i++)
        for(j=0;j<12;j++)
          if(i==0 && j==0)
            year[0]=ndaymonth[j]-1;
          else
            year[i*12+j]=year[i*12+j-1]+ndaymonth[j];
      break;
    case NDAYYEAR:
      for(i=0;i<nyear*n;i++)
        year[i]=i;
      break;
    default:
      fputs("ERROR425: Invalid value for number of data points per year.\n",
            stderr);
      free(year);
      free(lon);
      free(lat);
      return TRUE;
  }
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
    free(year);
    free(layer);
    return TRUE;
  }
  error(rc);
  rc=nc_def_dim(cdf->ncid,TIME_DIM_NAME,nyear*n,&time_dim_id);
  error(rc);
  rc=nc_def_dim(cdf->ncid,(index==SOILC_LAYER || index==MSOILTEMP || index==MSWC) ? "layer" : "npft",size,&pft_dim_id);
  error(rc);
  rc=nc_def_dim(cdf->ncid,LAT_DIM_NAME,array->nlat,&lat_dim_id);
  error(rc);
  rc=nc_def_dim(cdf->ncid,LON_DIM_NAME,array->nlon,&lon_dim_id);
  error(rc);
  dim[0]=time_dim_id;
  dim[1]=pft_dim_id;
  dim[2]=lat_dim_id;
  dim[3]=lon_dim_id;
  if(index==SOILC_LAYER || index==MSOILTEMP || index==MSWC)
  {
    rc=nc_def_var(cdf->ncid,"layer",NC_FLOAT,1,&pft_dim_id,&pft_var_id);
    error(rc);
    rc=nc_put_att_text(cdf->ncid,pft_var_id,"units",strlen("mm"),"mm");
    error(rc);
    rc=nc_put_att_text(cdf->ncid,pft_var_id,"long_name",strlen("soil depth"),"soil depth");
  }
  else
  {
    pftnames=createpftnames(index,npft,config->nbiomass,ncft,config->pftpar);
    if(pftnames==NULL)
    {
      free(lat);
      free(lon);
      free(year);
      printallocerr("pftnames");
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
  rc=nc_def_var(cdf->ncid,TIME_NAME,NC_INT,1,&time_dim_id,&time_var_id);
  error(rc);
  if(n==1)
    rc=nc_put_att_text(cdf->ncid,time_var_id,"units",strlen("year"),"year");
  else
  {
    snprintf(s,STRING_LEN,"days since %d-1-1 0:0:0",config->outputyear);
    rc=nc_put_att_text(cdf->ncid,time_var_id,"units",strlen(s),s);
    error(rc);
    rc=nc_put_att_text(cdf->ncid,time_var_id,"calendar",strlen("noleap"),
                       "noleap");
  }
  error(rc);
  /*rc=nc_def_var(cdf->ncid,"PFT",NC_INT,1,&pft_dim_id,&pft_var_id);
  error(rc); */
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
  rc=nc_def_var(cdf->ncid,name,nctype[type],4,dim,&cdf->varid);
  error(rc);
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
  rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"source",strlen("LPJmL C Version " LPJ_VERSION),
                     "LPJmL C Version " LPJ_VERSION);
  error(rc);
  time(&t);
  snprintf(s,STRING_LEN,"Created for user %s on %s at %s: %s",
           getuser(),gethost(),strdate(&t),config->arglist);
  rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"history",strlen(s),s);
  error(rc);
  rc=nc_enddef(cdf->ncid);
  error(rc);
  rc=nc_put_var_int(cdf->ncid,time_var_id,year);
  error(rc);
  if(index==SOILC_LAYER || index==MSOILTEMP || index==MSWC)
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
    freepftnames(pftnames,index,npft,config->nbiomass,ncft);
  }
  rc=nc_put_var_float(cdf->ncid,lat_var_id,lat);
  error(rc);
  rc=nc_put_var_float(cdf->ncid,lon_var_id,lon);
  error(rc);
  free(layer);
  free(lat);
  free(lon);
  free(year);
  return FALSE;
#else
  fputs("ERROR401: NetCDF output is not supported by this version of LPJmL.\n",stderr);
  return TRUE;
#endif
} /* of 'create_pft_netcdf' */

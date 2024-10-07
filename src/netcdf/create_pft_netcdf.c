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
#ifdef USE_NETCDF
#include <netcdf.h>
#include <time.h>

static nc_type nctype[]={NC_BYTE,NC_SHORT,NC_INT,NC_FLOAT,NC_DOUBLE};
#endif

#define error(rc) if(rc) {free(lon);free(lon_bnds);free(lat);free(lat_bnds);free(year);free(layer);free(bnds);free(time_bnds);free(pftnames); fprintf(stderr,"ERROR427: Cannot write '%s': %s.\n",filename,nc_strerror(rc)); nc_close(cdf->ncid); return TRUE;}

Bool create_pft_netcdf(Netcdf *cdf,
                       const char *filename, /**< filename of NetCDF file */
                       int index,            /**< output index */
                       int npft,             /**< number of natural PFTs */
                       int ncft,             /**< number of crop PFTs */
                       const char *name,     /**< name of output variable */
                       const char *standard_name, /**< standard name of output variable */
                       const char *long_name,/**< long name of output variable */
                       const char *units,    /**< unit of output variable */
                       Type type,            /**< type of output variable */
                       int n,                /**< number of samples per year (1/12/365) */
                       int timestep,         /**< time step for annual output (yrs) */
                       int actualyear,       /**< actual year for oneyear output */
                       Bool oneyear,         /**< one file for each year (TRUE/FALSE) */
                       const Coord_array *array, /**< coordinate array */
                       const Config *config  /**< LPJ configuration */
                      )                      /** \return TRUE on error */
{
#ifdef USE_NETCDF
  char *s;
  time_t t;
  int i,rc,nyear,size,len,*pft;
  double *lon=NULL,*lat=NULL,*lat_bnds,*lon_bnds;
  double *layer,*bnds,*time_bnds=NULL;
  double *year=NULL;
  int dim[4],bnds_var_id,bnds_dim_id,time_bnds_var_id,lon_bnds_var_id,lat_bnds_var_id;
  char **pftnames=NULL;
  size_t chunk[4];
  int dimids[2];
  int pft_len_id;
  size_t offset[2],count[2],pft_len;
  int time_dim_id,lon_dim_id,lat_dim_id,time_var_id,lon_var_id,lat_var_id,pft_dim_id,pft_var_id,pft_var_id2;
  if(array==NULL || name==NULL || filename==NULL)
  {
    fputs("ERROR424: Invalid array pointer in create_pft_netcdf().\n",stderr);
    return TRUE;
  }
  cdf->missing_value=config->netcdf.missing_value;
  cdf->index=array;
  if(oneyear)
    nyear=1;
  else
  {
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
        if(config->nofill)
          ncsetfill(cdf->ncid,NC_NOFILL);
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
  }
  if(setlatlon(&lat,&lon,&lat_bnds,&lon_bnds,array))
      return TRUE;
  if(n==1 && oneyear)
    year=NULL;
  else
  {
    if(settimeaxis(&year,&time_bnds,nyear,n,timestep,config->outputyear,config->baseyear,oneyear,config->with_days,config->absyear,filename))
    {
      free(lat);
      free(lat_bnds);
      free(lon);
      free(lon_bnds);
      return TRUE;
    }
  }
  size=outputsize(index,npft,ncft,config);
  if(issoil(index))
  {
    layer=newvec(double,size);
    if(layer==NULL)
    {
      free(lon);
      free(lon_bnds);
      free(lat);
      free(lat_bnds);
      free(time_bnds);
      free(year);
      printallocerr("layer");
      return TRUE;
    }
    bnds=newvec(double,2*size);
    if(bnds==NULL)
    {
      free(lon);
      free(lon_bnds);
      free(lat);
      free(lat_bnds);
      free(year);
      free(time_bnds);
      free(layer);
      printallocerr("bnds");
      return TRUE;
    }
    bnds[0]=0;
    bnds[1]=layerbound[0]*config->netcdf.depth_bnds.scale;
    layer[0]=midlayer[0]*config->netcdf.depth.scale;
    for(i=1;i<size;i++)
    {
      bnds[2*i]=layerbound[i-1]*config->netcdf.depth_bnds.scale;
      bnds[2*i+1]=layerbound[i]*config->netcdf.depth_bnds.scale;
      layer[i]=midlayer[i]*config->netcdf.depth.scale;
    }
  }
  else bnds=layer=NULL;
  rc=nc_create(filename,(config->isnetcdf4) ? NC_CLOBBER|NC_NETCDF4 : NC_CLOBBER,&cdf->ncid);
  if(rc)
  {
    fprintf(stderr,"ERROR426: Cannot create file '%s': %s.\n",
            filename,nc_strerror(rc));
    free(lon);
    free(lon_bnds);
    free(lat);
    free(lat_bnds);
    free(year);
    free(layer);
    free(time_bnds);
    free(bnds);
    return TRUE;
  }
  if(config->nofill)
  {
    ncsetfill(cdf->ncid,NC_NOFILL);
  }
  if(year!=NULL)
  {
    if(n==1)
      rc=nc_def_dim(cdf->ncid,config->netcdf.time.dim,nyear/timestep,&time_dim_id);
    else
      rc=nc_def_dim(cdf->ncid,config->netcdf.time.dim,nyear*n,&time_dim_id);
    error(rc);
  }
  rc=nc_def_dim(cdf->ncid,issoil(index) ? config->netcdf.depth.dim : config->netcdf.pft.dim,size,&pft_dim_id);
  error(rc);
  rc=nc_def_dim(cdf->ncid,config->netcdf.lat.dim,array->nlat,&lat_dim_id);
  error(rc);
  rc=nc_def_dim(cdf->ncid,config->netcdf.lon.dim,array->nlon,&lon_dim_id);
  error(rc);
  if(year==NULL)
  {
    dim[0]=pft_dim_id;
    dim[1]=lat_dim_id;
    dim[2]=lon_dim_id;
    chunk[0]=1;
    chunk[1]=array->nlat;
    chunk[2]=array->nlon;
  }
  else
  {
    dim[0]=time_dim_id;
    dim[1]=pft_dim_id;
    dim[2]=lat_dim_id;
    dim[3]=lon_dim_id;
    chunk[0]=chunk[1]=1;
    chunk[2]=array->nlat;
    chunk[3]=array->nlon;
  }
  rc=nc_def_dim(cdf->ncid,config->netcdf.bnds_name,2,&bnds_dim_id);
  error(rc);
  if(issoil(index))
  {
    rc=nc_def_var(cdf->ncid,config->netcdf.depth.name,NC_DOUBLE,1,&pft_dim_id,&pft_var_id);
    error(rc);
    rc=nc_put_att_text(cdf->ncid,pft_var_id,"units",
                       strlen(config->netcdf.depth.unit),
                       config->netcdf.depth.unit);
    error(rc);
    rc=nc_put_att_text(cdf->ncid,pft_var_id,"standard_name",
                       strlen(config->netcdf.depth.standard_name),
                       config->netcdf.depth.standard_name);
    error(rc);
    rc=nc_put_att_text(cdf->ncid,pft_var_id,"long_name",
                       strlen(config->netcdf.depth.long_name),
                       config->netcdf.depth.long_name);
    error(rc);
    rc=nc_put_att_text(cdf->ncid,pft_var_id,"bounds",
                       strlen(config->netcdf.depth_bnds.name),
                       config->netcdf.depth_bnds.name);
    error(rc);
    rc=nc_put_att_text(cdf->ncid,pft_var_id,"positive",strlen("down"),"down");
    error(rc);
    rc=nc_put_att_text(cdf->ncid,pft_var_id,"axis",strlen("Z"),"Z");
    error(rc);
    dimids[0]=pft_dim_id;
    dimids[1]=bnds_dim_id;
    rc=nc_def_var(cdf->ncid,config->netcdf.depth_bnds.name,NC_DOUBLE,2,dimids,&bnds_var_id);
    error(rc);
    rc=nc_put_att_text(cdf->ncid,bnds_var_id,"units",
                       strlen(config->netcdf.depth_bnds.unit),
                       config->netcdf.depth_bnds.unit);
    error(rc);
    rc=nc_put_att_text(cdf->ncid,bnds_var_id,"comment",
                       strlen(config->netcdf.depth_bnds.long_name),
                       config->netcdf.depth_bnds.long_name);
  }
  else
  {
    rc=nc_def_var(cdf->ncid,config->netcdf.pft.name,NC_INT,1,&pft_dim_id,&pft_var_id2);
    rc=nc_put_att_text(cdf->ncid,pft_var_id2,"long_name",strlen(config->netcdf.pft.long_name),config->netcdf.pft.long_name);
    pftnames=createpftnames(index,npft,ncft,config);
    if(pftnames==NULL)
    {
      free(lat);
      free(lat_bnds);
      free(lon);
      free(lon_bnds);
      free(year);
      free(time_bnds);
      printallocerr("pftnames");
      return TRUE;
    }
    if(config->isnetcdf4)
      rc=nc_def_var(cdf->ncid,config->netcdf.pft_name.name,NC_STRING,1,&pft_dim_id,&pft_var_id);
    else
    {
      pft_len=0;
      for(i=0;i<size;i++)
        if(pft_len<strlen(pftnames[i]))
          pft_len=strlen(pftnames[i]);
      pft_len++;
      rc=nc_def_dim(cdf->ncid,config->netcdf.pft_name.dim,pft_len,&pft_len_id);
      error(rc);
      dimids[0]=pft_dim_id;
      dimids[1]=pft_len_id;
      rc=nc_def_var(cdf->ncid,config->netcdf.pft_name.name,NC_CHAR,2,dimids,&pft_var_id);
    }
    error(rc);
    rc=nc_put_att_text(cdf->ncid,pft_var_id,"long_name",strlen(config->netcdf.pft_name.long_name),config->netcdf.pft_name.long_name);
  }
  error(rc);
  if(year!=NULL)
  {
    rc=nc_def_var(cdf->ncid,config->netcdf.time.name,NC_DOUBLE,1,&time_dim_id,&time_var_id);
    error(rc);
    dimids[0]=time_dim_id;
    dimids[1]=bnds_dim_id;
    rc=nc_def_var(cdf->ncid,config->netcdf.time_bnds.name,NC_DOUBLE,2,dimids,&time_bnds_var_id);
    error(rc);
    if(n==1)
    {
      if(config->absyear)
        s=strdup(config->netcdf.years_name);
      else
      {
        len=snprintf(NULL,0,"%s since %d-1-1 0:0:0",(config->with_days) ? "days" : "years",config->baseyear);
        s=malloc(len+1);
        sprintf(s,"%s since %d-1-1 0:0:0",(config->with_days) ? "days" : "years",config->baseyear);
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
    rc=nc_put_att_text(cdf->ncid,time_var_id,"units",strlen(s),s);
    rc=nc_put_att_text(cdf->ncid,time_bnds_var_id,"units",strlen(s),s);
    free(s);
    error(rc);
    rc=nc_put_att_text(cdf->ncid,time_var_id,"calendar",strlen(config->netcdf.calendar),
                       config->netcdf.calendar);
    error(rc);
    rc=nc_put_att_text(cdf->ncid,time_bnds_var_id,"calendar",strlen(config->netcdf.calendar),
                       config->netcdf.calendar);
    error(rc);
    rc=nc_put_att_text(cdf->ncid, time_var_id,"standard_name",
                       strlen(config->netcdf.time.standard_name),
                       config->netcdf.time.standard_name);
    error(rc);
    rc=nc_put_att_text(cdf->ncid, time_var_id,"long_name",
                       strlen(config->netcdf.time.long_name),
                       config->netcdf.time.long_name);
    error(rc);
    if(strlen(config->netcdf.time_bnds.long_name))
    {
      rc=nc_put_att_text(cdf->ncid, time_bnds_var_id,"long_name",strlen(config->netcdf.time_bnds.long_name),config->netcdf.time_bnds.long_name);
      error(rc);
    }
    rc=nc_put_att_text(cdf->ncid, time_var_id,"axis",strlen("T"),"T");
    error(rc);
  }
  /*rc=nc_def_var(cdf->ncid,"PFT",NC_INT,1,&pft_dim_id,&pft_var_id);
  error(rc); */
  rc=nc_def_var(cdf->ncid,config->netcdf.lat.name,NC_DOUBLE,1,&lat_dim_id,&lat_var_id);
  error(rc)
  dimids[0]=lat_dim_id;
  dimids[1]=bnds_dim_id;
  rc=nc_def_var(cdf->ncid,config->netcdf.lat_bnds.name,NC_DOUBLE,2,dimids,&lat_bnds_var_id);
  error(rc);
  rc=nc_put_att_text(cdf->ncid,lat_var_id,"units",
                       strlen(config->netcdf.lat.unit),
                       config->netcdf.lat.unit);
  error(rc);
  if(strlen(config->netcdf.lat_bnds.unit))
  {
    rc=nc_put_att_text(cdf->ncid,lat_bnds_var_id,"units",
                         strlen(config->netcdf.lat_bnds.unit),
                         config->netcdf.lat_bnds.unit);
    error(rc);
  }
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"long_name",
                       strlen(config->netcdf.lat.long_name),
                       config->netcdf.lat.long_name);
  error(rc);
  if(strlen(config->netcdf.lat_bnds.long_name))
  {
    rc=nc_put_att_text(cdf->ncid, lat_bnds_var_id,"long_name",
                         strlen(config->netcdf.lat_bnds.long_name),
                         config->netcdf.lat_bnds.long_name);
    error(rc);
  }
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"standard_name",
                       strlen(config->netcdf.lat.standard_name),
                       config->netcdf.lat.standard_name);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"bounds",strlen(config->netcdf.lat_bnds.name),config->netcdf.lat_bnds.name);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"axis",strlen("Y"),"Y");
  error(rc);
  rc=nc_def_var(cdf->ncid,config->netcdf.lon.name,NC_DOUBLE,1,&lon_dim_id,&lon_var_id);
  error(rc);
  dimids[0]=lon_dim_id;
  dimids[1]=bnds_dim_id;
  rc=nc_def_var(cdf->ncid,config->netcdf.lon_bnds.name,NC_DOUBLE,2,dimids,&lon_bnds_var_id);
  error(rc);
  rc=nc_put_att_text(cdf->ncid,lon_var_id,"units",
                     strlen(config->netcdf.lon.unit),
                     config->netcdf.lon.unit);
  error(rc);
  if(strlen(config->netcdf.lon_bnds.unit))
  {
    rc=nc_put_att_text(cdf->ncid,lon_bnds_var_id,"units",
                       strlen(config->netcdf.lon.unit),
                       config->netcdf.lon.unit);
    error(rc);
  }
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"long_name",
                     strlen(config->netcdf.lon.long_name),
                     config->netcdf.lon.long_name);
  error(rc);
  if(strlen(config->netcdf.lon_bnds.long_name))
  {
    rc=nc_put_att_text(cdf->ncid,lon_bnds_var_id,"long_name",
                       strlen(config->netcdf.lon_bnds.long_name),
                       config->netcdf.lon_bnds.long_name);
    error(rc);
  }
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"standard_name",
                     strlen(config->netcdf.lon.standard_name),
                     config->netcdf.lon.standard_name);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"bounds",strlen(config->netcdf.lon_bnds.name),config->netcdf.lon_bnds.name);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"axis",strlen("X"),"X");
  error(rc);
  rc=nc_def_var(cdf->ncid,name,nctype[type],(year==NULL) ? 3 : 4,dim,&cdf->varid);
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
  if(units!=NULL && strlen(units))
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
      nc_put_att_float(cdf->ncid, cdf->varid, "missing_value", NC_FLOAT,1,&config->netcdf.missing_value.f);
      rc=nc_put_att_float(cdf->ncid, cdf->varid, "_FillValue", NC_FLOAT,1,&config->netcdf.missing_value.f);
      break;
    case LPJ_SHORT:
      nc_put_att_short(cdf->ncid, cdf->varid, "missing_value", NC_SHORT,1,&config->netcdf.missing_value.s);
      rc=nc_put_att_short(cdf->ncid, cdf->varid, "_FillValue", NC_SHORT,1,&config->netcdf.missing_value.s);
      break;
    case LPJ_INT:
      nc_put_att_int(cdf->ncid, cdf->varid, "missing_value", NC_INT,1,&config->netcdf.missing_value.i);
      rc=nc_put_att_int(cdf->ncid, cdf->varid, "_FillValue", NC_INT,1,&config->netcdf.missing_value.i);
      break;
    default:
      fputs("ERROR428: Invalid data type in NetCDF file.\n",stderr);
      free(lat);
      free(lat_bnds);
      free(lon);
      free(lon_bnds);
      free(year);
      free(time_bnds);
      free(layer);
      free(bnds);
      freepftnames(pftnames,index,npft,ncft,config);
      return TRUE;
  }
  error(rc);
  rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"title",
                     strlen(config->sim_name),config->sim_name);
  error(rc);
  rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"source",strlen("LPJmL C Version " LPJ_VERSION),
                     "LPJmL C Version " LPJ_VERSION);
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
  rc=nc_enddef(cdf->ncid);
  error(rc);
  if(year!=NULL)
  {
    rc=nc_put_var_double(cdf->ncid,time_var_id,year);
    error(rc);
    rc=nc_put_var_double(cdf->ncid,time_bnds_var_id,time_bnds);
    error(rc);
  }
  if(issoil(index))
  {
    rc=nc_put_var_double(cdf->ncid,pft_var_id,layer);
    error(rc);
    rc=nc_put_var_double(cdf->ncid,bnds_var_id,bnds);
    error(rc);
  }
  else
  {
    pft=newvec(int,size);
    if(pft==NULL)
    {
      printallocerr("pft");
      return TRUE;
    }
    for(i=0;i<size;i++)
      pft[i]=i+1;
    rc=nc_put_var_int(cdf->ncid,pft_var_id2,pft);
    error(rc);
    free(pft);
    if(config->isnetcdf4)
    {
      rc=nc_put_var_string(cdf->ncid,pft_var_id,(const char **)pftnames);
      error(rc);
    }
    else
    {
      count[0]=1;
      offset[1]=0;
      for(i=0;i<size;i++)
      {
        offset[0]=i;
        count[1]=strlen(pftnames[i])+1;
        rc=nc_put_vara_text(cdf->ncid,pft_var_id,offset,count,pftnames[i]);
        error(rc);
      }
    }
    freepftnames(pftnames,index,npft,ncft,config);
    pftnames=NULL;
  }
  rc=nc_put_var_double(cdf->ncid,lat_var_id,lat);
  error(rc);
  rc=nc_put_var_double(cdf->ncid,lat_bnds_var_id,lat_bnds);
  error(rc);
  rc=nc_put_var_double(cdf->ncid,lon_var_id,lon);
  error(rc);
  rc=nc_put_var_double(cdf->ncid,lon_bnds_var_id,lon_bnds);
  error(rc);
  free(layer);
  free(bnds);
  free(time_bnds);
  free(lat);
  free(lat_bnds);
  free(lon);
  free(lon_bnds);
  free(year);
  return FALSE;
#else
  fputs("ERROR401: NetCDF output is not supported by this version of LPJmL.\n",stderr);
  return TRUE;
#endif
} /* of 'create_pft_netcdf' */

/**************************************************************************************/
/**                                                                                \n**/
/**                b  i  n  2  c  d  f  .  c                                       \n**/
/**                                                                                \n**/
/**     Converts binary output file into NetCDF data file                          \n**/
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

#define error(rc) if(rc) {free(lon);free(lat);free(year);fprintf(stderr,"ERROR427: Cannot write '%s': %s.\n",filename,nc_strerror(rc)); nc_close(cdf->ncid); free(cdf);return NULL;}

#define USAGE "Usage: %s [-h] [-v] [-clm] [-floatgrid] [-doublegrid] [-revlat] [-days] [-absyear] [-firstyear y] [-baseyear y] [-nbands n] [-nstep n] [-cellsize size] [-swap]\n       [[-attr name=value]..] [-global] [-short] [-compress level] [-units u] [-descr d] [-missing_value val] [-metafile] [-map name] [varname gridfile]\n       binfile netcdffile\n"

typedef struct
{
  const Coord_array *index;
  int ncid;
  int varid;
} Cdf;

static Cdf *create_cdf(const char *filename,
                       Map *map,
                       const char *map_name,
                       const char *cmdline,
                       const char *source,
                       const char *history,
                       const char *name,
                       const char *units,
                       const char *standard_name,
                       const char *long_name,
                       float miss,
                       short miss_short,
                       const Attr *global_attrs,
                       int n_global,
                       Type type,
                       Header header,
                       int baseyear,
                       Bool ispft,
                       int compress,
                       const Coord_array *array,
                       Bool revlat,
                       Bool with_days,
                       Bool absyear)
{
  Cdf *cdf;
  double *lon,*lat;
  double *layer=NULL,*bnds=NULL,*midlayer=NULL,*time_bnds=NULL;
  double *year;
  int i,j,rc,dim[4],dim2[2],dimids[2];
  size_t chunk[4],offset[2],count[2];
  char *s;
  time_t t;
  int time_var_id,lat_var_id,lon_var_id,time_dim_id,lat_dim_id,lon_dim_id,map_dim_id,len_dim_id,bnds_var_id,bnds_dim_id;
  int time_bnds_var_id,time_bnds_dim_id;
  int pft_dim_id,varid;
  int len;
  cdf=new(Cdf);
  lon=newvec(double,array->nlon);
  if(lon==NULL)
  {
    printallocerr("lon");
    free(cdf);
    return NULL;
  }
  lat=newvec(double,array->nlat);
  if(lat==NULL)
  {
    printallocerr("lat");
    free(lon);
    free(cdf);
    return NULL;
  }
  for(i=0;i<array->nlon;i++)
    lon[i]=array->lon_min+i*header.cellsize_lon;
  if(revlat)
    for(i=0;i<array->nlat;i++)
      lat[i]=array->lat_min+(array->nlat-1-i)*header.cellsize_lat;
  else
    for(i=0;i<array->nlat;i++)
      lat[i]=array->lat_min+i*header.cellsize_lat;
  year=newvec(double,header.nyear*header.nstep);
  if(year==NULL)
  {
    printallocerr("year");
    free(lon);
    free(lat);
    free(cdf);
    return NULL;
  }
  time_bnds=newvec(double,2*header.nyear*header.nstep);
  if(time_bnds==NULL)
  {
    printallocerr("year");
    free(lon);
    free(lat);
    free(cdf);
    free(year);
    return NULL;
  }
  switch(header.nstep)
  {
    case 1:
      if(with_days)
      {
        for(i=0;i<header.nyear;i++)
        {
          year[i]=(header.firstyear-baseyear+i*header.timestep+header.timestep)/2*365;
          time_bnds[2*i]=(header.firstyear-baseyear+i*header.timestep)*365;
          time_bnds[2*i+1]=(header.firstyear-baseyear+(i+1)*header.timestep)*365;
        }
      }
      else
      {
        if(absyear)
          for(i=0;i<header.nyear;i++)
          {
            year[i]=header.firstyear+i*header.timestep+header.timestep/2;
            time_bnds[2*i]=header.firstyear+i*header.timestep;
            time_bnds[2*i+1]=header.firstyear+(i+1)*header.timestep;
          }
        else
          for(i=0;i<header.nyear;i++)
          {
            year[i]=header.firstyear-baseyear+i*header.timestep+header.timestep/2;
            time_bnds[2*i]=header.firstyear-baseyear+i*header.timestep;
            time_bnds[2*i+1]=header.firstyear-baseyear+(i+1)*header.timestep;
          }
      }
      break;
    case 12:
      if(with_days)
      {
        for(i=0;i<header.nyear;i++)
          for(j=0;j<12;j++)
            if(i==0 && j==0)
            {
              year[0]=15+(header.firstyear-baseyear)*NDAYYEAR;
              time_bnds[0]=(header.firstyear-baseyear)*NDAYYEAR;
              time_bnds[1]=ndaymonth[j]-1+(header.firstyear-baseyear)*NDAYYEAR;
            }
            else
            {
              year[i*12+j]=year[i*12+j-1]+15;
              time_bnds[2*(i*12+j)]=year[i*12+j-1]+1;
              time_bnds[2*(i*12+j)+1]=year[i*12+j-1]+ndaymonth[j];
            }
      }
      else
        for(i=0;i<header.nyear*12;i++)
           year[i]=time_bnds[2*i]=time_bnds[2*i+1]=i+(header.firstyear-baseyear)*12;
      break;
    case NDAYYEAR:
      for(i=0;i<header.nyear*NDAYYEAR;i++)
        year[i]=time_bnds[2*i]=time_bnds[2*i+1]=i+(header.firstyear-baseyear)*NDAYYEAR;
      break;
    default:
      fprintf(stderr,"ERROR425: Invalid value=%d for number of data points per year, must be 1, 12 or 365.\n",
              header.nstep);
      free(year);
      free(lon);
      free(lat);
      free(cdf);
      return NULL;
  }
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
    free(year);
    free(lon);
    free(lat);
    free(cdf);
    return NULL;
  }
  rc=nc_def_dim(cdf->ncid,TIME_DIM_NAME,header.nyear*header.nstep,&time_dim_id);
  error(rc);
  rc=nc_def_dim(cdf->ncid,TIME_BNDS_NAME,2,&time_bnds_dim_id);
  error(rc);
  dimids[0]=time_dim_id;
  dimids[1]=time_bnds_dim_id;
  rc=nc_def_var(cdf->ncid,TIME_BNDS_NAME,NC_DOUBLE,2,dimids,&time_bnds_var_id);
  error(rc);
  rc=nc_def_var(cdf->ncid,TIME_NAME,NC_DOUBLE,1,&time_dim_id,&time_var_id);
  error(rc);
  rc=nc_def_dim(cdf->ncid,LAT_DIM_NAME,array->nlat,&lat_dim_id);
  error(rc);
  rc=nc_def_dim(cdf->ncid,LON_DIM_NAME,array->nlon,&lon_dim_id);
  error(rc);
  if(source!=NULL)
  {
    rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"source",strlen(source),source);
    error(rc);
  }
  time(&t);
  if(history!=NULL)
  {
    len=snprintf(NULL,0,"%s\n%s: %s",history,strdate(&t),cmdline);
    s=malloc(len+1);
    sprintf(s,"%s\n%s: %s",history,strdate(&t),cmdline);
  }
  else
  {
    len=snprintf(NULL,0,"%s: %s",strdate(&t),cmdline);
    s=malloc(len+1);
    sprintf(s,"%s: %s",strdate(&t),cmdline);
  }
  rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"history",strlen(s),s);
  free(s);
  for(i=0;i<n_global;i++)
  {
    rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,global_attrs[i].name,strlen(global_attrs[i].value),global_attrs[i].value);
    error(rc);
  }
  rc=nc_def_var(cdf->ncid,LAT_NAME,NC_DOUBLE,1,&lat_dim_id,&lat_var_id);
  error(rc);
  rc=nc_def_var(cdf->ncid,LON_NAME,NC_DOUBLE,1,&lon_dim_id,&lon_var_id);
  error(rc);
  if(header.nstep==1)
  {
    if(with_days)
    {
      len=snprintf(NULL,0,"days since %d-1-1 0:0:0",baseyear);
      s=malloc(len+1);
      sprintf(s,"days since %d-1-1 0:0:0",baseyear);
    }
    else
    {
      if(absyear)
        s=strdup(YEARS_NAME);
      else
      {
        len=snprintf(NULL,0,"years since %d-1-1 0:0:0",baseyear);
        s=malloc(len+1);
        sprintf(s,"years since %d-1-1 0:0:0",baseyear);
      }
    }
  }
  else if(header.nstep==12)
  {
    len=snprintf(NULL,0,"%s since %d-1-1 0:0:0",(with_days) ? "days" : "months",baseyear);
    s=malloc(len+1);
    sprintf(s,"%s since %d-1-1 0:0:0",(with_days) ? "days" : "months",baseyear);
  }
  else if(header.nstep==1)
  {
    len=snprintf(NULL,0,"days since %d-1-1 0:0:0",baseyear);
    s=malloc(len+1);
    sprintf(s,"days since %d-1-1 0:0:0",baseyear);
  }
  rc=nc_put_att_text(cdf->ncid,time_var_id,"units",strlen(s),s);
  free(s);
  error(rc);
  rc=nc_put_att_text(cdf->ncid,time_var_id,"calendar",strlen(CALENDAR),
                     CALENDAR);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, time_var_id,"standard_name",strlen(TIME_STANDARD_NAME),TIME_STANDARD_NAME);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, time_var_id,"long_name",strlen(TIME_LONG_NAME),TIME_LONG_NAME);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, time_var_id,"axis",strlen("T"),"T");
  error(rc);
  rc=nc_put_att_text(cdf->ncid,lon_var_id,"units",strlen("degrees_east"),
                     "degrees_east");
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"long_name",strlen(LON_LONG_NAME),LON_LONG_NAME);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"standard_name",strlen(LON_STANDARD_NAME),LON_STANDARD_NAME);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"axis",strlen("X"),"X");
  error(rc);
  rc=nc_put_att_text(cdf->ncid,lat_var_id,"units",strlen("degrees_north"),
                     "degrees_north");
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"long_name",strlen(LAT_LONG_NAME),LAT_LONG_NAME);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"standard_name",strlen(LAT_STANDARD_NAME),LAT_STANDARD_NAME);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"axis",strlen("Y"),"Y");
  error(rc);
  if(map!=NULL)
  {
    if(ispft)
    {
      rc=nc_def_dim(cdf->ncid,(map->isfloat) ? DEPTH_NAME : "npft",header.nbands,&pft_dim_id);
      error(rc);
    }
    if(getmapsize(map)==header.nbands)
      dim2[0]=pft_dim_id;
    else
    {
      rc=nc_def_dim(cdf->ncid,map_name,getmapsize(map),&map_dim_id);
      error(rc);
      dim2[0]=map_dim_id;
    }
    if(map->isfloat)
    {
      rc=nc_def_var(cdf->ncid,DEPTH_NAME,NC_DOUBLE,1,dim2,&varid);
      error(rc);
      rc=nc_put_att_text(cdf->ncid,varid,"units",strlen("m"),"m");
      error(rc);
      rc=nc_put_att_text(cdf->ncid,varid,"standard_name",strlen(DEPTH_STANDARD_NAME),DEPTH_STANDARD_NAME);
      error(rc);
      rc=nc_put_att_text(cdf->ncid,varid,"long_name",strlen(DEPTH_LONG_NAME),DEPTH_LONG_NAME);
      error(rc);
      rc=nc_put_att_text(cdf->ncid,varid,"bounds",strlen(BNDS_NAME),BNDS_NAME);
      error(rc);
      rc=nc_put_att_text(cdf->ncid,varid,"positive",strlen("down"),"down");
      error(rc);
      rc=nc_put_att_text(cdf->ncid,varid,"axis",strlen("Z"),"Z");
      error(rc);
      rc=nc_def_dim(cdf->ncid,BNDS_NAME,2,&bnds_dim_id);
      error(rc);
      dimids[0]=dim2[0];
      dimids[1]=bnds_dim_id;
      rc=nc_def_var(cdf->ncid,BNDS_NAME,NC_DOUBLE,2,dimids,&bnds_var_id);
      error(rc);
      rc=nc_put_att_text(cdf->ncid,bnds_var_id,"units",strlen("m"),"m");
      error(rc);
      rc=nc_put_att_text(cdf->ncid,bnds_var_id,"comment",strlen(BNDS_LONG_NAME),BNDS_LONG_NAME);
    }
    else
    {
      len=0;
      for(i=0;i<getmapsize(map);i++)
        if(getmapitem(map,i)==NULL)
          len=max(len,strlen(NULL_NAME));
        else
          len=max(len,strlen(getmapitem(map,i)));
      rc=nc_def_dim(cdf->ncid,"len",len+1,&len_dim_id);
      error(rc);
      dim2[1]=len_dim_id;
      rc=nc_def_var(cdf->ncid,MAP_NAME,NC_CHAR,2,dim2,&varid);
      error(rc);
    }
  }
  else if(ispft)
  {
    rc=nc_def_dim(cdf->ncid,"npft",header.nbands,&pft_dim_id);
    error(rc);
  }
  if(ispft)
  {
    dim[0]=time_dim_id;
    dim[1]=pft_dim_id;
    dim[2]=lat_dim_id;
    dim[3]=lon_dim_id;
    chunk[0]=chunk[1]=1;
    chunk[2]=array->nlat;
    chunk[3]=array->nlon;
  }
  else
  {
    dim[0]=time_dim_id;
    dim[1]=lat_dim_id;
    dim[2]=lon_dim_id;
    chunk[0]=1;
    chunk[1]=array->nlat;
    chunk[2]=array->nlon;
  }
  switch(type)
  {
    case LPJ_FLOAT:
      rc=nc_def_var(cdf->ncid,name,NC_FLOAT,(ispft) ? 4 : 3,dim,&cdf->varid);
      break;
    case LPJ_SHORT:
      rc=nc_def_var(cdf->ncid,name,NC_SHORT,(ispft) ? 4 : 3,dim,&cdf->varid);
      break;
    default:
      fprintf(stderr,"Invalid datatype %d.\n",type);
      return NULL;
  }
  error(rc);
#ifdef USE_NETCDF4
  rc=nc_def_var_chunking(cdf->ncid, cdf->varid, NC_CHUNKED,chunk);
  error(rc);
  if(compress)
  {
    rc=nc_def_var_deflate(cdf->ncid, cdf->varid, 0, 1,compress);
    error(rc);
  }
#endif
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
      nc_put_att_short(cdf->ncid, cdf->varid,"missing_value",NC_SHORT,1,&miss_short);
      rc=nc_put_att_short(cdf->ncid, cdf->varid,"_FillValue",NC_SHORT,1,&miss_short);
      break;
    default:
      break;
  }
  error(rc);
  rc=nc_enddef(cdf->ncid);
  error(rc);
  rc=nc_put_var_double(cdf->ncid,time_var_id,year);
  error(rc);
  rc=nc_put_var_double(cdf->ncid,time_bnds_var_id,time_bnds);
  error(rc);
  rc=nc_put_var_double(cdf->ncid,lat_var_id,lat);
  error(rc);
  rc=nc_put_var_double(cdf->ncid,lon_var_id,lon);
  error(rc);
  if(map!=NULL)
  {
    if(map->isfloat)
    {
      layer=newvec(double,getmapsize(map));
      check(layer);
      midlayer=newvec(double,getmapsize(map));
      check(midlayer);
      bnds=newvec(double,2*getmapsize(map));
      check(bnds);
      for(i=0;i<getmapsize(map);i++)
        layer[i]=*((double *)getmapitem(map,i))/1000;
      bnds[0]=0;
      bnds[1]=layer[0];
      midlayer[0]=0.5*layer[0];
      for(i=1;i<getmapsize(map);i++)
      {
        bnds[2*i]=layer[i-1];
        bnds[2*i+1]=layer[i];
        midlayer[i]=0.5*(layer[i-1]+layer[i]);
      }
      rc=nc_put_var_double(cdf->ncid,varid,midlayer);
      error(rc);
      rc=nc_put_var_double(cdf->ncid,bnds_var_id,bnds);
      error(rc);
      free(layer);
      free(midlayer);
      free(bnds);
    }
    else
    {
      count[0]=1;
      offset[1]=0;
      for(i=0;i<getmapsize(map);i++)
      {
        offset[0]=i;
        if(getmapitem(map,i)==NULL)
        {
          count[1]=strlen(NULL_NAME)+1;
          rc=nc_put_vara_text(cdf->ncid,varid,offset,count,NULL_NAME);
        }
        else
        {
          count[1]=strlen(getmapitem(map,i))+1;
          rc=nc_put_vara_text(cdf->ncid,varid,offset,count,getmapitem(map,i));
        }
        error(rc);
      }
    }
  }
  free(lat);
  free(lon);
  free(year);
  return cdf;
} /* of 'create_cdf' */

static Bool write_float_cdf(const Cdf *cdf,const float vec[],int year,
                            int size,Bool ispft,int nband,float miss)
{
  int i,rc;
  size_t offsets[4],counts[4];
  float *grid;
  grid=newvec(float,cdf->index->nlon*cdf->index->nlat);
  if(grid==NULL)
  {
    printallocerr("grid");
    return TRUE;
  }
  for(i=0;i<cdf->index->nlon*cdf->index->nlat;i++)
    grid[i]=miss;
  for(i=0;i<size;i++)
    grid[cdf->index->index[i]]=vec[i];
  if(year==NO_TIME)
    rc=nc_put_var_float(cdf->ncid,cdf->varid,grid);
  else
  {
    offsets[0]=year;
    counts[0]=1;
    if(ispft)
    {
      counts[1]=1;
      counts[2]=cdf->index->nlat;
      counts[3]=cdf->index->nlon;
      offsets[1]=nband;
      offsets[2]=offsets[3]=0;
    }
    else
    {
      counts[1]=cdf->index->nlat;
      counts[2]=cdf->index->nlon;
      offsets[1]=offsets[2]=0;
    }
    rc=nc_put_vara_float(cdf->ncid,cdf->varid,offsets,counts,grid);
  }
  free(grid);
  if(rc!=NC_NOERR)
  {
    fprintf(stderr,"ERROR431: Cannot write output data: %s.\n",
            nc_strerror(rc));
    return TRUE;
  }
  return FALSE;
} /* of 'write_float_cdf' */

static Bool write_short_cdf(const Cdf *cdf,const short vec[],int year,
                            int size,Bool ispft,int nband,short miss)
{
  int i,rc;
  size_t offsets[4],counts[4];
  short *grid;
  grid=newvec(short,cdf->index->nlon*cdf->index->nlat);
  if(grid==NULL)
  {
    printallocerr("grid");
    return TRUE;
  }
  for(i=0;i<cdf->index->nlon*cdf->index->nlat;i++)
    grid[i]=miss;
  for(i=0;i<size;i++)
    grid[cdf->index->index[i]]=vec[i];
  if(year==NO_TIME)
    rc=nc_put_var_short(cdf->ncid,cdf->varid,grid);
  else
  {
    offsets[0]=year;
    counts[0]=1;
    if(ispft)
    {
      counts[1]=1;
      counts[2]=cdf->index->nlat;
      counts[3]=cdf->index->nlon;
      offsets[1]=nband;
      offsets[2]=offsets[3]=0;
    }
    else
    {
      counts[1]=cdf->index->nlat;
      counts[2]=cdf->index->nlon;
      offsets[1]=offsets[2]=0;
    }
    rc=nc_put_vara_short(cdf->ncid,cdf->varid,offsets,counts,grid);
  }
  free(grid);
  if(rc!=NC_NOERR)
  {
    fprintf(stderr,"ERROR431: Cannot write output data: %s.\n",
            nc_strerror(rc));
    return TRUE;
  }
  return FALSE;
} /* of 'write_short_cdf' */


static void close_cdf(Cdf *cdf)
{
  nc_close(cdf->ncid);
  free(cdf);
} /* of 'close_cdf' */

#endif

int main(int argc,char **argv)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  FILE *file,*gridfile;
  Intcoord intcoord;
  Coord_array *index;
  Coord *grid,res;
  Cdf *cdf;
  Header header;
  float *data=NULL;
  short *data_short=NULL;
  int i,j,k,ngrid,iarg,compress,version,n_global,n_global2,baseyear;
  Bool swap,ispft,isshort,isglobal,isclm,ismeta,isbaseyear,revlat,withdays,absyear;
  Type gridtype;
  float cellsize,fcoord[2];
  double dcoord[2];
  char *units,*long_name,*endptr,*cmdline,*pos,*outname,*missing_value;
  Filename coord_filename;
  float cellsize_lon,cellsize_lat;
  float miss=MISSING_VALUE_FLOAT;
  short miss_short=MISSING_VALUE_SHORT;
  Coordfile coordfile;
  Map *map=NULL;
  Attr *global_attrs=NULL;
  Attr *global_attrs2=NULL;
  size_t offset;
  char *map_name,*filename;
  char *var_units=NULL,*var_long_name=NULL,*var_name=NULL,*var_standard_name=NULL;
  char *source=NULL,*history=NULL;
  Filename grid_name;
  char *variable,*grid_filename,*path;
  grid_name.fmt=RAW;
  units=long_name=NULL;
  compress=0;
  swap=isglobal=absyear=FALSE;
  res.lon=res.lat=header.cellsize_lon=header.cellsize_lat=0.5;
  header.firstyear=1901;
  header.nbands=1;
  header.nstep=1;
  header.timestep=1;
  ispft=FALSE;
  isshort=FALSE;
  gridtype=LPJ_SHORT;
  isclm=FALSE;
  ismeta=FALSE;
  isbaseyear=FALSE;
  revlat=FALSE;
  withdays=FALSE;
  map_name=BAND_NAMES;
  n_global=0;
  missing_value=NULL;
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-h") || !strcmp(argv[iarg],"--help"))
      {
        printf("   bin2cdf (" __DATE__ ") Help\n"
               "   ==========================\n\n"
               "Convert binary output into NetCDF files for LPJmL version " LPJ_VERSION "\n\n");
        printf(USAGE
               "\nArguments:\n"
               "-h,--help        print this help text\n"
               "-v,--version     print LPJmL version\n"
               "-clm             file is in CLM format, default is raw\n"
               "-floatgrid       set data type of grid file to float, default is short\n"
               "-doublegrid      set data type of grid file to double, default is short\n"
               "-revlat          reverse order of latitudes in NetCDF file\n"
               "-days            use days as units for monthly output\n"
               "-absyear         absolute year instead of relative to base year\n"
               "-cellsize s      set cell size, default is %g\n"
               "-compress l      set compression level for NetCDF4 files\n"
               "-attr name=value set global attribute name to value in NetCDF file\n"
               "-descr d         set long name in NetCDF file\n"
               "-units u         set units in NetCDF file\n"
               "-missing_value v set missing value to v\n"
               "-global          use global grid for NetCDF file\n"
               "-swap            change byte order in binary file\n"
               "-short           data type of NetCDF data is short, default is float\n"
               "-nbands n        number of bands, default is 1\n"
               "-nstep n         number of steps per year, default is 1\n"
               "-ispft           output is PFT-specific\n"
               "-firstyear f     first year, default is %d\n"
               "-baseyear y      base year of annual time axis, default is firstyear\n"
               "-metafile        set the input format to JSON metafile instead of raw\n"
               "-map name        name of map in JSON metafile, default is \"band_names\"\n"
               "varname          variable name in NetCDF file\n"
               "gridfile         filename of grid data file\n"
               "binfile          filename of binary data file\n"
               "netcdffile       filename of NetCDF file created\n\n"
               "(C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file\n",
               argv[0],header.cellsize_lon,header.firstyear);
        return EXIT_SUCCESS;
      }
      else if(!strcmp(argv[iarg],"-v") || !strcmp(argv[iarg],"--version"))
      {
        puts(LPJ_VERSION);
        return EXIT_SUCCESS;
      }
      else if(!strcmp(argv[iarg],"-units"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Error: Missing argument after option '-units'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        units=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-floatgrid"))
        gridtype=LPJ_FLOAT;
      else if(!strcmp(argv[iarg],"-doublegrid"))
        gridtype=LPJ_DOUBLE;
      else if(!strcmp(argv[iarg],"-ispft"))
        ispft=TRUE;
      else if(!strcmp(argv[iarg],"-clm"))
        isclm=TRUE;
      else if(!strcmp(argv[iarg],"-metafile"))
        ismeta=TRUE;
      else if(!strcmp(argv[iarg],"-short"))
        isshort=TRUE;
      else if(!strcmp(argv[iarg],"-global"))
        isglobal=TRUE;
      else if(!strcmp(argv[iarg],"-swap"))
        swap=TRUE;
      else if(!strcmp(argv[iarg],"-revlat"))
        revlat=TRUE;
      else if(!strcmp(argv[iarg],"-days"))
        withdays=TRUE;
      else if(!strcmp(argv[iarg],"-absyear"))
        absyear=TRUE;
      else if(!strcmp(argv[iarg],"-descr"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Error: Missing argument after option '-descr'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        long_name=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-missing_value"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Error: Missing argument after option '-missing_value'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        missing_value=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-attr"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,
                  "Error: Argument missing for '-attr' option.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        pos=strchr(argv[++iarg],'=');
        if(pos==NULL)
        {
          fprintf(stderr,
                  "Error: Missing '=' for '-attr' option.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        *pos='\0';
        global_attrs=realloc(global_attrs,(n_global+1)*sizeof(Attr));
        check(global_attrs);
        global_attrs[n_global].name=strdup(argv[iarg]);
        check(global_attrs[n_global].name);
        global_attrs[n_global].value=strdup(pos+1);
        check(global_attrs[n_global].value);
        n_global++;
      }
      else if(!strcmp(argv[iarg],"-map"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Error: Missing argument after option '-map'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        map_name=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-nbands"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Error: Missing argument after option '-nbands'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        header.nbands=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-nitem'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
        if(header.nbands<=0)
        {
          fputs("Error: Number of bands must be greater than zero.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-nstep"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Error: Missing argument after option '-nstep'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        header.nstep=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-nstep'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
        if(header.nstep!=1 && header.nstep!=NMONTH && header.nstep!=NDAYYEAR)
        {
          fprintf(stderr,"Error: Number of steps=%d must be 1, 12, or 365.\n",header.nstep);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-firstyear"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Error: Missing argument after option '-firstyear'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        header.firstyear=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-firstyear'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-baseyear"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Error: Missing argument after option '-baseyear'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        baseyear=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-baseyear'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
        isbaseyear=TRUE;
      }
      else if(!strcmp(argv[iarg],"-cellsize"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Error: Missing argument after option '-cellsize'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        cellsize=(float)strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-cellsize'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
        res.lon=res.lat=header.cellsize_lon=header.cellsize_lat=cellsize;
      }
      else if(!strcmp(argv[iarg],"-compress"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Error: Missing argument after option '-compress'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        compress=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-compress'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else
      {
        fprintf(stderr,"Error: Invalid option '%s'.\n"
                USAGE,argv[iarg],argv[0]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(ismeta && argc==iarg+2)
  {
    filename=argv[iarg];
    outname=argv[iarg+1];
  }
  else if(argc<iarg+4)
  {
    fprintf(stderr,"Error: Missing argument(s).\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  else
  {
    filename=argv[iarg+2];
    outname=argv[iarg+3];
  }
  if(ismeta)
  {
    file=openmetafile(&header,&map,map_name,&global_attrs2,&n_global2,&source,&history,&var_name,&var_units,&var_standard_name,&var_long_name,&grid_name,&gridtype,NULL,&swap,&offset,filename,TRUE);
    if(file==NULL)
      return EXIT_FAILURE;
    if(units==NULL && var_units!=NULL)
      units=var_units;
    if(long_name==NULL && var_long_name!=NULL)
      long_name=var_long_name;
    if(global_attrs2!=NULL)
    {
      mergeattrs(&global_attrs,&n_global,global_attrs2,n_global2,FALSE);
      freeattrs(global_attrs2,n_global2);
    }
  }
  if(argc!=iarg+2)
  {
    variable=argv[iarg];
    grid_filename=argv[iarg+1];
  }
  else
  {
    if(var_name==NULL)
    {
      fprintf(stderr,"Error: variable name must be specified in '%s' metafile.\n",filename);
      return EXIT_FAILURE;
    }
    if(grid_name.name==NULL)
    {
      fprintf(stderr,"Error: grid filename must be specified in '%s' metafile.\n",filename);
      return EXIT_FAILURE;
    }
    variable=var_name;
    path=getpath(filename);
    grid_filename=addpath(grid_name.name,path);
    if(grid_filename==NULL)
    {
      printallocerr("name");
      return EXIT_FAILURE;
    }
    free(grid_name.name);
    free(path);
  }
  if(isclm || grid_name.fmt==CLM || grid_name.fmt==META)
  {
    coord_filename.name=grid_filename;
    coord_filename.fmt=(isclm) ? CLM : grid_name.fmt;
    coordfile=opencoord(&coord_filename,TRUE);
    if(coordfile==NULL)
      return EXIT_FAILURE;
    grid=newvec(Coord,numcoord(coordfile));
    if(grid==NULL)
    {
      printallocerr("grid");
      return EXIT_FAILURE;
    }
    ngrid=numcoord(coordfile);
    if(ngrid==0)
    {
      fprintf(stderr,"Number of cells is zero in '%s'.\n",grid_filename);
      return EXIT_FAILURE;
    }
    getcellsizecoord(&cellsize_lon,&cellsize_lat,coordfile);
    res.lat=cellsize_lat;
    res.lon=cellsize_lon;
    for(j=0;j<numcoord(coordfile);j++)
      readcoord(coordfile,grid+j,&res);
    closecoord(coordfile);
  }
  else
  {
    gridfile=fopen(grid_filename,"rb");
    if(gridfile==NULL)
    {
      fprintf(stderr,"Error opening grid file '%s': %s.\n",grid_filename,
              strerror(errno));
      return EXIT_FAILURE;
    }
    switch(gridtype)
    {
      case LPJ_SHORT:
        ngrid=getfilesizep(gridfile)/sizeof(short)/2;
        if(getfilesizep(gridfile) % (sizeof(short)/2))
          fprintf(stderr,"Size of grid file '%s' is non multiple of coord size.\n",grid_filename);
        break;
      case LPJ_FLOAT:
        ngrid=getfilesizep(gridfile)/sizeof(float)/2;
        if(getfilesizep(gridfile) % (sizeof(float)/2))
          fprintf(stderr,"Size of grid file '%s' is non multiple of coord size.\n",grid_filename);
        break;
      case LPJ_DOUBLE:
        ngrid=getfilesizep(gridfile)/sizeof(double)/2;
        if(getfilesizep(gridfile) % (sizeof(double)/2))
          fprintf(stderr,"Size of grid file '%s' is non multiple of coord size.\n",grid_filename);
        break;
      default:
        fprintf(stderr,"Invalid datatype %d in '%s'.\n",gridtype,grid_filename);
        return EXIT_FAILURE;
    }
    if(ngrid==0)
    {
       fprintf(stderr,"Error: Number of grid cells in '%s' is zero.\n",grid_filename);
       return EXIT_FAILURE;
    }
    grid=newvec(Coord,ngrid);
    if(grid==NULL)
    {
      printallocerr("grid");
      return EXIT_FAILURE;
    }
    switch(gridtype)
    {
      case LPJ_FLOAT:
        for(i=0;i<ngrid;i++)
        {
          freadfloat(fcoord,2,swap,gridfile);
          grid[i].lon=fcoord[0];
          grid[i].lat=fcoord[1];
        }
        break;
      case LPJ_DOUBLE:
        for(i=0;i<ngrid;i++)
        {
          freaddouble(dcoord,2,swap,gridfile);
          grid[i].lon=dcoord[0];
          grid[i].lat=dcoord[1];
        }
        break;
      case LPJ_SHORT:
        for(i=0;i<ngrid;i++)
        {
          readintcoord(gridfile,&intcoord,swap);
          grid[i].lat=intcoord.lat*0.01;
          grid[i].lon=intcoord.lon*0.01;
        }
        break;
      default:
        break;
    }
    fclose(gridfile);
  }
  if(ismeta)
  {
    if(fseek(file,offset,SEEK_CUR))
    {
      fprintf(stderr,"Error seeking in '%s' to offset %lu.\n",filename,offset);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(header.ncell!=ngrid)
    {
      fprintf(stderr,"Number of cells=%d in '%s' does not match %d in grid file '%s'.\n",header.ncell,filename,ngrid,grid_filename);
      return EXIT_FAILURE;
    }
    if(header.nbands>1)
      ispft=TRUE;
    if(header.order!=CELLSEQ)
    {
      fprintf(stderr,"Error: Order in '%s' must be cellseq, order ",filename);
      if(header.order>0 || header.order<=CELLSEQ)
        fprintf(stderr,"%s",ordernames[header.order-1]);
      else
        fprintf(stderr,"%d",header.order);
      fprintf(stderr," is not supported.\n.");
      return EXIT_FAILURE;
    }
  }
  else
  {
    file=fopen(argv[iarg+2],"rb");
    if(file==NULL)
    {
      fprintf(stderr,"Error opening '%s': %s.\n",argv[iarg+2],strerror(errno));
      return EXIT_FAILURE;
    }
    if(isclm)
    {
      version=READ_VERSION;
      if(freadheader(file,&header,&swap,LPJOUTPUT_HEADER,&version,TRUE))
      {
        fprintf(stderr,"Error opening '%s': %s.\n",argv[iarg+2],strerror(errno));
        return EXIT_FAILURE;
      }
      if(header.ncell!=ngrid)
      {
        fprintf(stderr,"Number of cells=%d in '%s' does not match %d in grid file.\n",header.ncell,argv[iarg+2],ngrid);
        return EXIT_FAILURE;
      }
      if(version<4)
      {
        if(!ispft)
        {
          header.nstep=header.nbands;
          header.nbands=1;
        }
      }
      if(version>=3)
        isshort=header.datatype==LPJ_SHORT;
      if(header.nbands>1)
        ispft=TRUE;
      if(version>CLM_MAX_VERSION)
      {
        fprintf(stderr,"Error: Unsupported version %d in '%s', must be less than %d.\n",
                version,argv[iarg+2],CLM_MAX_VERSION+1);
        return EXIT_FAILURE;
      }
      if(header.cellsize_lat!=res.lat || header.cellsize_lon!=res.lon)
      {
        fprintf(stderr,"Resolution (%g,%g) in '%s' does not match (%g,%g) in grid file.\n",
                header.cellsize_lon,header.cellsize_lat,argv[iarg+2],res.lon,res.lat);
        return EXIT_FAILURE;
      }
      if(header.order!=CELLSEQ)
      {
        fprintf(stderr,"Error: Order in '%s' must be cellseq, order ",argv[iarg+2]);
        if(header.order>0 || header.order<=CELLSEQ)
          fprintf(stderr,"%s",ordernames[header.order-1]);
        else
          fprintf(stderr,"%d",header.order);
        fprintf(stderr," is not supported.\n.");
        return EXIT_FAILURE;
      }
    }
    else
    {
      if(header.nbands>1)
        ispft=TRUE;
      if(isshort)
      {
        header.nyear=getfilesizep(file)/sizeof(short)/ngrid/header.nbands/header.nstep;
        if(getfilesizep(file) % (sizeof(short)*ngrid*header.nbands*header.nstep))
          fprintf(stderr,"Warning: file size of '%s' is not multiple of bands %d, steps %d and number of cells %d.\n",argv[iarg+2],header.nbands,header.nstep,ngrid);
      }
      else
      {
        header.nyear=getfilesizep(file)/sizeof(float)/ngrid/header.nbands/header.nstep;
        if(getfilesizep(file) % (sizeof(float)*ngrid*header.nbands*header.nstep))
          fprintf(stderr,"Warning: file size of '%s' is not multiple of bands %d, steps %d  and number of cells %d.\n",argv[iarg+2],header.nbands,header.nstep,ngrid);
      }
    }
  }
  if(!isbaseyear)
    baseyear=header.firstyear;
  index=createindex(grid,ngrid,res,isglobal,revlat);
  if(index==NULL)
    return EXIT_FAILURE;
  free(grid);
  cmdline=catstrvec(argv,argc);
  if(missing_value!=NULL)
  {
    if(isshort)
    {
      miss_short=strtol(missing_value,&endptr,10);
      if(*endptr!='\0')
      {
        fprintf(stderr,"Invalid number '%s' for missing value.\n",missing_value);
        return EXIT_FAILURE;
      }
    }
    else
    {
      miss=strtod(missing_value,&endptr);
      if(*endptr!='\0')
      {
        fprintf(stderr,"Invalid number '%s' for missing value.\n",missing_value);
        return EXIT_FAILURE;
      }
    }
  }

  cdf=create_cdf(outname,map,map_name,cmdline,source,history,variable,units,var_standard_name,long_name,miss,miss_short,global_attrs,n_global,(isshort) ? LPJ_SHORT : LPJ_FLOAT,header,baseyear,ispft,compress,index,revlat,withdays,absyear);
  free(cmdline);
  if(cdf==NULL)
    return EXIT_FAILURE;
  if(isshort)
  {
    data_short=newvec(short,ngrid);
    if(data_short==NULL)
    {
      printallocerr("data");
      return EXIT_FAILURE;
    }
  }
  else
  {
    data=newvec(float,ngrid);
    if(data==NULL)
    {
      printallocerr("data");
      return EXIT_FAILURE;
    }
  }
  for(i=0;i<header.nyear;i++)
    for(j=0;j<header.nstep;j++)
      for(k=0;k<header.nbands;k++)
      {
        if(isshort)
        {
          if(freadshort(data_short,ngrid,swap,file)!=ngrid)
          {
            fprintf(stderr,"Error reading data in year %d.\n",i+header.firstyear);
            close_cdf(cdf);
            return EXIT_FAILURE;
          }
          if(write_short_cdf(cdf,data_short,i*header.nstep+j,ngrid,ispft,k,miss_short))
            return EXIT_FAILURE;
        }
        else
        {
          if(freadfloat(data,ngrid,swap,file)!=ngrid)
          {
            fprintf(stderr,"Error reading data in year %d.\n",i+header.firstyear);
            close_cdf(cdf);
            return EXIT_FAILURE;
          }
          if(write_float_cdf(cdf,data,i*header.nstep+j,ngrid,ispft,k,miss))
            return EXIT_FAILURE;
        }
      }
  close_cdf(cdf);
  fclose(file);
  if(isshort)
    free(data_short);
  else
    free(data);
  return EXIT_SUCCESS;
#else
  fprintf(stderr,"ERROR401: NetCDF is not supported in this version of %s.\n",argv[0]);
  return EXIT_FAILURE;
#endif
} /* of 'main' */

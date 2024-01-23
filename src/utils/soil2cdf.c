/**************************************************************************************/
/**                                                                                \n**/
/**                s  o  i  l  2  c  d  f  .  c                                    \n**/
/**                                                                                \n**/
/**     Converts soil data file into NetCDF data file                              \n**/
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

#define error(rc) if(rc) {free(lon);free(lat);fprintf(stderr,"ERROR427: Cannot write '%s': %s.\n",filename,nc_strerror(rc)); nc_close(cdf->ncid); free(cdf);return NULL;}

#define MISSING_VALUE 999
#define USAGE "%s [-scale s] [-global] [-compress level] [-descr d] name gridfile soilfile netcdffile\n"

typedef struct
{
  const Coord_array *index;
  int ncid;
  int varid;
} Cdf;

static Cdf *create_cdf(const char *filename,
                       const char *cmdline,
                       const char *name,
                       const char *descr,
                       Coord res,
                       int compress,
                       const Coord_array *array)
{
  Cdf *cdf;
  float *lon,*lat;
  short miss=MISSING_VALUE;
  int i,rc,dim[2];
  String s;
  time_t t;
  int lat_var_id,lon_var_id,lat_dim_id,lon_dim_id;
  cdf=new(Cdf);
  lon=newvec(float,array->nlon);
  if(lon==NULL)
  {
    printallocerr("lon");
    return NULL;
  }
  lat=newvec(float,array->nlat);
  if(lat==NULL)
  {
    printallocerr("lat");
    free(lon);
    free(cdf);
    return NULL;
  }
  cdf->index=array;
  for(i=0;i<array->nlon;i++)
    lon[i]=array->lon_min+i*res.lon;
  for(i=0;i<array->nlat;i++)
    lat[i]=array->lat_min+i*res.lat;
#ifdef USE_NETCDF4
  rc=nc_create(filename,(compress) ? NC_CLOBBER|NC_NETCDF4 : NC_CLOBBER,&cdf->ncid);
#else
  rc=nc_create(filename,NC_CLOBBER,&cdf->ncid);
#endif
  if(rc)
  {
    fprintf(stderr,"ERROR426: Cannot create file '%s': %s.\n",
            filename,nc_strerror(rc));
    free(lon);
    free(lat);
    free(cdf);
    return NULL;
  }
  rc=nc_def_dim(cdf->ncid,LAT_DIM_NAME,array->nlat,&lat_dim_id);
  error(rc);
  rc=nc_def_dim(cdf->ncid,LON_DIM_NAME,array->nlon,&lon_dim_id);
  error(rc);
  rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"source",strlen(cmdline),cmdline);
  error(rc);
  time(&t);
  snprintf(s,STRING_LEN,"Created for user %s on %s at %s",getuser(),gethost(),
           strdate(&t));
  rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"history",strlen(s),s);
  error(rc);
  rc=nc_def_var(cdf->ncid,LAT_NAME,NC_FLOAT,1,&lat_dim_id,&lat_var_id);
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
  rc=nc_put_att_text(cdf->ncid,lat_var_id,"units",strlen("degrees_north"),
                     "degrees_north");
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"long_name",strlen("latitude"),"latitude");
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"standard_name",strlen("latitude"),"latitude");
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"axis",strlen("Y"),"Y");
  error(rc);
  dim[0]=lat_dim_id;
  dim[1]=lon_dim_id;
  rc=nc_def_var(cdf->ncid,name,NC_SHORT,2,dim,&cdf->varid);
  error(rc);
#ifdef USE_NETCDF4
  if(compress)
  {
    rc=nc_def_var_deflate(cdf->ncid, cdf->varid, 0, 1,compress);
    error(rc);
  }
#endif
  if(descr!=NULL)
  {
    rc=nc_put_att_text(cdf->ncid, cdf->varid,"long_name",strlen(descr),descr);
    error(rc);
  }
  nc_put_att_short(cdf->ncid, cdf->varid,"missing_value",NC_SHORT,1,&miss);
  rc=nc_put_att_short(cdf->ncid, cdf->varid,"_FillValue",NC_SHORT,1,&miss);
  rc=nc_enddef(cdf->ncid);
  error(rc);
  rc=nc_put_var_float(cdf->ncid,lat_var_id,lat);
  error(rc);
  rc=nc_put_var_float(cdf->ncid,lon_var_id,lon);
  error(rc);
  free(lat);
  free(lon);
  return cdf;
} /* of 'create_cdf' */

static Bool write_short_cdf(const Cdf *cdf,const short vec[],int size)
{
  int i,rc;
  short *grid;
  grid=newvec(short,cdf->index->nlon*cdf->index->nlat);
  if(grid==NULL)
  {
    printallocerr("grid");
    return TRUE;
  }
  for(i=0;i<cdf->index->nlon*cdf->index->nlat;i++)
    grid[i]=MISSING_VALUE;
  for(i=0;i<size;i++)
    grid[cdf->index->index[i]]=vec[i];
  rc=nc_put_var_short(cdf->ncid,cdf->varid,grid);
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
  FILE *file;
  Coordfile coordfile;
  Coord_array *index;
  Coord *grid,res;
  Cdf *cdf;
  Byte *data;
  int i,ngrid,iarg,compress,rc;
  short *f;
  char *descr,*endptr,*cmdline;
  float lon,lat;
  Filename filename;
  Bool isglobal;
  descr=NULL;
  compress=0;
  isglobal=FALSE;
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-descr"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Missing argument after option '-descr'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        descr=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-global"))
        isglobal=TRUE;
      else if(!strcmp(argv[iarg],"-compress"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Missing argument after option '-compress'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        compress=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-compress'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[iarg],argv[0]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<iarg+4)
  {
    fprintf(stderr,"Missing arguments.\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  filename.fmt=CLM;
  filename.name=argv[iarg+1];
  coordfile=opencoord(&filename,TRUE);
  if(coordfile==NULL)
    return EXIT_FAILURE;
  ngrid=numcoord(coordfile);
  getcellsizecoord(&lon,&lat,coordfile);
  res.lon=lon;
  res.lat=lat;
  grid=newvec(Coord,numcoord(coordfile));
  if(grid==NULL)
  {
    printallocerr("grid"); 
    return EXIT_FAILURE;
  }
  for(i=0;i<numcoord(coordfile);i++)
    readcoord(coordfile,grid+i,&res);
  closecoord(coordfile);
  file=fopen(argv[iarg+2],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[iarg+2],strerror(errno));
    return EXIT_FAILURE;
  }
  if(getfilesize(argv[iarg+2])!=ngrid)
  {
    fprintf(stderr,"Number of cells in '%s' is different from %d in '%s'.\n",
            argv[iarg+2],ngrid,argv[iarg+1]);
    fclose(file);
    return EXIT_FAILURE;
  }
  index=createindex(grid,ngrid,res,isglobal);
  if(index==NULL)
    return EXIT_FAILURE;
  free(grid);
  cmdline=catstrvec(argv,argc); 
  cdf=create_cdf(argv[iarg+3],cmdline,argv[iarg],descr,res,compress,index);
  free(cmdline);
  if(cdf==NULL)
    return EXIT_FAILURE;
  data=newvec(Byte,ngrid);
  if(data==NULL)
  {
    printallocerr("data");
    return EXIT_FAILURE;
  }
  f=newvec(short,ngrid);
  if(f==NULL)
  {
    printallocerr("f");
    return EXIT_FAILURE;
  }
  rc=fread(data,1,ngrid,file);
  if(rc!=ngrid)
  {
    fprintf(stderr,"Error reading soil data, %d items read.\n",rc);
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid;i++)
    f[i]=data[i];
  rc=write_short_cdf(cdf,f,ngrid); 
  close_cdf(cdf);
  fclose(file);
  free(data);
  return (rc) ? EXIT_FAILURE : EXIT_SUCCESS;
#else
  fprintf(stderr,"ERROR401: NetCDF is not supported in this version of %s.\n",argv[0]);
  return EXIT_FAILURE;
#endif
} /* of 'main' */

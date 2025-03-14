/**************************************************************************************/
/**                                                                                \n**/
/**                c  l  m  2  c  d  f  .  c                                       \n**/
/**                                                                                \n**/
/**     Converts CLM data file into NetCDF data file                               \n**/
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

#define error(rc) if(rc) {free(lon);free(lat);free(year);fprintf(stderr,"ERROR427: Cannot write '%s': %s.\n",filename,nc_strerror(rc)); nc_close(cdf->ncid); free(cdf);return NULL;}

#define USAGE "Usage: %s [-h] [-v] [-scale s] [-longheader] [-global] [-cellsize size]\n       [-byte] [-int] [-float] [[-attr name=value] ...] [-intnetcdf]\n       [-metafile] [-raw] [-nbands n] [-landuse] [-notime] [-compress level]\n       [-units u] [-map name] [-descr d] [-missing_value val] [-config file] [-netcdf4]\n       [name gridfile] clmfile netcdffile\n"
#define ERR_USAGE USAGE "\nTry \"%s --help\" for more information.\n"

typedef struct
{
  const Coord_array *index;
  int ncid;
  int varid;
} Cdf;

static Cdf *create_cdf(const char *filename,
                       Map *map,
                       const char *source,
                       const char *history,
                       const char *name,
                       const char *units,
                       const char *standard_name,
                       const char *long_name,
                       Netcdf_config *netcdf_config,
                       const char *args,
                       const Attr *global_attrs,
                       int n_global,
                       const Header *header,
                       int compress,
                       Bool landuse,
                       Bool notime,
                       Bool isint,
                       Bool isnetcdf4,
                       const Coord_array *array)
{
  Cdf *cdf;
  double *lon,*lat;
  int *year,i,j,rc,dim[4],varid;
  char *s;
  const char *ptr;
  time_t t;
  size_t chunk[4];
  int len_dim_id;
  size_t offset[2],count[2];
  int time_var_id,lat_var_id,lon_var_id,time_dim_id,lat_dim_id,lon_dim_id,map_dim_id;
  int landuse_dim_id;
  int index;
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
    lon[i]=array->lon_min+i*header->cellsize_lon;
  for(i=0;i<array->nlat;i++)
    lat[i]=array->lat_min+i*header->cellsize_lat;
  if(notime)
    year=NULL;
  else
  {
    year=newvec(int,(landuse) ?  header->nyear : header->nyear*header->nbands);
    if(year==NULL)
    {
      printallocerr("year");
      free(lon);
      free(lat);
      free(cdf);
      return NULL;
    }
    if(landuse)
      for(i=0;i<header->nyear;i++)
        year[i]=i;
    else switch(header->nbands)
    {
      case 1:
        for(i=0;i<header->nyear;i++)
          year[i]=i;
        break;
      case NMONTH:
        for(i=0;i<header->nyear;i++)
          for(j=0;j<12;j++)
            if(i==0 && j==0)
              year[0]=ndaymonth[j]-1;
            else
              year[i*12+j]=year[i*12+j-1]+ndaymonth[j];
        break;
      case NDAYYEAR:
        for(i=0;i<header->nyear*NDAYYEAR;i++)
          year[i]=i;
        break;
      default:
        fprintf(stderr,"ERROR425: Invalid value=%d for number of data points per year, must be 1, 12 or 365.\n",
                header->nbands);
        free(year);
        free(lon);
        free(lat);
        free(cdf);
        return NULL;
    }
  }
  cdf->index=array;
  rc=nc_create(filename,(isnetcdf4) ? NC_CLOBBER|NC_NETCDF4 : NC_CLOBBER,&cdf->ncid);
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
  if(!notime)
  {
    rc=nc_def_dim(cdf->ncid,netcdf_config->time.dim,(landuse) ? header->nyear : header->nyear*header->nbands,&time_dim_id);
    error(rc);
    rc=nc_def_var(cdf->ncid,netcdf_config->time.name,NC_INT,1,&time_dim_id,&time_var_id);
    error(rc);
  }
  rc=nc_def_dim(cdf->ncid,netcdf_config->lat.dim,array->nlat,&lat_dim_id);
  error(rc);
  rc=nc_def_dim(cdf->ncid,netcdf_config->lon.dim,array->nlon,&lon_dim_id);
  error(rc);
  if(source!=NULL)
  {
    rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"source",strlen(source),source);
    error(rc);
  }
  time(&t);
  if(history!=NULL)
  {
    s=getsprintf("%s\n%s: %s",history,strdate(&t),args);
  }
  else
  {
    s=getsprintf("%s: %s",strdate(&t),args);
  }
  check(s);
  rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"history",strlen(s),s);
  free(s);
  error(rc);
  for(i=0;i<n_global;i++)
  {
    rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,global_attrs[i].name,strlen(global_attrs[i].value),global_attrs[i].value);
    error(rc);
  }
  rc=nc_def_var(cdf->ncid,netcdf_config->lat.name,NC_DOUBLE,1,&lat_dim_id,&lat_var_id);
  error(rc);
  rc=nc_def_var(cdf->ncid,netcdf_config->lon.name,NC_DOUBLE,1,&lon_dim_id,&lon_var_id);
  error(rc);
  if(!notime)
  {
    if(landuse || header->nbands==1)
    {
      s=getsprintf("years since %d-1-1 0:0:0",header->firstyear);
    }
    else if(header->nbands>1)
    {
      s=getsprintf("days since %d-1-1 0:0:0",header->firstyear);
    }
    check(s);
    rc=nc_put_att_text(cdf->ncid,time_var_id,"units",strlen(s),s);
    free(s);
    error(rc);
    rc=nc_put_att_text(cdf->ncid,time_var_id,"calendar",strlen(netcdf_config->calendar),netcdf_config->calendar);
    error(rc);
    rc=nc_put_att_text(cdf->ncid, time_var_id,"axis",strlen("T"),"T");
    error(rc);
    rc=nc_put_att_text(cdf->ncid, time_var_id,"standard_name",
                       strlen(netcdf_config->time.standard_name),
                       netcdf_config->time.standard_name);
    error(rc);
    rc=nc_put_att_text(cdf->ncid, time_var_id,"long_name",
                       strlen(netcdf_config->time.long_name),
                       netcdf_config->time.long_name);
    error(rc);
  }
  rc=nc_put_att_text(cdf->ncid,lon_var_id,"units",
                     strlen(netcdf_config->lon.unit),
                     netcdf_config->lon.unit);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"long_name",
                     strlen(netcdf_config->lon.long_name),
                     netcdf_config->lon.long_name);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"standard_name",
                     strlen(netcdf_config->lon.standard_name),
                     netcdf_config->lon.standard_name);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"axis",strlen("X"),"X");
  error(rc);
  rc=nc_put_att_text(cdf->ncid,lat_var_id,"units",
                     strlen(netcdf_config->lat.unit),
                     netcdf_config->lat.unit);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"long_name",
                     strlen(netcdf_config->lat.long_name),
                     netcdf_config->lat.long_name);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"standard_name",
                     strlen(netcdf_config->lat.standard_name),
                     netcdf_config->lat.standard_name);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"axis",strlen("Y"),"Y");
  error(rc);
  if(map!=NULL)
  {
    rc=nc_def_dim(cdf->ncid,MAP_NAME,getmapsize(map),&map_dim_id);
    error(rc);
    if(map->isfloat)
    {
      rc=nc_def_var(cdf->ncid,MAP_NAME,NC_DOUBLE,1,&map_dim_id,&varid);
      error(rc);
    }
    else
    {
      if(isnetcdf4)
        rc=nc_def_var(cdf->ncid,MAP_NAME,NC_STRING,1,&map_dim_id,&varid);
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
        dim[0]=map_dim_id;
        dim[1]=len_dim_id;
        rc=nc_def_var(cdf->ncid,MAP_NAME,NC_CHAR,2,dim,&varid);
      }
      error(rc);
    }
  }
  if(notime)
    index=0;
  else
  {
    dim[0]=time_dim_id;
    chunk[0]=1;
    index=1;
  }
  if(landuse)
  {
    rc=nc_def_dim(cdf->ncid,"pft",header->nbands,&landuse_dim_id);
    error(rc);
    dim[index]=landuse_dim_id;
    dim[index+1]=lat_dim_id;
    dim[index+2]=lon_dim_id;
    chunk[index]=1;
    chunk[index+1]=array->nlat;
    chunk[index+2]=array->nlon;
  }
  else
  {
    dim[index]=lat_dim_id;
    dim[index+1]=lon_dim_id;
    chunk[index]=array->nlat;
    chunk[index+1]=array->nlon;
  }
  if(notime)
    rc=nc_def_var(cdf->ncid,name,(isint) ? NC_INT : NC_FLOAT,(landuse) ? 3 : 2,dim,&cdf->varid);
  else
    rc=nc_def_var(cdf->ncid,name,(isint) ? NC_INT : NC_FLOAT,(landuse) ? 4 : 3,dim,&cdf->varid);
  error(rc);
  if(isnetcdf4)
  {
    rc=nc_def_var_chunking(cdf->ncid, cdf->varid, NC_CHUNKED,chunk);
    error(rc);
    if(compress)
    {
      rc=nc_def_var_deflate(cdf->ncid, cdf->varid, 0, 1,compress);
      error(rc);
    }
  }
  if(units!=NULL)
  {
    rc=nc_put_att_text(cdf->ncid, cdf->varid,"units",strlen(units),units);
    error(rc);
  }
  if(standard_name!=NULL)
  {
    rc=nc_put_att_text(cdf->ncid, cdf->varid,"standard_name",strlen(standard_name),standard_name);
    error(rc);
  }
  if(long_name!=NULL)
  {
    rc=nc_put_att_text(cdf->ncid, cdf->varid,"long_name",strlen(long_name),long_name);
    error(rc);
  }
  if(isint)
  {
    nc_put_att_int(cdf->ncid, cdf->varid,"missing_value",NC_INT,1,&netcdf_config->missing_value.i);
    rc=nc_put_att_int(cdf->ncid, cdf->varid,"_FillValue",NC_INT,1,&netcdf_config->missing_value.i);
  }
  else
  {
    nc_put_att_float(cdf->ncid, cdf->varid,"missing_value",NC_FLOAT,1,&netcdf_config->missing_value.f);
    rc=nc_put_att_float(cdf->ncid, cdf->varid,"_FillValue",NC_FLOAT,1,&netcdf_config->missing_value.f);
  }
  rc=nc_enddef(cdf->ncid);
  error(rc);
  if(!notime)
  {
    rc=nc_put_var_int(cdf->ncid,time_var_id,year);
    error(rc);
    free(year);
  }
  rc=nc_put_var_double(cdf->ncid,lat_var_id,lat);
  error(rc);
  rc=nc_put_var_double(cdf->ncid,lon_var_id,lon);
  error(rc);
  if(map!=NULL)
  {
    offset[1]=0;
    count[0]=1;
    if(map->isfloat)
      for(i=0;i<getmapsize(map);i++)
      {
        offset[0]=i;
        rc=nc_put_vara_double(cdf->ncid,varid,offset,count,(double *)getmapitem(map,i));
        error(rc);
      }
    else
      for(i=0;i<getmapsize(map);i++)
      {
        offset[0]=i;
        if(getmapitem(map,i)==NULL)
        {
          if(isnetcdf4)
          {
            ptr=NULL_NAME;
            rc=nc_put_vara_string(cdf->ncid,varid,offset,count,&ptr);
          }
          else
          {
            count[1]=strlen(NULL_NAME)+1;
            rc=nc_put_vara_text(cdf->ncid,varid,offset,count,NULL_NAME);
          }
        }
        else
        {
          if(isnetcdf4)
          {
            ptr=getmapitem(map,i);
            rc=nc_put_vara_string(cdf->ncid,varid,offset,count,&ptr);
          }
          else
          {
            count[1]=strlen(getmapitem(map,i))+1;
            rc=nc_put_vara_text(cdf->ncid,varid,offset,count,getmapitem(map,i));
          }
        }
        error(rc);
    }
  }
  free(lat);
  free(lon);
  return cdf;
} /* of 'create_cdf' */

static Bool write_float_cdf(const Cdf *cdf,const float vec[],int year,
                            int size,Bool landuse,Bool notime,int nband,float miss)
{
  int i,rc,index;
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
    if(notime)
      index=0;
    else
    {
      index=1;
      offsets[0]=year;
      counts[0]=1;
    }
    if(landuse)
    {
      counts[index]=1;
      counts[index+1]=cdf->index->nlat;
      counts[index+2]=cdf->index->nlon;
      offsets[index]=nband;
      offsets[index+1]=offsets[index+2]=0;
    }
    else
    {
      counts[index]=cdf->index->nlat;
      counts[index+1]=cdf->index->nlon;
      offsets[index]=offsets[index+1]=0;
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

static Bool write_int_cdf(const Cdf *cdf,const int vec[],int year,
                          int size,Bool landuse,Bool notime,int nband,int imiss)
{
  int i,rc,index;
  size_t offsets[4],counts[4];
  int *grid;
  grid=newvec(int,cdf->index->nlon*cdf->index->nlat);
  if(grid==NULL)
  {
    printallocerr("grid");
    return TRUE;
  }
  for(i=0;i<cdf->index->nlon*cdf->index->nlat;i++)
    grid[i]=imiss;
  for(i=0;i<size;i++)
    grid[cdf->index->index[i]]=vec[i];
  if(year==NO_TIME)
    rc=nc_put_var_int(cdf->ncid,cdf->varid,grid);
  else
  {
    if(notime)
      index=0;
    else
    {
      index=1;
      offsets[0]=year;
      counts[0]=1;
    }
    if(landuse)
    {
      counts[index]=1;
      counts[index+1]=cdf->index->nlat;
      counts[index+2]=cdf->index->nlon;
      offsets[index]=nband;
      offsets[index+1]=offsets[index+2]=0;
    }
    else
    {
      counts[index]=cdf->index->nlat;
      counts[index+1]=cdf->index->nlon;
      offsets[index]=offsets[index+1]=0;
    }
    rc=nc_put_vara_int(cdf->ncid,cdf->varid,offsets,counts,grid);
  }
  free(grid);
  if(rc!=NC_NOERR)
  {
    fprintf(stderr,"ERROR431: Cannot write output data: %s.\n",
            nc_strerror(rc));
    return TRUE;
  }
  return FALSE;
} /* of 'write_int_cdf' */

static void close_cdf(Cdf *cdf)
{
  nc_close(cdf->ncid);
  free(cdf);
} /* of 'close_cdf' */

#endif
int main(int argc,char **argv)
{
#ifdef USE_NETCDF
  FILE *file;
  Coordfile coordfile;
  Coord_array *index;
  Coord *grid,res;
  Cdf *cdf;
  Header header;
  String headername;
  float *data;
  Type type;
  size_t offset;
  Map *map=NULL;
  Attr *global_attrs=NULL;
  Attr *global_attrs2=NULL;
  int i,j,k,ngrid,version,iarg,compress,nbands,setversion;
  Bool swap,landuse,notime,isglobal,istype,israw,ismeta,isint,isnetcdf4;
  int n_global,n_global2;
  float *f,scale,cellsize_lon,cellsize_lat;
  int *idata,*iarr;
  char *units,*long_name,*endptr,*arglist,*missing_value;
  char *map_name,*pos;
  const char *progname;
  char *grid_filename,*path;
  Filename grid_name;
  char *filename,*outname,*variable;
  char *var_name=NULL;
  size_t filesize;
  char *var_units=NULL,*var_long_name=NULL,*var_standard_name=NULL;
  char *source=NULL,*history=NULL,*config_filename=NULL;
  Netcdf_config netcdf_config;
  units=long_name=NULL;
  scale=1.0;
  compress=0;
  cellsize_lon=cellsize_lat=0;
  istype=FALSE;
  landuse=FALSE;
  notime=FALSE;
  isglobal=FALSE;
  israw=FALSE;
  ismeta=FALSE;
  isint=FALSE;
  isnetcdf4=FALSE;
  nbands=1;
  setversion=READ_VERSION;
  map_name=MAP_NAME;
  grid_name.fmt=CLM;
  n_global=0;
  missing_value=NULL;
  type=LPJ_SHORT;
  progname=strippath(argv[0]);
  initsetting_netcdf(&netcdf_config);
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-h") || !strcmp(argv[iarg],"--help"))
      {
        printf("   clm2cdf (" __DATE__ ") Help\n"
               "   ==========================\n\n"
               "Convert CLM input data into NetCDF input data for LPJmL version %s\n",getversion());
        printf(USAGE
               "\nArguments:\n"
               "-h,--help        print this help text\n"
               "-v,--version     print LPJmL version\n"
               "-global          use global grid for NetCDF file\n"
               "-longheader      force version of CLM file to 2\n"
               "-scale s         set scaling factor for CLM version 1 files, default is 1\n"
               "-cellsize s      set cell size, default is 0.5\n"
               "-byte            set data type in CLM file to byte, default is short\n"
               "-int             set data type in CLM file to int, default is short\n"
               "-float           set data type in CLM file to float, default is short\n"
               "-intnetcdf       set datatype in NetCDF file to int, default is float\n"
               "-netcdf4         file written is in NetCDF4 format\n"
               "-metafile        set the input format to JSON metafile instead of CLM\n"
               "-map name        name of map in JSON metafile, default is \"map\"\n"
               "-raw             set the input format to raw instead of CLM\n"
               "-nbands n        number of bands for raw input, default is 1\n"
               "-landuse         convert land-use input data\n"
               "-notime          No time dimension in NetCDF file\n"
               "-compress l      set compression level for NetCDF4 files\n"
               "-attr name=value set global attribute name to value in NetCDF file\n"
               "-descr d         set long name in NetCDF file\n"
               "-units u         set units in NetCDF file\n"
               "-missing_value v set missing value to v\n"
               "-config file     read NetCDF setting from JSON file\n"
               "name             variable name in NetCDF file\n"
               "gridfile         filename of grid data file\n"
               "clmfile          filename of CLM data file\n"
               "netcdffile       filename of NetCDF file created\n\n"
               "(C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file\n",
               progname);
        return EXIT_SUCCESS;
      }
      else if(!strcmp(argv[iarg],"-v") || !strcmp(argv[iarg],"--version"))
      {
        puts(getversion());
        return EXIT_SUCCESS;
      }
      else if(!strcmp(argv[iarg],"-units"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-units'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        units=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-global"))
        isglobal=TRUE;
      else if(!strcmp(argv[iarg],"-longheader"))
        setversion=2;
      else if(!strcmp(argv[iarg],"-raw"))
        israw=TRUE;
      else if(!strcmp(argv[iarg],"-metafile"))
        ismeta=TRUE;
      else if(!strcmp(argv[iarg],"-netcdf4"))
        isnetcdf4=TRUE;
      else if(!strcmp(argv[iarg],"-int"))
      {
        istype=TRUE;
        type=LPJ_INT;
      }
      else if(!strcmp(argv[iarg],"-intnetcdf"))
      {
        isint=TRUE;
        type=LPJ_INT;
      }
      else if(!strcmp(argv[iarg],"-float"))
      {
        istype=TRUE;
        type=LPJ_FLOAT;
      }
      else if(!strcmp(argv[iarg],"-byte"))
      {
        istype=TRUE;
        type=LPJ_BYTE;
      }
      else if(!strcmp(argv[iarg],"-notime"))
        notime=TRUE;
      else if(!strcmp(argv[iarg],"-landuse"))
        landuse=TRUE;
      else if(!strcmp(argv[iarg],"-descr"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-descr'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        long_name=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-missing_value"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-missing_value'.\n"
                  ERR_USAGE,progname,progname);
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
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        pos=strchr(argv[++iarg],'=');
        if(pos==NULL)
        {
          fprintf(stderr,
                  "Error: Missing '=' for '-attr' option.\n"
                  ERR_USAGE,progname,progname);
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
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-map'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        map_name=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-config"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-config'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        config_filename=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-scale"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-scale'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        scale=(float)strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-scale'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-cellsize"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-cellsize'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        cellsize_lon=cellsize_lat=(float)strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-cellsize'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
        if(cellsize_lon<=0)
        {
          fprintf(stderr,"Cell size=%g must be greater than zero.\n",cellsize_lon);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-nbands"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-nbands'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        nbands=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-nbands'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
        if(nbands<1)
        {
          fprintf(stderr,"Error: Invalid number %d of bands, must be >0.\n",nbands);
          return EXIT_FAILURE;
        }

      }
      else if(!strcmp(argv[iarg],"-compress"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-compress'.\n"
                  ERR_USAGE,progname,progname);
          return EXIT_FAILURE;
        }
        compress=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-compress'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
        if(compress<0 || compress>9)
        {
          fprintf(stderr,"Error: Invalid compression value %d, must be in [0,9].\n",
                  compress);
          return EXIT_FAILURE;
        }
      }
      else
      {
        fprintf(stderr,"Error: Invalid option '%s'.\n"
                  ERR_USAGE,argv[iarg],progname,progname);
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
    fprintf(stderr,"Error: Missing arguments.\n"
            ERR_USAGE,progname,progname);
    return EXIT_FAILURE;
  }
  else
  {
    filename=argv[iarg+2];
    outname=argv[iarg+3];
  }
  if(config_filename!=NULL)
  {
    if(parse_config_netcdf(&netcdf_config,config_filename))
    {
      fprintf(stderr,"Error reading NetCDF configuration file `%s`.\n",config_filename);
      return EXIT_FAILURE;
    }
  }
  if(ismeta)
  {
    header.cellsize_lon=header.cellsize_lat=0.5;
    header.firstyear=1901;
    header.firstcell=0;
    header.nyear=1;
    header.nbands=nbands;
    header.nstep=1;
    header.datatype=type;
    header.order=CELLYEAR;

    file=openmetafile(&header,&map,map_name,&global_attrs2,&n_global2,&source,&history,&var_name,&var_units,&var_standard_name,&var_long_name,&grid_name,NULL,NULL,&swap,&offset,filename,TRUE);
    if(file==NULL)
      return EXIT_FAILURE;
    if(fseek(file,offset,SEEK_CUR))
    {
      fprintf(stderr,"Error seeking in '%s' to offset %lu.\n",filename,offset);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(units==NULL && var_units!=NULL)
      units=var_units;
    if(long_name==NULL && var_long_name==NULL)
      long_name=var_long_name;
    if(global_attrs2!=NULL)
    {
      mergeattrs(&global_attrs,&n_global,global_attrs2,n_global2,FALSE);
      freeattrs(global_attrs2,n_global2);
    }
  }
  else
  {
    file=fopen(filename,"rb");
    if(file==NULL)
    {
      fprintf(stderr,"Error opening '%s': %s.\n",filename,strerror(errno));
      return EXIT_FAILURE;
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
  /* Open and read grid file */
  grid_name.name=grid_filename;
  coordfile=opencoord(&grid_name,TRUE);
  if(coordfile==NULL)
    return EXIT_FAILURE;
  ngrid=numcoord(coordfile);
  if(cellsize_lon>0)
    res.lon=res.lat=cellsize_lon;
  else
  {
    getcellsizecoord(&cellsize_lon,&cellsize_lat,coordfile);
    res.lon=cellsize_lon;
    res.lat=cellsize_lat;
  }
  grid=newvec(Coord,numcoord(coordfile));
  if(grid==NULL)
  {
    printallocerr("grid");
    return EXIT_FAILURE;
  }
  for(i=0;i<numcoord(coordfile);i++)
    readcoord(coordfile,grid+i,&res);
  closecoord(coordfile);
  if(!israw)
  {
    if(ismeta)
    {
      version=4;
      if(header.nstep==1 && header.nbands>1)
        landuse=TRUE;
      else if(header.nstep>1)
      {
        if(header.nbands>1)
        {
          fprintf(stderr,"Error: Number of bands %d and number of steps %d >1 in '%s' not supported.\n",
                  header.nbands,header.nstep,filename);
          fclose(file);
          return EXIT_FAILURE;
        }
        header.nbands=header.nstep;
      }
    }
    else
    {
    version=setversion;
    if(freadanyheader(file,&header,&swap,headername,&version,TRUE))
    {
      fprintf(stderr,"Error reading header of '%s'.\n",filename);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(version>CLM_MAX_VERSION)
    {
      fprintf(stderr,"Error: Unsupported version %d in '%s', must be less than %d.\n",
              version,filename,CLM_MAX_VERSION+1);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(version==1)
      header.scalar=scale;
    if(version<3)
      header.datatype=(istype)  ? type  : LPJ_SHORT;
    filesize=getfilesizep(file)-headersize(headername,version);
    if(version==4)
    {
      if(header.nstep==1 && header.nbands>1)
        landuse=TRUE;
      else if(header.nstep>1)
      {
        if(header.nbands>1)
        {
          fprintf(stderr,"Error: Number of bands %d and number of steps %d >1 in '%s' not supported.\n",
                  header.nbands,header.nstep,filename);
          fclose(file);
          return EXIT_FAILURE;
        }
        header.nbands=header.nstep;
      }
    }
    if(filesize!=(long long)header.nyear*header.ncell*header.nbands*typesizes[header.datatype])
      fprintf(stderr,"Warning: File size of '%s' does not match nbands*ncell*nyear.\n",filename);
    }
    if(header.order!=CELLYEAR)
    {
      fprintf(stderr,"Error: Order in '%s' must be cellyear, order ",filename);
      if(header.order>0 || header.order<=CELLSEQ)
        fprintf(stderr,"%s",ordernames[header.order-1]);
      else
        fprintf(stderr,"%d",header.order);
      fprintf(stderr," is not supported.\n.");
    }
    if(notime && (header.nyear>1 || (!landuse && header.nbands>1)))
    {
      fprintf(stderr,"No time axis set, but number of time steps>1 in '%s'.\n",
              filename);
      fclose(file);
      return EXIT_FAILURE;
    }

    if(header.ncell!=ngrid)
    {
      fprintf(stderr,"Number of cells=%d in '%s' is different from %d in '%s'.\n",
              header.ncell,filename,ngrid,grid_filename);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(version==1 && cellsize_lon>0)
    {
      header.cellsize_lon=cellsize_lon;
      header.cellsize_lat=cellsize_lat;
    }
    if(fabs(header.cellsize_lon-res.lon)>min(res.lon,header.cellsize_lon)*0.5)
    {
      fprintf(stderr,"Longitudinal cell size=%g in '%s' differs from %g in '%s'.\n",
              header.cellsize_lon,filename,res.lon,grid_filename);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(fabs(header.cellsize_lat-res.lat)>min(res.lat,header.cellsize_lat)*0.5)
    {
      fprintf(stderr,"Latitudinal cell size=%g in '%s' differs from %g in '%s'.\n",
              header.cellsize_lat,filename,res.lat,grid_filename);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(ismeta)
    {
      header.cellsize_lon=(float)res.lon;
      header.cellsize_lat=(float)res.lat;
    }
  }
  else
  {
    header.cellsize_lon=(float)res.lon;
    header.cellsize_lat=(float)res.lat;
    header.datatype=(istype)  ? type  : LPJ_SHORT;
    header.ncell=ngrid;
    header.nbands=nbands;
    header.scalar=scale;
    header.nyear=getfilesizep(file)/typesizes[type]/ngrid/header.nbands;
    if(getfilesizep(file) % (typesizes[type]*ngrid*header.nbands))
      fprintf(stderr,"Warning: file size of '%s' is not multiple of bands %d and number of cells %d.\n",filename,header.nbands,ngrid);
  }
  index=createindex(grid,ngrid,res,isglobal,FALSE);
  if(index==NULL)
    return EXIT_FAILURE;
  free(grid);
  arglist=catstrvec(argv,argc);
  if(missing_value!=NULL)
  {
    if(isint)
    {
      netcdf_config.missing_value.i=strtol(missing_value,&endptr,10);
      if(*endptr!='\0')
      {
        fprintf(stderr,"Invalid number '%s' for missing value.\n",missing_value);
        return EXIT_FAILURE;
      }
    }
    else
    {
      netcdf_config.missing_value.f=strtod(missing_value,&endptr);
      if(*endptr!='\0')
      {
        fprintf(stderr,"Invalid number '%s' for missing value.\n",missing_value);
        return EXIT_FAILURE;
      }
    }
  }
  cdf=create_cdf(outname,map,source,history,variable,units,var_standard_name,long_name,&netcdf_config,arglist,global_attrs,n_global,&header,compress,landuse,notime,isint || ((header.datatype==LPJ_INT || header.datatype==LPJ_BYTE) && header.scalar==1),isnetcdf4,index);
  free(arglist);
  if(cdf==NULL)
    return EXIT_FAILURE;
  if((isint ||((header.datatype==LPJ_INT || header.datatype==LPJ_BYTE) && header.scalar==1)))
  {
    idata=newvec(int,ngrid*header.nbands);
    if(idata==NULL)
    {
      printallocerr("data");
      return EXIT_FAILURE;
    }
    iarr=newvec(int,ngrid);
    if(iarr==NULL)
    {
      printallocerr("iarr");
      return EXIT_FAILURE;
    }
    for(i=0;i<header.nyear;i++)
    {
      if(readintvec(file,idata,ngrid*header.nbands,swap,header.datatype))
      {
        fprintf(stderr,"Error reading data in year %d.\n",i+header.firstyear);
        close_cdf(cdf);
        return EXIT_FAILURE;
      }
      for(j=0;j<header.nbands;j++)
      {
        for(k=0;k<ngrid;k++)
          iarr[k]=idata[k*header.nbands+j];
        if(write_int_cdf(cdf,iarr,(landuse) ? i : i*header.nbands+j,ngrid,landuse,notime,j,netcdf_config.missing_value.i))
          return EXIT_FAILURE;
      }
    }
    free(idata);
    free(iarr);
  }
  else
  {
    data=newvec(float,ngrid*header.nbands);
    if(data==NULL)
    {
      printallocerr("data");
      return EXIT_FAILURE;
    }
    f=newvec(float,ngrid);
    if(f==NULL)
    {
      printallocerr("f");
      return EXIT_FAILURE;
    }
    for(i=0;i<header.nyear;i++)
    {
      if(readfloatvec(file,data,header.scalar,ngrid*header.nbands,swap,header.datatype))
      {
        fprintf(stderr,"Error reading data in year %d.\n",i+header.firstyear);
        close_cdf(cdf);
        return EXIT_FAILURE;
      }
      for(j=0;j<header.nbands;j++)
      {
        for(k=0;k<ngrid;k++)
          f[k]=data[k*header.nbands+j];
        if(write_float_cdf(cdf,f,(landuse) ? i : i*header.nbands+j,ngrid,landuse,notime,j,netcdf_config.missing_value.f))
          return EXIT_FAILURE;
      }
    }
    free(data);
    free(f);
  }
  close_cdf(cdf);
  fclose(file);

  return EXIT_SUCCESS;
#else
  fprintf(stderr,"ERROR401: NetCDF is not supported in this version of %s.\n",argv[0]);
  return EXIT_FAILURE;
#endif
} /* of 'main' */

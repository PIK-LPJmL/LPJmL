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

#if defined(USE_NETCDF) || defined(USE_NETCDF4)
#include <netcdf.h>

#define error(rc) if(rc) {free(lon);free(lat);free(year);fprintf(stderr,"ERROR427: Cannot write '%s': %s.\n",filename,nc_strerror(rc)); nc_close(cdf->ncid); free(cdf);return NULL;}

#define MISSING_VALUE -9999.99
#define USAGE "Usage: %s [-scale s] [-longheader] [-global] [-cellsize size] [-byte] [-int] [-float]\n       [[-attr name=value] ...] [-intnetcdf] [-metafile] [-raw] [-nbands n] [-landuse] [-notime] [-compress level] [-units u]\n       [-map name] [-descr d] name gridfile clmfile netcdffile\n"

typedef struct
{
  const Coord_array *index;
  int ncid;
  int varid;
} Cdf;

static Cdf *create_cdf(const char *filename,
                       List *map,
                       const char *name,
                       const char *units,
                       const char *descr,
                       const char *args,
                       const Attr *global_attrs,
                       int n_global,
                       const Header *header,
                       int compress,
                       Bool landuse,
                       Bool notime,
                       Bool isint,
                       const Coord_array *array)
{
  Cdf *cdf;
  float *lon,*lat,miss=MISSING_VALUE;
  int *year,i,j,rc,dim[4],imiss=MISSING_VALUE_INT,varid;
  String s;
  time_t t;
  size_t offset[2],count[2];
  int time_var_id,lat_var_id,lon_var_id,time_dim_id,lat_dim_id,lon_dim_id,map_dim_id,len_dim_id;
  int landuse_dim_id;
  int index;
  int len;
  cdf=new(Cdf);
  lon=newvec(float,array->nlon);
  if(lon==NULL)
  {
    printallocerr("lon");
    free(cdf);
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
  for(i=0;i<array->nlon;i++)
    lon[i]=(float)(array->lon_min+i*header->cellsize_lon);
  for(i=0;i<array->nlat;i++)
    lat[i]=(float)(array->lat_min+i*header->cellsize_lat);
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
        year[i]=header->firstyear+i;
    else switch(header->nbands)
    {
      case 1:
        for(i=0;i<header->nyear;i++)
          year[i]=header->firstyear+i;
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
#ifdef USE_NETCDF4
  rc=nc_create(filename,(compress) ? NC_CLOBBER|NC_NETCDF4 : NC_CLOBBER,&cdf->ncid);
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
  if(!notime)
  {
    rc=nc_def_dim(cdf->ncid,TIME_DIM_NAME,(landuse) ? header->nyear : header->nyear*header->nbands,&time_dim_id);
    error(rc);
    rc=nc_def_var(cdf->ncid,"time",NC_INT,1,&time_dim_id,&time_var_id);
    error(rc);
  }
  rc=nc_def_dim(cdf->ncid,LAT_DIM_NAME,array->nlat,&lat_dim_id);
  error(rc);
  rc=nc_def_dim(cdf->ncid,LON_DIM_NAME,array->nlon,&lon_dim_id);
  error(rc);
  rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"source",strlen(args),args);
  error(rc);
  time(&t);
  snprintf(s,STRING_LEN,"Created for user %s on %s at %s",getuser(),gethost(),
           strdate(&t));
  rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"history",strlen(s),s);
  error(rc);
  for(i=0;i<n_global;i++)
  {
    rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,global_attrs[i].name,strlen(global_attrs[i].value),global_attrs[i].value);
    error(rc);
  }
  rc=nc_def_var(cdf->ncid,LAT_NAME,NC_FLOAT,1,&lat_dim_id,&lat_var_id);
  error(rc);
  rc=nc_def_var(cdf->ncid,LON_NAME,NC_FLOAT,1,&lon_dim_id,&lon_var_id);
  error(rc);
  if(!notime)
  {
    if(landuse || header->nbands==1)
      rc=nc_put_att_text(cdf->ncid,time_var_id,"units",strlen("years"),"years");
    else if(header->nbands>1)
    {
      snprintf(s,STRING_LEN,"days since %d-1-1 0:0:0",header->firstyear);
      rc=nc_put_att_text(cdf->ncid,time_var_id,"units",strlen(s),s);
      error(rc);
      rc=nc_put_att_text(cdf->ncid,time_var_id,"calendar",strlen("noleap"),
                         "noleap");
    }
    error(rc);
  }
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
  if(map!=NULL)
  {
    len=0;
    for(i=0;i<getlistlen(map);i++)
      if(getlistitem(map,i)==NULL)
        len=max(len,strlen(NULL_NAME));
      else
        len=max(len,strlen(getlistitem(map,i)));
    rc=nc_def_dim(cdf->ncid,"len",len+1,&len_dim_id);
    error(rc);
    rc=nc_def_dim(cdf->ncid,MAP_NAME,getlistlen(map),&map_dim_id);
    error(rc);
    dim[0]=map_dim_id;
    dim[1]=len_dim_id;
    rc=nc_def_var(cdf->ncid,MAP_NAME,NC_CHAR,2,dim,&varid);
    error(rc);
  }
  if(notime)
    index=0;
  else
  {
    dim[0]=time_dim_id;
    index=1;
  }
  if(landuse)
  {
    rc=nc_def_dim(cdf->ncid,"pft",header->nbands,&landuse_dim_id);
    error(rc);
    dim[index]=landuse_dim_id;
    dim[index+1]=lat_dim_id;
    dim[index+2]=lon_dim_id;
  }
  else
  {
    dim[index]=lat_dim_id;
    dim[index+1]=lon_dim_id;
  }
  if(notime)
    rc=nc_def_var(cdf->ncid,name,(isint) ? NC_INT : NC_FLOAT,(landuse) ? 3 : 2,dim,&cdf->varid);
  else
    rc=nc_def_var(cdf->ncid,name,(isint) ? NC_INT : NC_FLOAT,(landuse) ? 4 : 3,dim,&cdf->varid);
  error(rc);
#ifdef USE_NETCDF4
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
  if(descr!=NULL)
  {
    rc=nc_put_att_text(cdf->ncid, cdf->varid,"long_name",strlen(descr),descr);
    error(rc);
  }
  if(isint)
  {
    nc_put_att_int(cdf->ncid, cdf->varid,"missing_value",NC_INT,1,&imiss);
    rc=nc_put_att_int(cdf->ncid, cdf->varid,"_FillValue",NC_INT,1,&imiss);
  }
  else
  {
    nc_put_att_float(cdf->ncid, cdf->varid,"missing_value",NC_FLOAT,1,&miss);
    rc=nc_put_att_float(cdf->ncid, cdf->varid,"_FillValue",NC_FLOAT,1,&miss);
  }
  rc=nc_enddef(cdf->ncid);
  error(rc);
  if(!notime)
  {
    rc=nc_put_var_int(cdf->ncid,time_var_id,year);
    error(rc);
    free(year);
  }
  rc=nc_put_var_float(cdf->ncid,lat_var_id,lat);
  error(rc);
  rc=nc_put_var_float(cdf->ncid,lon_var_id,lon);
  error(rc);
  if(map!=NULL)
  {
    offset[1]=0;
    count[0]=1;
    for(i=0;i<getlistlen(map);i++)
    {
      offset[0]=i;
      if(getlistitem(map,i)==NULL)
      {
        count[1]=strlen(NULL_NAME)+1;
        rc=nc_put_vara_text(cdf->ncid,varid,offset,count,NULL_NAME);
      }
      else
      {
        count[1]=strlen(getlistitem(map,i))+1;
        rc=nc_put_vara_text(cdf->ncid,varid,offset,count,getlistitem(map,i));
      }
      error(rc);
    }
  }
  free(lat);
  free(lon);
  return cdf;
} /* of 'create_cdf' */

static Bool write_float_cdf(const Cdf *cdf,const float vec[],int year,
                            int size,Bool landuse,Bool notime,int nband)
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
    grid[i]=MISSING_VALUE;
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
                          int size,Bool landuse,Bool notime,int nband)
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
    grid[i]=MISSING_VALUE_INT;
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
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
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
  List *map=NULL;
  Attr *global_attrs=NULL;
  int i,j,k,ngrid,version,iarg,compress,nbands,setversion;
  Bool swap,landuse,notime,isglobal,istype,israw,ismeta,isint,n_global;
  float *f,scale,cellsize_lon,cellsize_lat;
  int *idata,*iarr;
  char *units,*descr,*endptr,*arglist;
  char *map_name,*pos;
  const char *progname;
  Filename filename;
  size_t filesize;
  units=descr=NULL;
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
  nbands=1;
  setversion=READ_VERSION;
  map_name=MAP_NAME;
  n_global=0;
  progname=strippath(argv[0]);
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-units"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-units'.\n"
                 USAGE,progname);
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
                 USAGE,progname);
          return EXIT_FAILURE;
        }
        descr=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-attr"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,
                  "Error: Argument missing for '-attr' option.\n"
                 USAGE,progname);
          return EXIT_FAILURE;
        }
        pos=strchr(argv[++iarg],'=');
        if(pos==NULL)
        {
          fprintf(stderr,
                  "Error: Missing '=' for '-attr' option.\n"
                 USAGE,progname);
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
                 USAGE,progname);
          return EXIT_FAILURE;
        }
        map_name=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-scale"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-scale'.\n"
                  USAGE,progname);
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
                  USAGE,progname);
          return EXIT_FAILURE;
        }
        cellsize_lon=cellsize_lat=(float)strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-cellsize'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-nbands"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-nbands'.\n"
                  USAGE,progname);
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
                  USAGE,progname);
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
                USAGE,argv[iarg],progname);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<iarg+4)
  {
    fprintf(stderr,"Error: Missing arguments.\n"
            USAGE,progname);
    return EXIT_FAILURE;
  }
  filename.fmt=CLM;
  filename.name=argv[iarg+1];
  coordfile=opencoord(&filename,TRUE);
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

    file=openmetafile(&header,&map,map_name,&swap,&offset,argv[iarg+2],TRUE);
    if(file==NULL)
      return EXIT_FAILURE;
    if(fseek(file,offset,SEEK_CUR))
    {
      fprintf(stderr,"Error seeking in '%s' to offset %lu.\n",argv[iarg+2],offset);
      fclose(file);
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
  }
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
                  header.nbands,header.nstep,argv[iarg+2]);
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
      fprintf(stderr,"Error reading header of '%s'.\n",argv[iarg+2]);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(version>CLM_MAX_VERSION)
    {
      fprintf(stderr,"Error: Unsupported version %d in '%s', must be less than %d.\n",
              version,argv[iarg+2],CLM_MAX_VERSION+1);
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
                  header.nbands,header.nstep,argv[iarg+2]);
          fclose(file);
          return EXIT_FAILURE;
        }
        header.nbands=header.nstep;
      }
    }
    if(filesize!=(long long)header.nyear*header.ncell*header.nbands*typesizes[header.datatype])
      fprintf(stderr,"Warning: File size of '%s' does not match nbands*ncell*nyear.\n",argv[iarg+2]);
    }
    if(header.order!=CELLYEAR)
    {
      fprintf(stderr,"Error: Order in '%s' must be cellyear, order ",argv[iarg+2]);
      if(header.order>0 || header.order<=CELLSEQ)
        fprintf(stderr,"%s",ordernames[header.order-1]);
      else
        fprintf(stderr,"%d",header.order);
      fprintf(stderr," is not supported.\n.");
    }
    if(notime && (header.nyear>1 || (!landuse && header.nbands>1)))
    {
      fprintf(stderr,"No time axis set, but number of time steps>1 in '%s'.\n",
              argv[iarg+2]);
      fclose(file);
      return EXIT_FAILURE;
    }

    if(header.ncell!=ngrid)
    {
      fprintf(stderr,"Number of cells in '%s' is different from %d in '%s'.\n",
              argv[iarg+2],ngrid,argv[iarg+1]);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(version==1 && cellsize_lon>0)
    {
      header.cellsize_lon=cellsize_lon;
      header.cellsize_lat=cellsize_lat;
    }
    if(header.cellsize_lon!=res.lon)
    {
      fprintf(stderr,"Cell size in '%s' differs from '%s'.\n",
              argv[iarg+2],argv[iarg+1]);
      fclose(file);
      return EXIT_FAILURE;
    }
    if(header.cellsize_lat!=res.lat)
    {
      fprintf(stderr,"Cell size in '%s' differs from '%s'.\n",
              argv[iarg+2],argv[iarg+1]);
      fclose(file);
      return EXIT_FAILURE;
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
      fprintf(stderr,"Warning: file size of '%s' is not multiple of bands %d and number of cells %d.\n",argv[iarg+2],header.nbands,ngrid);
  }
  index=createindex(grid,ngrid,res,isglobal);
  if(index==NULL)
    return EXIT_FAILURE;
  free(grid);
  arglist=catstrvec(argv,argc);
  cdf=create_cdf(argv[iarg+3],map,argv[iarg],units,descr,arglist,global_attrs,n_global,&header,compress,landuse,notime,isint || ((header.datatype==LPJ_INT || header.datatype==LPJ_BYTE) && header.scalar==1),index);
  free(arglist);
  if(cdf==NULL)
    return EXIT_FAILURE;
  if((isint ||(header.datatype==LPJ_INT || header.datatype==LPJ_BYTE) && header.scalar==1))
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
        return EXIT_FAILURE;
      }
      for(j=0;j<header.nbands;j++)
      {
        for(k=0;k<ngrid;k++)
          iarr[k]=idata[k*header.nbands+j];
        if(write_int_cdf(cdf,iarr,(landuse) ? i : i*header.nbands+j,ngrid,landuse,notime,j))
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
        return EXIT_FAILURE;
      }
      for(j=0;j<header.nbands;j++)
      {
        for(k=0;k<ngrid;k++)
          f[k]=data[k*header.nbands+j];
        if(write_float_cdf(cdf,f,(landuse) ? i : i*header.nbands+j,ngrid,landuse,notime,j))
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

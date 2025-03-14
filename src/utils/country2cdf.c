/**************************************************************************************/
/**                                                                                \n**/
/**                c  o  u  n  t  r  y  2  c  d  f  .  c                           \n**/
/**                                                                                \n**/
/**     Converts country/region code data file into NetCDF data file               \n**/
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

#define error(rc) if(rc) {free(lon);free(lat);fprintf(stderr,"ERROR427: Cannot write '%s': %s.\n",filename,nc_strerror(rc)); nc_close(cdf->ncid); free(cdf);return NULL;}

#define USAGE "Usage: %s [-global] [-index i] [-cellsize size] [-compress level] [-descr d] [-netcdf4] [-config file]\n       name gridfile countryfile netcdffile\n"

typedef struct
{
  const Coord_array *index;
  int ncid;
  int varid;
} Cdf;

static Cdf *create_cdf(const char *filename,
                       const char *progname,
                       const char *arglist,
                       const char *name,
                       const char *descr,
                       const Netcdf_config *netcdf_config,
                       Coord res,
                       int compress,
                       Bool isnetcdf4,
                       const Coord_array *array)
{
  Cdf *cdf;
  double *lon,*lat;
  int i,rc,dim[2];
  char *s;
  time_t t;
  int lat_var_id,lon_var_id,lat_dim_id,lon_dim_id;
  cdf=new(Cdf);
  lon=newvec(double,array->nlon);
  if(lon==NULL)
  {
    free(cdf);
    printallocerr("lon");
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
  cdf->index=array;
  for(i=0;i<array->nlon;i++)
    lon[i]=array->lon_min+i*res.lon;
  for(i=0;i<array->nlat;i++)
    lat[i]=array->lat_min+i*res.lat;

  if(isnetcdf4)
    rc=nc_create(filename,(compress) ? NC_CLOBBER|NC_NETCDF4 : NC_CLOBBER,&cdf->ncid);
  else
    rc=nc_create(filename,NC_CLOBBER,&cdf->ncid);
  if(rc)
  {
    fprintf(stderr,"ERROR426: Cannot create file '%s': %s.\n",
            filename,nc_strerror(rc));
    free(lon);
    free(lat);
    free(cdf);
    return NULL;
  }
  rc=nc_def_dim(cdf->ncid,netcdf_config->lat.dim,array->nlat,&lat_dim_id);
  error(rc);
  rc=nc_def_dim(cdf->ncid,netcdf_config->lon.dim,array->nlon,&lon_dim_id);
  error(rc);
  time(&t);
  s=getsprintf("%s: %s",strdate(&t),arglist);
  rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"history",strlen(s),s);
  free(s);
  error(rc);
  rc=nc_put_att_text(cdf->ncid,NC_GLOBAL,"source",strlen(progname),progname);
  error(rc);
  rc=nc_def_var(cdf->ncid,netcdf_config->lat.name,NC_DOUBLE,1,&lat_dim_id,&lat_var_id);
  error(rc);
  rc=nc_def_var(cdf->ncid,netcdf_config->lon.name,NC_DOUBLE,1,&lon_dim_id,&lon_var_id);
  error(rc);
  rc=nc_put_att_text(cdf->ncid,lon_var_id,"units",
                     strlen(netcdf_config->lon.unit),netcdf_config->lon.unit);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"long_name",
                     strlen(netcdf_config->lon.long_name),netcdf_config->lon.long_name);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"standard_name",
                     strlen(netcdf_config->lon.standard_name),netcdf_config->lon.standard_name);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lon_var_id,"axis",strlen("X"),"X");
  error(rc);
  rc=nc_put_att_text(cdf->ncid,lat_var_id,"units",
                     strlen(netcdf_config->lat.unit),netcdf_config->lat.unit);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"long_name",
                     strlen(netcdf_config->lat.long_name),netcdf_config->lat.long_name);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"standard_name",
                     strlen(netcdf_config->lat.standard_name),netcdf_config->lat.standard_name);
  error(rc);
  rc=nc_put_att_text(cdf->ncid, lat_var_id,"axis",strlen("Y"),"Y");
  error(rc);
  dim[0]=lat_dim_id;
  dim[1]=lon_dim_id;
  rc=nc_def_var(cdf->ncid,name,NC_SHORT,2,dim,&cdf->varid);
  error(rc);
  if(isnetcdf4 && compress)
  {
    rc=nc_def_var_deflate(cdf->ncid, cdf->varid, 0, 1,compress);
    error(rc);
  }
  if(descr!=NULL)
  {
    rc=nc_put_att_text(cdf->ncid, cdf->varid,"long_name",strlen(descr),descr);
    error(rc);
  }
  nc_put_att_short(cdf->ncid, cdf->varid,"missing_value",NC_SHORT,1,&netcdf_config->missing_value.s);
  rc=nc_put_att_short(cdf->ncid, cdf->varid,"_FillValue",NC_SHORT,1,&netcdf_config->missing_value.s);
  rc=nc_enddef(cdf->ncid);
  error(rc);
  rc=nc_put_var_double(cdf->ncid,lat_var_id,lat);
  error(rc);
  rc=nc_put_var_double(cdf->ncid,lon_var_id,lon);
  error(rc);
  free(lat);
  free(lon);
  return cdf;
} /* of 'create_cdf' */

static Bool write_short_cdf(const Cdf *cdf,const short vec[],int size,short smiss)
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
    grid[i]=smiss;
  for(i=0;i<size;i++)
    grid[cdf->index->index[i]]=vec[i];
  rc=nc_put_var_short(cdf->ncid,cdf->varid,grid);
  free(grid);
  if(rc!=NC_NOERR)
  {
    fprintf(stderr,"ERROR428: Cannot write output data: %s.\n",
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
#ifdef USE_NETCDF
  FILE *file;
  Coordfile coordfile;
  Coord_array *index;
  Coord *grid,res;
  Cdf *cdf;
  short *data;
  String headername;
  int i,ngrid,iarg,compress,inum,version;
  Header header;
  short *f;
  float cellsize,lon,lat;
  Bool swap,isglobal,isnetcdf4;
  char *descr,*endptr,*arglist;
  char  *config_filename;
  Netcdf_config netcdf_config;
  Filename filename;
  descr=NULL;
  compress=0;
  inum=0;
  cellsize=0;
  isglobal=FALSE;
  isnetcdf4=FALSE;
  config_filename=NULL;
  initsetting_netcdf(&netcdf_config);
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
      else if(!strcmp(argv[iarg],"-netcdf4"))
        isnetcdf4=TRUE;
      else if(!strcmp(argv[iarg],"-cellsize"))
      {
        if(argc==iarg+1)
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
      }
      else if(!strcmp(argv[iarg],"-index"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-descr'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        inum=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-index'.\n",
                  argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-compress"))
      {
        if(argc==iarg+1)
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
        if(compress<0 || compress>9)
        {
          fprintf(stderr,"Error: Invalid compression value %d, must be in [0,9].\n",
                  compress);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-config"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-config'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        config_filename=argv[++iarg];
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
  if(argc<iarg+4)
  {
    fprintf(stderr,"Error: Missing arguments.\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  if(config_filename!=NULL)
  {
    if(parse_config_netcdf(&netcdf_config,config_filename))
    {
      fprintf(stderr,"Error reading NetCDF configuration file `%s`.\n",config_filename);
      return EXIT_FAILURE;
    }
  }
  filename.fmt=CLM;
  filename.name=argv[iarg+1];
  coordfile=opencoord(&filename,TRUE);
  if(coordfile==NULL)
  {
    fprintf(stderr,"Error opening grid file '%s'.\n",filename.name);
    return EXIT_FAILURE;
  }
  ngrid=numcoord(coordfile);
  if(cellsize>0)
    res.lon=res.lat=cellsize;
  else
  {
    getcellsizecoord(&lon,&lat,coordfile);
    res.lon=lon;
    res.lat=lat;
  }
  grid=newvec(Coord,numcoord(coordfile));
  for(i=0;i<numcoord(coordfile);i++)
    readcoord(coordfile,grid+i,&res);
  closecoord(coordfile);
  file=fopen(argv[iarg+2],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[iarg+2],strerror(errno));
    return EXIT_FAILURE;
  }
  version=READ_VERSION;
  if(freadanyheader(file,&header,&swap,headername,&version,TRUE))
  {
    fprintf(stderr,"Error reading header of '%s'.\n",argv[iarg+2]);
    fclose(file);
    return EXIT_FAILURE;
  }
  if(header.ncell!=ngrid)
  {
    fprintf(stderr,"Error: Number of cells in '%s' is different from %d in '%s'.\n",
            argv[iarg+2],ngrid,argv[iarg+1]);
    fclose(file);
    return EXIT_FAILURE;
  }
  if(inum<0 || inum>=header.nbands)
  {
    fprintf(stderr,"Error: Invalid value of index %d, must be 0<index<%d.\n",inum,header.nbands-1);
    fclose(file);
    return EXIT_FAILURE;
  }
  index=createindex(grid,ngrid,res,isglobal,FALSE);
  if(index==NULL)
    return EXIT_FAILURE;
  free(grid);
  if(version==1 && cellsize>0)
    header.cellsize_lon=header.cellsize_lat=cellsize;
  else if(version>1 && header.cellsize_lon!=res.lon)
  {
    fprintf(stderr,"Error: Cell size in '%s' differs from '%s'.\n",
            argv[iarg+2],argv[iarg+1]);
    fclose(file);
    return EXIT_FAILURE;
  }
  else if(version>2 && header.cellsize_lat!=res.lat)
  {
    fprintf(stderr,"Error: Cell size in '%s' differs from '%s'.\n",
            argv[iarg+2],argv[iarg+1]);
    fclose(file);
    return EXIT_FAILURE;
  }
  else if(version>2 && header.datatype!=LPJ_SHORT)
  {
    fprintf(stderr,"Error: Datatype in '%s' of %s must be short.\n",
            argv[iarg+2],typenames[header.datatype]);
    fclose(file);
    return EXIT_FAILURE;
  }
  arglist=catstrvec(argv,argc);
  cdf=create_cdf(argv[iarg+3],argv[0],arglist,argv[iarg],descr,&netcdf_config,res,compress,isnetcdf4,index);
  if(cdf==NULL)
    return EXIT_FAILURE;
  data=newvec(short,ngrid*header.nbands);
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
  if(freadshort(data,ngrid*header.nbands,swap,file)!=ngrid*header.nbands)
  {
    fprintf(stderr,"Error reading country data.\n");
    return EXIT_FAILURE;
  }
  for(i=0;i<ngrid;i++)
    f[i]=data[header.nbands*i+inum];
  write_short_cdf(cdf,f,ngrid,netcdf_config.missing_value.s);
  close_cdf(cdf);
  fclose(file);
  free(data);
  return EXIT_SUCCESS;
#else
  fputs("NetCDF is not supported in this version of LPJmL.\n",stderr);
  return EXIT_FAILURE;
#endif
} /* of 'main' */

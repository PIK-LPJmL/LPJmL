/**************************************************************************************/
/**                                                                                \n**/
/**                d  r  a  i  n  a  g  e  2  c  d  f  .  c                        \n**/
/**                                                                                \n**/
/**     Converts drainage clm file into NetCDF data file                           \n**/
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
#endif
#define error(rc) if(rc) {fprintf(stderr,"ERROR427: Cannot write '%s': %s.\n",argv[iarg+3],nc_strerror(rc)); nc_close(ncid);return EXIT_FAILURE;}

#define USAGE "Usage: %s [-var name] [-config file] [-compress level] [-netcdf4] soilcode.nc grid.clm drainage.clm drainage.nc\n"
#define INDEX_LONG_NAME "index of cell into/from which water flows (ilat*nlon+ilon)"

int main(int argc,char **argv)
{
#ifdef USE_NETCDF
  char *var;
  var=NULL;
  const double *lon,*lat;
  int *index;
  Coord *grid_soil,*grid;
  FILE *file;
  Coordfile gridfile;
  Coord_netcdf soil;
  Coord resolution;
  Filename name;
  Header header;
  Bool swap,isnetcdf4;
  Netcdf_config nc_config;
  int iarg,i,n,nlon,nlat,j,offset[2]={0,0};
  int ncid,rc,lat_dim_id,lon_dim_id,lon_var_id,lat_var_id;
  int dim[2];
  int index_varid,len_varid;
  int data[2];
  int version,compress;
  int src_cell,dst_cell;
  int *out;
  char *s,*endptr;
  char *config_filename;
  time_t t;
  char *cmdline;
  float *len=NULL,glon,glat;
  unsigned int soilcode;
  String headername,line;
  config_filename=NULL;
  initsetting_netcdf(&nc_config);
  compress=0;
  isnetcdf4=FALSE;
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-var"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Missing argument after option '-var'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        var=argv[++iarg];
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
      else if(!strcmp(argv[iarg],"-netcdf4"))
        isnetcdf4=TRUE;
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
    fprintf(stderr,"Error: Missing argument(s).\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  if(config_filename!=NULL)
  {
    if(parse_config_netcdf(&nc_config,config_filename))
    {
      fprintf(stderr,"Error reading NetCDF configuration file `%s`.\n",config_filename);
      return EXIT_FAILURE;
    }
  }
  soil=opencoord_netcdf(argv[iarg],var,&nc_config,TRUE);
  if(soil==NULL)
    return EXIT_FAILURE;
  getresolution_netcdf(soil,&resolution);
  n=numcoord_netcdf(soil);
  if(n==-1)
  {
    fputs("ERROR165: Cannot get number of cells from soil code file.\n",stderr);
    return EXIT_FAILURE;
  }
  printf("%d cells found in `%s`.\n",n,argv[iarg]);
  grid_soil=newvec(Coord,n);
  check(grid_soil);
  index=newvec(int,n);
  check(index);
  lon=getlon_netcdf(soil,&nlon);
  lat=getlat_netcdf(soil,&nlat);
  for(i=0;i<n;i++)
  {
    if(readcoord_netcdf(soil,grid_soil+i,&resolution,&soilcode))
    {
      fprintf(stderr,"ERROR190: Cannot read coordinate from '%s' for cell %d.\n",
              argv[iarg],i);
      return EXIT_FAILURE;
    }
    for(j=0;j<nlon;j++)
      if(grid_soil[i].lon==lon[j])
      {
        offset[1]=j;
        break;
      }
    for(j=0;j<nlat;j++)
      if(grid_soil[i].lat==lat[j])
      {
        offset[0]=j;
        break;
      }
    index[i]=offset[0]*nlon+offset[1];
#ifdef DEBUG
    printf("%d %d\n",i,index[i]);
#endif
  }
  name.name=argv[iarg+1];
  name.fmt=CLM;
  gridfile=opencoord(&name,TRUE);
  if(gridfile==NULL)
    return EXIT_FAILURE;
  if(numcoord(gridfile)!=n)
  {
    fprintf(stderr,"Number of cells=%d in '%s' differs from %d in '%s'.\n",
            numcoord(gridfile),argv[iarg+1],n,argv[iarg]);
    return EXIT_FAILURE;
  }
  getcellsizecoord(&glon,&glat,gridfile);
  if(resolution.lon!=glon || resolution.lat!=glat)
  {
    fprintf(stderr,"Cell size (%g,%g) in '%s' differs from (%g,%g) in '%s'.\n",
            glon,glat,argv[iarg+1],resolution.lon,resolution.lat,argv[iarg]);
    return EXIT_FAILURE;
  }
  grid=newvec(Coord,n);
  check(grid);
  for(i=0;i<n;i++)
  {
    if(readcoord(gridfile,grid+i,&resolution))
    {
      fprintf(stderr,"Error reading cell %d in `%s`.\n",
              i,argv[iarg+1]);
      return EXIT_FAILURE;
    }
  }
  closecoord(gridfile);
  file=fopen(argv[iarg+2],"rb");
  if(file==NULL)
  {
    printfopenerr(argv[iarg+2]);
    return EXIT_FAILURE;
  }
  version=READ_VERSION;
  if(freadanyheader(file,&header,&swap,headername,&version,TRUE))
  {
    fprintf(stderr,"Invalid header in '%s'.\n",argv[iarg+2]);
    return EXIT_FAILURE;
  }
  if(resolution.lon!=header.cellsize_lon || resolution.lat!=header.cellsize_lat)
  {
    fprintf(stderr,"Cell size (%g,%g) in '%s' differs from (%g,%g) in '%s'.\n",
            header.cellsize_lon,header.cellsize_lat,argv[iarg+2],resolution.lon,resolution.lat,argv[iarg]);
    return EXIT_FAILURE;
  }
  if(header.ncell!=n)
  {
    fprintf(stderr,"Number of cells=%d in '%s' differs from %d in '%s'.\n",
            header.ncell,argv[iarg+2],n,argv[iarg]);
    return EXIT_FAILURE;
  }
  if(header.nbands>2)
  {
    fprintf(stderr,"Number of bands=%d in '%s' must be 1 or 2.\n",
            header.nbands,argv[iarg+2]);
    return EXIT_FAILURE;
  }
  if(version>2 && header.datatype!=LPJ_INT)
  {
    fprintf(stderr,"Datatype of file '%s' is %s, must be int.\n",argv[iarg+2],typenames[header.datatype]);
    return EXIT_FAILURE;
  }
  if(isnetcdf4)
    rc=nc_create(argv[iarg+3],(compress) ? NC_CLOBBER|NC_NETCDF4 : NC_CLOBBER,&ncid);
  else
    rc=nc_create(argv[iarg+3],NC_CLOBBER,&ncid);
  if(rc)
  {
    fprintf(stderr,"ERROR426: Cannot create file '%s': %s.\n",
            argv[iarg+3],nc_strerror(rc));
    return EXIT_FAILURE;
  }
  time(&t);
  cmdline=catstrvec(argv,argc);
  s=getsprintf("%s: %s",strdate(&t),cmdline);
  check(s);
  rc=nc_put_att_text(ncid,NC_GLOBAL,"history",strlen(s),s);
  error(rc);
  free(s);
  free(cmdline);
  rc=nc_def_dim(ncid,nc_config.lat.dim,nlat,&lat_dim_id);
  error(rc);
  rc=nc_def_dim(ncid,nc_config.lon.dim,nlon,&lon_dim_id);
  error(rc);
  rc=nc_def_var(ncid,nc_config.lat.name,NC_DOUBLE,1,&lat_dim_id,&lat_var_id);
  error(rc);
  rc=nc_def_var(ncid,nc_config.lon.name,NC_DOUBLE,1,&lon_dim_id,&lon_var_id);
  error(rc);
  rc=nc_put_att_text(ncid,lon_var_id,"units",strlen(nc_config.lon.unit),
                     nc_config.lon.unit);
  error(rc);
  rc=nc_put_att_text(ncid, lon_var_id,"long_name",strlen(nc_config.lon.long_name),nc_config.lon.long_name);
  error(rc);
  rc=nc_put_att_text(ncid, lon_var_id,"standard_name",strlen(nc_config.lon.standard_name),nc_config.lon.standard_name);
  error(rc);
  rc=nc_put_att_text(ncid, lon_var_id,"axis",strlen("X"),"X");
  error(rc);
  rc=nc_put_att_text(ncid,lat_var_id,"units",strlen(nc_config.lat.unit),
                     nc_config.lat.unit);
  error(rc);
  rc=nc_put_att_text(ncid, lat_var_id,"long_name",strlen(nc_config.lat.long_name),nc_config.lat.long_name);
  error(rc);
  rc=nc_put_att_text(ncid, lat_var_id,"standard_name",strlen(nc_config.lat.standard_name),nc_config.lat.standard_name);
  error(rc);
  rc=nc_put_att_text(ncid, lat_var_id,"axis",strlen("Y"),"Y");
  error(rc);
  dim[0]=lat_dim_id;
  dim[1]=lon_dim_id;
  rc=nc_def_var(ncid,"index",NC_INT,2,dim,&index_varid);
  error(rc);
  if(isnetcdf4 && compress)
  {
    rc=nc_def_var_deflate(ncid, index_varid, 0, 1,compress);
    error(rc);
  }
  rc=nc_put_att_text(ncid, index_varid,"standard_name",strlen("index"),"index");
  error(rc);
  rc=nc_put_att_text(ncid, index_varid,"long_name",strlen(INDEX_LONG_NAME),INDEX_LONG_NAME);
  error(rc);
  nc_put_att_int(ncid, index_varid,"missing_value",NC_INT,1,&nc_config.missing_value.i);
  rc=nc_put_att_int(ncid, index_varid,"_FillValue",NC_INT,1,&nc_config.missing_value.i);
  error(rc);
  if(header.nbands==2)
  {
    rc=nc_def_var(ncid,"riverlen",NC_FLOAT,2,dim,&len_varid);
    if(isnetcdf4 && compress)
    {
      rc=nc_def_var_deflate(ncid, len_varid, 0, 1,compress);
      error(rc);
    }
    rc=nc_put_att_text(ncid,len_varid,"units",strlen("m"),"m");
    error(rc);
    rc=nc_put_att_text(ncid, len_varid,"standard_name",strlen("river_length"),"river_length");
    error(rc);
    rc=nc_put_att_text(ncid, len_varid,"long_name",strlen("length of river"),"length of river");
    error(rc);
    nc_put_att_float(ncid, len_varid,"missing_value",NC_FLOAT,1,&nc_config.missing_value.f);
    rc=nc_put_att_float(ncid, len_varid,"_FillValue",NC_FLOAT,1,&nc_config.missing_value.f);
    error(rc);
  }
  rc=nc_enddef(ncid);
  error(rc);
  rc=nc_put_var_double(ncid,lat_var_id,lat);
  error(rc);
  rc=nc_put_var_double(ncid,lon_var_id,lon);
  error(rc);
  out=newvec(int,nlon*nlat);
  check(out);
  for(i=0;i<nlon*nlat;i++)
    out[i]=nc_config.missing_value.i;
  if(header.nbands==2)
  {
    len=newvec(float,nlon*nlat);
    check(len);
    for(i=0;i<nlon*nlat;i++)
      len[i]=nc_config.missing_value.f;
  }
  for(i=0;i<n;i++)
  {
    src_cell=findcoord(grid+i,grid_soil,&resolution,n);
    if(src_cell==NOT_FOUND)
    {
      fprintf(stderr,"Source cell (%s) not found in `%s`.\n",
              sprintcoord(line,grid+i),argv[iarg]);
      return EXIT_FAILURE;
    }
    freadint(data,header.nbands,swap,file);
#ifdef DEBUG
    printf("data[0]=%d\n",data[0]);
#endif
    if(data[0]<0)
     out[index[src_cell]]=data[0];
    else
    {
      if(data[0]>=n)
      {
        fprintf(stderr,"Index for cell (%s)=%d in '%s' must be in <=%d.\n",
                sprintcoord(line,grid+i),data[0],argv[iarg+2],n-1);
        return EXIT_FAILURE;
      }
      dst_cell=findcoord(grid+data[0],grid_soil,&resolution,n);
      if(dst_cell==NOT_FOUND)
      {
        fprintf(stderr,"Destination cell (%s) not found in `%s`.\n",
                sprintcoord(line,grid+data[0]),argv[iarg]);
        return EXIT_FAILURE;
      }
      out[index[src_cell]]=index[dst_cell];
#ifdef DEBUG
      printf("out(%d]=%d\n",index[src_cell],index[dst_cell]);
#endif
    }
    if(header.nbands==2)
      len[index[src_cell]]=data[1];
  }
  rc=nc_put_var_int(ncid,index_varid,out);
  if(header.nbands==2)
    rc=nc_put_var_float(ncid,len_varid,len);
  nc_close(ncid);
  closecoord_netcdf(soil);
  return EXIT_SUCCESS;
#else
  fprintf(stderr,"ERROR401: NetCDF is not supported in this version of %s.\n",argv[0]);
  return EXIT_FAILURE;
#endif
} /* of 'main' */

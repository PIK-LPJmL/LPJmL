/**************************************************************************************/
/**                                                                                \n**/
/**                c  d  f  2  g  r  i  d  .  c                                    \n**/
/**                                                                                \n**/
/**     Converts NetCDF data grid file into LPJ grid file in raw or CLM format     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE  "Usage: %s [-var name] [-{float|double}] [-scale s] [-raw] [-json] netcdffile gridfile\n"

#ifdef USE_NETCDF
#include <netcdf.h>

#define error(rc) if(rc){ fprintf(stderr,"ERROR421: Cannot read '%s': %s.\n",argv[iarg],nc_strerror(rc)); return EXIT_FAILURE;}

typedef struct
{
  int index;
  int ilon,ilat;
} Data;

static int cmp(const void *a,const void *b)
{
  return ((const Data *)a)->index-((const Data *)b)->index;
} /* of 'cmp' */
#endif

int main(int argc,char **argv)
{
#ifdef USE_NETCDF
  int rc,ncid,var_id,dimids[2],i,j,nvars,lon_id,lat_id,ndims,x,y,iarg,len;
  double *lat,*lon;
  double scalar=0.01;
  size_t lat_len,lon_len;
  int missing_value;
  Bool israw,isjson,scalar_set;
  char name[NC_MAX_NAME+1],*endptr;
  Header header;
  Metadata metadata;
  Coord coord;
  Netcdf_config netcdf_config;
  char *var;
  char *out_json,*arglist;
  char *title=NULL;
  FILE *out;
  Data *data;
  int *index;
  var=NULL;
  header.datatype=LPJ_SHORT;
  header.scalar=0.01;
  israw=isjson=scalar_set=FALSE;
  initsetting_netcdf(&netcdf_config);
  initmetadata(&metadata,NULL);
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
      else if(!strcmp(argv[iarg],"-float"))
      {
        header.datatype=LPJ_FLOAT;
        header.scalar=1;
        scalar=1;
      }
      else if(!strcmp(argv[iarg],"-double"))
      {
        header.datatype=LPJ_DOUBLE;
        header.scalar=1;
        scalar=1;
      }
      else if(!strcmp(argv[iarg],"-raw"))
      {
        israw=TRUE;
      }
      else if(!strcmp(argv[iarg],"-json"))
      {
        isjson=TRUE;
      }
      else if(!strcmp(argv[iarg],"-scale"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Missing argument after option '-scale'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        header.scalar=scalar=strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for scale.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
        if(scalar!=1)
          scalar_set=TRUE;
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
  if(argc<iarg+2)
  {
    fprintf(stderr,"Missing argument(s).\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  if(header.datatype!=LPJ_SHORT && scalar_set)
  {
    fprintf(stderr,"Warning: Scaling set to %g but datatype is %s, scaling set to 1.\n",
            scalar,typenames[header.datatype]);

    header.scalar=1;
    scalar=1;
  }
  rc=nc_open(argv[iarg],NC_NOWRITE,&ncid);
  if(rc)
  {
    fprintf(stderr,"ERROR409: Cannot open '%s': %s.\n",
            argv[iarg],nc_strerror(rc));
    return EXIT_FAILURE;
  }
  if(var==NULL)
  {
    nc_inq_nvars(ncid,&nvars);
    for(j=0;j<nvars;j++)
    {
      nc_inq_varname(ncid,j,name);
      if(strcmp(name,netcdf_config.lon_bnds.name) && strcmp(name,netcdf_config.lat_bnds.name))
      {
        nc_inq_varndims(ncid,j,&ndims);
        if(ndims==2)
        {
          var_id=j;
          break;
        }
      }
    }
    if(j==nvars)
    {
      fprintf(stderr,"ERRO405: No variable with 2 dimensions found in '%s'.\n",argv[iarg]);
      nc_close(ncid);
      return EXIT_FAILURE;
    }
  }
  else if(nc_inq_varid(ncid,var,&var_id))
  {
    fprintf(stderr,"ERROR406: Cannot find variable '%s' in '%s'.\n",var,argv[iarg]);
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  nc_inq_varndims(ncid,var_id,&ndims);
  if(ndims!=2)
  {
    fprintf(stderr,"ERROR408: Invalid number of dimensions %d in '%s', must be 2.\n",
            ndims,argv[iarg]);
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  nc_inq_vardimid(ncid,var_id,dimids);
  nc_inq_dimname(ncid,dimids[1],name);
  rc=nc_inq_varid(ncid,name,&lon_id);
  if(rc)
  {
    fprintf(stderr,"ERROR410: Cannot read %s in '%s': %s.\n",
            name,argv[iarg],nc_strerror(rc));
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  nc_inq_dimlen(ncid,dimids[1],&lon_len);
  lon=newvec(double,lon_len);
  if(lon==NULL)
  {
    printallocerr("lon");
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  rc=nc_get_var_double(ncid,lon_id,lon);
  if(rc)
  {
    free(lon);
    fprintf(stderr,"ERROR410: Cannot read longitude in '%s': %s.\n",
            argv[iarg],nc_strerror(rc));
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  nc_inq_dimname(ncid,dimids[0],name);
  rc=nc_inq_varid(ncid,name,&lat_id);
  if(rc)
  {
    fprintf(stderr,"ERROR410: Cannot read %s in '%s': %s.\n",
            name,argv[iarg],nc_strerror(rc));
    free(lon);
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  nc_inq_dimlen(ncid,dimids[0],&lat_len);
  lat=newvec(double,lat_len);
  if(lat==NULL)
  {
    free(lon);
    printallocerr("lat");
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  rc=nc_get_var_double(ncid,lat_id,lat);
  if(rc)
  {
    free(lon);
    free(lat);
    fprintf(stderr,"ERROR410: Cannot read latitude in '%s': %s.\n",
            argv[iarg],nc_strerror(rc));
    nc_close(ncid);
    return EXIT_FAILURE;
  }

  rc=nc_get_att_int(ncid,var_id,"missing_value",&missing_value);
  if(rc)
    rc=nc_get_att_int(ncid,var_id,"_FillValue",&missing_value);
  if(rc)
  {
    fprintf(stderr,"WARNING402: Cannot read missing or fill value in '%s': %s, set to %d.\n",
            argv[iarg],nc_strerror(rc),netcdf_config.missing_value.i);
    missing_value=netcdf_config.missing_value.i;
  }
  header.cellsize_lon=(lon[lon_len-1]-lon[0])/(lon_len-1);
  header.cellsize_lat=(float)fabs((lat[lat_len-1]-lat[0])/(lat_len-1));
  if(header.datatype==LPJ_SHORT && (isfloatcoord(header.cellsize_lon*0.5,scalar) || isfloatcoord(header.cellsize_lat*0.5,scalar)))
    fprintf(stderr,"Warning: Cell size (%g,%g) does not allow short datatype for grid with scaling factor %g.\n",
            header.cellsize_lat,header.cellsize_lon,scalar);

  index=newvec(int,lat_len*lon_len);
  if(index==NULL)
  {
    free(lon);
    free(lat);
    printallocerr("index");
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  rc=nc_get_var_int(ncid,var_id,index);
  error(rc);
  if(isjson)
  {
    metadata.history=getattr_netcdf(ncid,NC_GLOBAL,"history");
    metadata.source=getattr_netcdf(ncid,NC_GLOBAL,"source");
    title=getattr_netcdf(ncid,NC_GLOBAL,"title");
    if(nc_inq_natts(ncid,&len))
      metadata.n_attr=0;
    else
    {
      metadata.attrs=newvec(Attr,len);
      if(metadata.attrs==NULL)
      {
        printallocerr("attrs");
        free(index);
        free(lon);
        free(lat);
        nc_close(ncid);
        return EXIT_FAILURE;
      }
      metadata.n_attr=0;
      for(i=0;i<len;i++)
      {
        if(!nc_inq_attname(ncid,NC_GLOBAL,i,name))
        {
          if(strcmp(name,"history") && strcmp(name,"source") && strcmp(name,"title"))
          {
            metadata.attrs[metadata.n_attr].value=getattr_netcdf(ncid,NC_GLOBAL,name);
            if(metadata.attrs[metadata.n_attr].value!=NULL)
              metadata.attrs[metadata.n_attr++].name=strdup(name);
          }
        }
      }
    }
  }
  nc_close(ncid);
  /* find number of valid cells in grid NetCDF file */
  header.ncell=0;
  for(i=0;i<lat_len*lon_len;i++)
    if(index[i]!=missing_value)
      header.ncell++;
  if(header.ncell==0)
  {
    fprintf(stderr,"No grid cells found in '%s'.\n",argv[iarg]);
    free(index);
    free(lon);
    free(lat);
    freemetadata(&metadata);
    return EXIT_FAILURE;
  }
  data=newvec(Data,header.ncell);
  if(data==NULL)
  {
    printallocerr("data");
    free(index);
    free(lon);
    free(lat);
    freemetadata(&metadata);
    return EXIT_FAILURE;
  }
  header.ncell=0;
  for(y=0;y<lat_len;y++)
    for(x=0;x<lon_len;x++)
    {
      if(index[y*lon_len+x]!=missing_value)
      {
        /* valid cell found, store index and ilat,ilon */
        data[header.ncell].index=index[y*lon_len+x];
        data[header.ncell].ilon=x;
        data[header.ncell].ilat=y;
#ifdef DEBUG
        printf("%d %d %d\n",index[y*lon_len+x],x,y);
#endif
        header.ncell++;
      }
    }
  free(index);
  /* sort in ascending order of index */
  qsort(data,header.ncell,sizeof(Data),cmp);
  header.firstcell=data[0].index;
  for(i=1;i<header.ncell;i++)
    if(data[i].index!=i+header.firstcell)
    {
      fprintf(stderr,"Missing or double index in grid NetCDF file '%s' at cell %d.\n",
              argv[iarg],i);
      free(data);
      free(lon);
      free(lat);
      freemetadata(&metadata);
      return EXIT_FAILURE;
    }
  header.nyear=1;
  header.nstep=1;
  header.timestep=1;
  header.firstyear=1901;
  header.nbands=2;
  header.order=CELLYEAR;
  out=fopen(argv[iarg+1],"wb");
  if(out==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[iarg+1],strerror(errno));
    free(data);
    free(lon);
    free(lat);
    freemetadata(&metadata);
    return EXIT_FAILURE;
  }
  if(!israw)
    fwriteheader(out,&header,LPJGRID_HEADER,LPJGRID_VERSION);
  for(i=0;i<header.ncell;i++)
  {
    coord.lat=lat[data[i].ilat];
    coord.lon=lat[data[i].ilon];
    if(writecoord(out,&coord,scalar,header.datatype))
    {
      fprintf(stderr,"Error writing grid file '%s': %s.\n",
              argv[iarg+1],strerror(errno));
      fclose(out);
      free(data);
      free(lon);
      free(lat);
      freemetadata(&metadata);
      return EXIT_FAILURE;
    }
  }
  fclose(out);
  free(data);
  free(lon);
  free(lat);
  printf("Number of cells: %d\n",header.ncell);
  if(isjson)
  {
    out_json=malloc(strlen(argv[iarg+1])+strlen(JSON_SUFFIX)+1);
    if(out_json==NULL)
    {
      printallocerr("filename");
      freemetadata(&metadata);
      return EXIT_FAILURE;
    }
    strcat(strcpy(out_json,argv[iarg+1]),JSON_SUFFIX);
    arglist=catstrvec(argv,argc);
    if(arglist==NULL)
    {
    {
      printallocerr("arglist");
      free(out_json);
      freemetadata(&metadata);
      return EXIT_FAILURE;
    }
    }
    out=fopen(out_json,"w");
    if(out==NULL)
    {
      printfcreateerr(out_json);
      free(out_json);
      free(arglist);
      freemetadata(&metadata);
      return EXIT_FAILURE;
    }
    free(out_json);
    metadata.variable=strdup("grid");
    metadata.unit=strdup("degree");
    metadata.long_name=strdup("cell coordinates");
    fprintjson(out,argv[iarg+1],title,arglist,&header,&metadata,NULL,LPJ_SHORT,(israw) ? RAW : CLM,LPJGRID_HEADER,FALSE,LPJGRID_VERSION);
    free(arglist);
    free(title);
    freemetadata(&metadata);
    fclose(out);
  }
  return EXIT_SUCCESS;
#else
  fprintf(stderr,"ERROR401: NetCDF is not supported in this version of %s.\n",argv[0]);
  return EXIT_FAILURE;
#endif
} /* of 'main' */

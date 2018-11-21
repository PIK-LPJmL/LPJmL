/**************************************************************************************/
/**                                                                                \n**/
/**                c  d  f  2  c  o  o  r  d  .  c                                 \n**/
/**                                                                                \n**/
/**     Converts NetCDF data file into LPJ coord data file                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE  "Usage: %s [-var name] [-index i] [-float] [-scale s] netcdffile coordfile\n"

#if defined(USE_NETCDF) || defined(USE_NETCDF4)
#include <netcdf.h>
#endif

#define error(rc) if(rc){ fprintf(stderr,"ERROR: Cannot read '%s': %s.\n",argv[i],nc_strerror(rc)); return EXIT_FAILURE;}

int main(int argc,char **argv)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int rc,ncid,var_id,*dimids,i,j,nvars,lon_id,lat_id,ndims,index,first;

  float *lat,*lon;
  size_t lat_len,lon_len;
  size_t offsets[3],counts[3]={1,1,1};
  float missing_value,data;
  char name[NC_MAX_NAME+1],*endptr;
  Header header;
  Intcoord coord;
  struct
  {
    float lon,lat;
  } coord_f;
  char *var;
  FILE *out;
  Bool isfloat;
  var=NULL;
  isfloat=FALSE;
  header.scalar=0.01;
  offsets[0]=0;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-var"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Missing argument after option '-var'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        var=argv[++i];
      }
      else if(!strcmp(argv[i],"-float"))
      {
        isfloat=TRUE;
        header.scalar=1;
      }
      else if(!strcmp(argv[i],"-index"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Missing argument after option '-index'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        offsets[0]=strtol(argv[++i],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for index.\n",argv[i]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[i],"-scale"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Missing argument after option '-scale'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        header.scalar=strtod(argv[++i],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for scale.\n",argv[i]);
          return EXIT_FAILURE;
        }
      }

      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[i],argv[0]); 
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<i+2)
  {
    fprintf(stderr,"Missing argument(s).\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  rc=nc_open(argv[i],NC_NOWRITE,&ncid);
  if(rc)
  {
    fprintf(stderr,"ERROR409: Cannot open '%s': %s.\n",
            argv[i],nc_strerror(rc));
    return EXIT_FAILURE;
  }
  if(var==NULL)
  {
    nc_inq_nvars(ncid,&nvars);
    for(j=0;j<nvars;j++)
    {
      nc_inq_varname(ncid,j,name);
      if(strcmp(name,LON_NAME) && strcmp(name,LAT_NAME) && strcmp(name,"time"))
      {
        var_id=j;
        break;
      }
    }
    if(j==nvars)
    {
      fprintf(stderr,"ERRO405: No variable found in '%s'.\n",argv[i]);
      nc_close(ncid);
      return EXIT_FAILURE;
    }
  }
  else if(nc_inq_varid(ncid,var,&var_id))
  {
    fprintf(stderr,"ERROR406: Cannot find variable '%s' in '%s'.\n",var,argv[i]);
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  nc_inq_varndims(ncid,var_id,&ndims);
  if(ndims<2)
  {
    fprintf(stderr,"ERROR408: Invalid number of dimensions %d in '%s'.\n",
            ndims,argv[i]);
    return TRUE;
  }
  else if(ndims==2)
    first=0;
  else
    first=1;
  index=ndims-1;
  dimids=newvec(int,ndims);
  if(dimids==NULL)
  {
    printallocerr("dimids");
    return TRUE;
  }
  nc_inq_vardimid(ncid,var_id,dimids);
  nc_inq_dimname(ncid,dimids[index],name);
  rc=nc_inq_varid(ncid,name,&lon_id);
  if(rc)
  {
    fprintf(stderr,"ERROR410: Cannot read %s in '%s': %s.\n",
            name,argv[i],nc_strerror(rc));
    free(dimids);
    return TRUE;
  }
  nc_inq_dimlen(ncid,dimids[index],&lon_len);
  lon=newvec(float,lon_len);
  if(lon==NULL)
  {
    free(dimids);
    printallocerr("dimids");
    return EXIT_FAILURE;
  }
  rc=nc_get_var_float(ncid,lon_id,lon);
  if(rc)
  {
    free(dimids);
    free(lon);
    fprintf(stderr,"ERROR410: Cannot read longitude in '%s': %s.\n",
            argv[i],nc_strerror(rc));
    return EXIT_FAILURE;
  }
  nc_inq_dimname(ncid,dimids[index-1],name);
  rc=nc_inq_varid(ncid,name,&lat_id);
  if(rc)
  {
    fprintf(stderr,"ERROR410: Cannot read %s in '%s': %s.\n",
            name,argv[i],nc_strerror(rc));
    free(dimids);
    free(lon);
    return EXIT_FAILURE;
  }
  nc_inq_dimlen(ncid,dimids[index-1],&lat_len);
  lat=newvec(float,lat_len);
  if(lat==NULL)
  {
    free(dimids);
    free(lon);
    printallocerr("lat");
    return EXIT_FAILURE;
  }
  rc=nc_get_var_float(ncid,lat_id,lat);
  if(rc)
  {
    free(dimids);
    free(lon);
    free(lat);
    fprintf(stderr,"ERROR410: Cannot read latitude in '%s': %s.\n",
            argv[i],nc_strerror(rc));
    return EXIT_FAILURE;
  }

  rc=nc_get_att_float(ncid,var_id,"missing_value",&missing_value);
  if(rc)
    rc=nc_get_att_float(ncid,var_id,"_FillValue",&missing_value);
  if(rc)
  {
    fprintf(stderr,"ERROR407: Cannot read missing value in '%s': %s.\n",
            argv[i],nc_strerror(rc));
    free(lon);
    free(lat);
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  out=fopen(argv[i+1],"wb");
  if(out==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[i+1],strerror(errno));
    free(lon);
    free(lat);
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  header.cellsize_lon=(lon[lon_len-1]-lon[0])/(lon_len-1);
  header.cellsize_lat=(float)fabs((lat[lat_len-1]-lat[0])/(lat_len-1));
  header.datatype=(isfloat) ? LPJ_FLOAT : LPJ_SHORT;
  fwriteheader(out,&header,LPJGRID_HEADER,LPJGRID_VERSION);
  header.ncell=0;
  for(offsets[first]=0;offsets[first]<lat_len;offsets[first]++)
  {
    for(offsets[first+1]=0;offsets[first+1]<lon_len;offsets[first+1]++)
    {
      rc=nc_get_vara_float(ncid,var_id,offsets,counts,&data);
      error(rc);
      if((!isnan(missing_value) && data!=missing_value) ||
          (isnan(missing_value) && !isnan(data)))
      {
        if(isfloat)
        {
          coord_f.lat=lat[offsets[first]];
          coord_f.lon=lon[offsets[first+1]];
          fwrite(&coord_f,sizeof(coord_f),1,out);
        }
        else
        { 
         coord.lat=(short)(lat[offsets[first]]/header.scalar);
         coord.lon=(short)(lon[offsets[first+1]]/header.scalar);
#ifdef DEBUG
          printf("%.3f %3f %d %d\n",lat[offsets[1]],lon[offsets[2]],coord.lat,coord.lon);
#endif
          fwrite(&coord,sizeof(coord),1,out);
        }
        header.ncell++;
      }
    }
  }
  rewind(out);
  header.firstcell=0;
  header.nyear=1;
  header.firstyear=1901;
  header.nbands=2;
  header.order=CELLYEAR;
  fwriteheader(out,&header,LPJGRID_HEADER,LPJGRID_VERSION);
  fclose(out);
  free(lon);
  free(lat);
  nc_close(ncid);
  printf("Number of cells: %d\n",header.ncell);
  return EXIT_SUCCESS;
#else
  fprintf(stderr,"ERROR401: NetCDF is not supported in this version of %s.\n",argv[0]);
  return EXIT_FAILURE;
#endif
} /* of 'main' */

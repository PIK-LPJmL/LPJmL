/**************************************************************************************/
/**                                                                                \n**/
/**                c  d  f  2  s  o  i  l  .  c                                    \n**/
/**                                                                                \n**/
/**     Converts NetCDF data file into LPJ soil and coord data file                \n**/
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
#endif

#define USAGE  "Usage: %s [-var name] [-float] [-scale s] [-soilmap] netcdffile coordfile soilfile\n"

#define error(rc) if(rc){ fprintf(stderr,"ERROR: Cannot read '%s': %s.\n",argv[1],nc_strerror(rc)); return EXIT_FAILURE;}

#define SOIL_NAME "stexture"

int main(int argc,char **argv)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int rc,ncid,var_id,dim_id,i;
  size_t ilat,ilon;
  float *lat,*lon;
  size_t lat_len,lon_len;
  int missing_value,*data;
  Header header;
  Intcoord coord;
  FILE *out,*soil;
  Byte soilcode;
  Bool isfloat,ismap;
  Byte soilmap[]={Sa,LoSa,SaLo,SiLo,Lo,SaClLo,SiClLo,ClLo,SaCl,SiCl,Cl,Si};
  struct
  {
    float lon,lat;
  } coord_f;
  char *var=SOIL_NAME;
  char *endptr;
  header.scalar=0.01;
  isfloat=ismap=FALSE;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-var"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-var'.\n"
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
      else if(!strcmp(argv[i],"-soilmap"))
        ismap=TRUE;
      else if(!strcmp(argv[i],"-scale"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-scale'.\n"
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
  if(argc<i+3)
  {
    fprintf(stderr,"Error: Missing argument(s).\n"
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
  rc=nc_inq_varid(ncid,LON_NAME,&var_id);
  error(rc);
  rc=nc_inq_dimid(ncid,LON_NAME,&dim_id);
  error(rc);
  rc=nc_inq_dimlen(ncid,dim_id,&lon_len);
  error(rc);
  lon=newvec(float,lon_len);
  if(lon==NULL)
  {
    printallocerr("lon");
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  rc=nc_get_var_float(ncid,var_id,lon);
  if(rc)
  {
    fprintf(stderr,"ERROR410: Cannot read longitude in '%s': %s.\n",
            argv[i],nc_strerror(rc));
    free(lon);
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  rc=nc_inq_varid(ncid,LAT_NAME,&var_id);
  if(rc)
  {
    fprintf(stderr,"ERROR411: Cannot read latitude in '%s': %s.\n",
            argv[i],nc_strerror(rc));
    free(lon);
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  rc=nc_inq_dimid(ncid,LAT_NAME,&dim_id);
  if(rc)
  {
    fprintf(stderr,"ERROR411: Cannot read latitude in '%s': %s.\n",
            argv[i],nc_strerror(rc));
    free(lon);
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  rc=nc_inq_dimlen(ncid,dim_id,&lat_len);
  if(rc)
  {
    fprintf(stderr,"ERROR403: Cannot read '%s': %s.\n",
            argv[i],nc_strerror(rc));
    free(lon);
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  lat=newvec(float,lat_len);
  if(lat==NULL)
  {
    printallocerr("lat");
    free(lon);
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  rc=nc_get_var_float(ncid,var_id,lat);
  if(rc)
  {
    fprintf(stderr,"ERROR404: Cannot read latitude in '%s': %s.\n",
            argv[i],nc_strerror(rc));
    free(lon);
    free(lat);
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  rc=nc_inq_varid(ncid,var,&var_id);
  if(rc)
  {
    fprintf(stderr,"ERRO405: No variable found in '%s'.\n",argv[i]);
    free(lon);
    free(lat);
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  rc=nc_get_att_int(ncid,var_id,"missing_value",&missing_value);
  if(rc)
    rc=nc_get_att_int(ncid,var_id,"_FillValue",&missing_value);
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
  fwriteheader(out,&header,LPJGRID_HEADER,LPJGRID_VERSION);
  data=newvec(int,lat_len*lon_len);
  rc=nc_get_var_int(ncid,var_id,data);
  error(rc);
  header.ncell=0;
  soil=fopen(argv[i+2],"wb");
  if(soil==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[i+2],strerror(errno));
    free(lon);
    free(lat);
    free(data);
    nc_close(ncid);
    return EXIT_FAILURE;
  }
  for(ilat=0;ilat<lat_len;ilat++)
  {
    for(ilon=0;ilon<lon_len;ilon++)
    {
      if(data[ilat*lon_len+ilon]!=missing_value)
      {
         if(isfloat)
         {
           coord_f.lat=lat[ilat];
           coord_f.lon=lon[ilon];
           fwrite(&coord_f,sizeof(coord_f),1,out);
         }
         else
         {
           coord.lat=(short)(lat[ilat]/header.scalar);
           coord.lon=(short)(lon[ilon]/header.scalar);
#ifdef DEBUG
           printf("%.3f %3f %d %d\n",lat[ilat],lon[ilon],coord.lat,coord.lon);
#endif
           fwrite(&coord,sizeof(coord),1,out);
         }
         header.ncell++;
         if(ismap)
           soilcode=(Byte)(soilmap[data[ilat*lon_len+ilon]-1]+1);
         else
           soilcode=(Byte)data[ilat*lon_len+ilon];
         fwrite(&soilcode,1,1,soil);
      }
    }
  }
  fclose(soil);
  rewind(out);
  header.firstcell=0;
  header.nyear=1;
  header.firstyear=1901;
  header.nbands=2;
  header.order=CELLYEAR;
  header.datatype=(isfloat) ? LPJ_FLOAT : LPJ_SHORT;
  header.cellsize_lon=(lon[lon_len-1]-lon[0])/(lon_len-1);
  header.cellsize_lat=(float)(fabs(lat[lat_len-1]-lat[0])/(lat_len-1));
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

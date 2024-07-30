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

#define USAGE  "Usage: %s [-var name] [-index i] [-{float|double}] [-scale s] [-raw] [-json] netcdffile coordfile\n"

#if defined(USE_NETCDF) || defined(USE_NETCDF4)
#include <netcdf.h>
#endif

#define error(rc) if(rc){ fprintf(stderr,"ERROR421: Cannot read '%s': %s.\n",argv[i],nc_strerror(rc)); return EXIT_FAILURE;}

int main(int argc,char **argv)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int rc,ncid,var_id,*dimids,i,j,nvars,lon_id,lat_id,ndims,index,first;

  double *lat,*lon;
  size_t lat_len,lon_len;
  size_t offsets[4]={0,0,0,0},counts[4]={1,1,1,1};
  double missing_value,data;
  char name[NC_MAX_NAME+1],*endptr;
  Header header;
  Intcoord coord;
  struct
  {
    double lon,lat;
  } coord_d;
  struct
  {
    float lon,lat;
  } coord_f;
  char *var;
  char *out_json,*arglist;
  FILE *out;
  Bool isjson,israw;
  var=NULL;
  header.datatype=LPJ_SHORT;
  header.scalar=0.01;
  isjson=israw=FALSE;
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
      else if(!strcmp(argv[i],"-json"))
        isjson=TRUE;
      else if(!strcmp(argv[i],"-raw"))
        israw=TRUE;
      else if(!strcmp(argv[i],"-float"))
      {
        header.datatype=LPJ_FLOAT;
        header.scalar=1;
      }
      else if(!strcmp(argv[i],"-double"))
      {
        header.datatype=LPJ_DOUBLE;
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
        header.scalar=(float)strtod(argv[++i],&endptr);
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
      if(strcmp(name,LON_NAME) && strcmp(name,LON_STANDARD_NAME) && strcmp(name,LAT_NAME) && strcmp(name,LAT_STANDARD_NAME) && strcmp(name,TIME_NAME) && strcmp(name,PFT_NAME) && strcmp(name,DEPTH_NAME) && strcmp(name,BNDS_NAME))
      {
        nc_inq_varndims(ncid,j,&ndims);
        if(ndims>1)
        {
          var_id=j;
          break;
        }
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
    fprintf(stderr,"ERROR408: Invalid number of dimensions %d in '%s', must be >1.\n",
            ndims,argv[i]);
    return EXIT_FAILURE;
  }
  else if(ndims>4)
  {
    fprintf(stderr,"ERROR408: Invalid number of dimensions %d in '%s', must be <5.\n",
            ndims,argv[i]);
    return EXIT_FAILURE;
  }
  else
    first=ndims-2;
  index=ndims-1;
  dimids=newvec(int,ndims);
  if(dimids==NULL)
  {
    printallocerr("dimids");
    return EXIT_FAILURE;
  }
  nc_inq_vardimid(ncid,var_id,dimids);
  nc_inq_dimname(ncid,dimids[index],name);
  rc=nc_inq_varid(ncid,name,&lon_id);
  if(rc)
  {
    fprintf(stderr,"ERROR410: Cannot read %s in '%s': %s.\n",
            name,argv[i],nc_strerror(rc));
    free(dimids);
    return EXIT_FAILURE;
  }
  nc_inq_dimlen(ncid,dimids[index],&lon_len);
  lon=newvec(double,lon_len);
  if(lon==NULL)
  {
    free(dimids);
    printallocerr("lon");
    return EXIT_FAILURE;
  }
  rc=nc_get_var_double(ncid,lon_id,lon);
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
  lat=newvec(double,lat_len);
  if(lat==NULL)
  {
    free(dimids);
    free(lon);
    printallocerr("lat");
    return EXIT_FAILURE;
  }
  rc=nc_get_var_double(ncid,lat_id,lat);
  if(rc)
  {
    free(dimids);
    free(lon);
    free(lat);
    fprintf(stderr,"ERROR410: Cannot read latitude in '%s': %s.\n",
            argv[i],nc_strerror(rc));
    return EXIT_FAILURE;
  }

  rc=nc_get_att_double(ncid,var_id,"missing_value",&missing_value);
  if(rc)
    rc=nc_get_att_double(ncid,var_id,"_FillValue",&missing_value);
  if(rc)
  {
    fprintf(stderr,"WARNING402: Cannot read missing or fill value in '%s': %s, set to %g.\n",
            argv[i],nc_strerror(rc),MISSING_VALUE_FLOAT);
    missing_value=MISSING_VALUE_FLOAT;
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
  if(!israw)
    fwriteheader(out,&header,LPJGRID_HEADER,LPJGRID_VERSION);
  header.ncell=0;
  for(offsets[first]=0;offsets[first]<lat_len;offsets[first]++)
  {
    for(offsets[first+1]=0;offsets[first+1]<lon_len;offsets[first+1]++)
    {
      rc=nc_get_vara_double(ncid,var_id,offsets,counts,&data);
      error(rc);
      if((!isnan(missing_value) && !isnan(data) && data!=missing_value) ||
          (isnan(missing_value) && !isnan(data)))
      {
        switch(header.datatype)
        {
          case LPJ_FLOAT:
            coord_f.lat=(float)lat[offsets[first]];
            coord_f.lon=(float)lon[offsets[first+1]];
            fwrite(&coord_f,sizeof(coord_f),1,out);
            break;
          case LPJ_DOUBLE:
            coord_d.lat=lat[offsets[first]];
            coord_d.lon=lon[offsets[first+1]];
            fwrite(&coord_d,sizeof(coord_d),1,out);
            break;
          default:
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
  header.firstcell=0;
  header.nyear=1;
  header.nstep=1;
  header.timestep=1;
  header.firstyear=1901;
  header.nbands=2;
  header.order=CELLYEAR;
  if(!israw)
  {
    rewind(out);
    fwriteheader(out,&header,LPJGRID_HEADER,LPJGRID_VERSION);
  }
  fclose(out);
  free(lon);
  free(lat);
  nc_close(ncid);
  printf("Number of cells: %d\n",header.ncell);
  if(isjson)
  {
    out_json=malloc(strlen(argv[i+1])+strlen(JSON_SUFFIX)+1);
    if(out_json==NULL)
    {
      printallocerr("filename");
      return EXIT_FAILURE;
    }
    strcat(strcpy(out_json,argv[i+1]),JSON_SUFFIX);
    arglist=catstrvec(argv,argc);
    out=fopen(out_json,"w");
    if(out==NULL)
    {
      printfcreateerr(out_json);
      return EXIT_FAILURE;
    }
    fprintjson(out,argv[i+1],NULL,argv[0],NULL,arglist,&header,NULL,NULL,NULL,0,"grid","degree",NULL,"cell coordinates",NULL,LPJ_SHORT,(israw) ? RAW : CLM,LPJGRID_HEADER,FALSE,LPJGRID_VERSION);
    fclose(out);
  }
  return EXIT_SUCCESS;
#else
  fprintf(stderr,"ERROR401: NetCDF is not supported in this version of %s.\n",argv[0]);
  return EXIT_FAILURE;
#endif
} /* of 'main' */

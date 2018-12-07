/**************************************************************************************/
/**                                                                                \n**/
/**                     c  d  f  2  b  i  n  .  c                                  \n**/
/**                                                                                \n**/
/**     Program converts NetCDF output data into raw binary format                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef USE_UDUNITS
#define USAGE "Usage: %s [-swap] [-v] [-units unit] [-var name] [-clm] [-cellsize size] [-o filename] gridfile netcdffile\n"
#else
#define USAGE "Usage: %s [-swap] [-v] [-var name] [-clm] [-cellsize size] [-o filename] gridfile netcdffile\n"
#endif

#if defined(USE_NETCDF) || defined(USE_NETCDF4)
#include <netcdf.h>

static Bool readdata(Climatefile *file,    /* climate data file */
                     FILE *bin,            /* pointer to output file */
                     const Coord coords[], /* coordinates */
                     const Config *config  /* LPJ configuration */
                    )                      /* returns TRUE on error */
{
  int t,cell,rc,index,n;
  size_t i;
  float *f;
  short *s;
  float data;
  size_t offsets[4];
  int address[2];
  size_t counts[4];
  //printf("len=%d\n",(int)file->var_len);
  counts[0]=1;
  if(file->var_len>1)
  {
    counts[1]=1;
    index=2;
  }
  else
    index=1;
  offsets[index]=offsets[index+1]=0;
  counts[index]=file->nlat;
  counts[index+1]=file->nlon;
  switch(file->datatype)
  {
    case LPJ_FLOAT:
      f=newvec(float,file->nlon*file->nlat);
      if(f==NULL)
      {
        printallocerr("data");
        nc_close(file->ncid);
        return TRUE;
      }
      break;
    case LPJ_SHORT:
      s=newvec(short,file->nlon*file->nlat);
      if(s==NULL)
      {
        printallocerr("data");
        nc_close(file->ncid);
        return TRUE;
      }
      break;
    default:
      fprintf(stderr,"Datatype not supported.\n");
      nc_close(file->ncid);
      return TRUE;
  }
  switch(file->time_step)
  {
    case DAY:
      n=NDAYYEAR;
      break;
    case MONTH:
      n=NMONTH;
      break;
    case YEAR:
      n=1;
      break;
  }
  for(t=0;t<file->nyear*n;t++)
  {
    offsets[0]=t;
    for(i=0;i<file->var_len;i++)
    {
      //printf("t=%d,i=%d\n",t,i);
      if(file->var_len>1)
        offsets[1]=i;
      if(file->datatype==LPJ_FLOAT)
      {
        //printf("%d %d %d %d\n",(int)offsets[0],(int)offsets[1],(int)offsets[2],(int)offsets[3]);
        //printf("%d %d %d %d\n",(int)counts[0],(int)counts[1],(int)counts[2],(int)counts[3]);
        if((rc=nc_get_vara_float(file->ncid,file->varid,offsets,counts,f)))
        {
          free(f);
          fprintf(stderr,"ERROR421: Cannot read float data: %s.\n",
                  nc_strerror(rc)); 
          nc_close(file->ncid);
          return TRUE;
        }
      }
      else
      {
        if((rc=nc_get_vara_short(file->ncid,file->varid,offsets,counts,s)))
        {
          free(s);
          fprintf(stderr,"ERROR421: Cannot read short data: %s.\n",
                 nc_strerror(rc)); 
          nc_close(file->ncid);
          return TRUE;
        }
      }
      for(cell=0;cell<config->ngridcell;cell++)
      {
        if(file->offset)
          address[0]=file->offset-(int)((coords[cell].lat-file->lat_min)/config->resolution.lat+0.5);
        else
          address[0]=(int)((coords[cell].lat-file->lat_min)/config->resolution.lat+0.5);
        address[1]=(int)((coords[cell].lon-file->lon_min)/config->resolution.lon+0.5);
        if(address[0]>=file->nlat || address[1]>=file->nlon)
        {
          fprintf(stderr,"ERROR422: Invalid coordinate for cell %d (",cell);
          fprintcoord(stderr,coords+cell);
          fputs(") in data file.\n",stderr);
          free(f);
          nc_close(file->ncid);
          return TRUE;
        }
        if(file->datatype==LPJ_FLOAT)
        {
          if(f[file->nlon*address[0]+address[1]]==file->missing_value.f)
          {
            fprintf(stderr,"ERROR423: Missing value for cell=%d (",cell);
            fprintcoord(stderr,coords+cell);
            fprintf(stderr,").\n");
            free(f);
            nc_close(file->ncid);
            return TRUE;
          }
          else if(isnan(f[file->nlon*address[0]+address[1]]))
          {
            fprintf(stderr,"ERROR423: Invalid value for cell=%d (",cell);
            fprintcoord(stderr,coords+cell);
            fprintf(stderr,").\n");
            free(f);
            nc_close(file->ncid);
            return TRUE;
          }
          data=file->slope*f[file->nlon*address[0]+address[1]]+file->intercept;
          fwrite(&data,sizeof(float),1,bin);
        }
        else
        {
          if(s[file->nlon*address[0]+address[1]]==file->missing_value.s)
          {
            fprintf(stderr,"ERROR423: Missing value for cell=%d (",cell);
            fprintcoord(stderr,coords+cell);
            fprintf(stderr,").\n");
            free(s);
            nc_close(file->ncid);
            return TRUE;
          }
          fwrite(s+file->nlon*address[0]+address[1],sizeof(short),1,bin);
        }
      }
    }
  }
  if(file->datatype==LPJ_FLOAT)
    free(f);
  else 
    free(s);
  return FALSE;
} /* of 'readdata' */

#endif

int main(int argc,char **argv)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  Coordfile coordfile;
  Filename coord_filename;
  Climatefile data;
  Config config;
  char *units,*var,*outname,*endptr;
  Coord *grid;
  Intcoord intcoord;
  FILE *file;
  int i,j;
  float cellsize_lon,cellsize_lat;
  Bool swap,verbose,isclm;
  swap=verbose=isclm=FALSE;
  units=NULL;
  var=NULL;
  outname="out.bin"; /* default file name for output */
  cellsize_lon=cellsize_lat=0.5;      /* default cell size */
  initconfig(&config);
  for(i=1;i<argc;i++)
  {
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-swap"))
        swap=TRUE; 
      else if(!strcmp(argv[i],"-v"))
        verbose=TRUE; 
      else if(!strcmp(argv[i],"-clm"))
        isclm=TRUE; 
      else if(!strcmp(argv[i],"-var"))
      {
        if(argc==i-1)
        {
          fprintf(stderr,"Missing argument after option '-var'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        var=argv[++i];
      }
#ifdef USE_UDUNITS
      else if(!strcmp(argv[i],"-units"))
      {
        if(argc==i-1)
        {
          fprintf(stderr,"Missing argument after option '-units'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        units=argv[++i];
      }
#endif
      else if(!strcmp(argv[i],"-o"))
      {
        if(argc==i-1)
        {
          fprintf(stderr,"Missing argument after option '-o'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        outname=argv[++i];
      }
      else if(!strcmp(argv[i],"-cellsize"))
      {
        if(i==argc-1)
        {
          fprintf(stderr,"Missing argument after option '-cellsize'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        cellsize_lon=(float)strtod(argv[++i],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-cellsize'.\n",argv[i]);
          return EXIT_FAILURE;
        }
        cellsize_lat=cellsize_lon;
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
  }
  if(argc<i+2)
  {
    fprintf(stderr,"Missing arguments.\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  if(isclm)
  {
    coord_filename.name=argv[i];
    coord_filename.fmt=CLM;
    coordfile=opencoord(&coord_filename,TRUE);
    if(coordfile==NULL)
      return EXIT_FAILURE;
    grid=newvec(Coord,numcoord(coordfile));
    if(grid==NULL)
    {
      printallocerr("grid");
      return EXIT_FAILURE;
    }
    config.ngridcell=numcoord(coordfile);
    if(config.ngridcell==0)
    {
      fprintf(stderr,"Number of cells is zero in '%s'.\n",argv[i]);
      return EXIT_FAILURE;
    }
    getcellsizecoord(&cellsize_lon,&cellsize_lat,coordfile);
    config.resolution.lat=cellsize_lat;
    config.resolution.lon=cellsize_lon;
    for(j=0;j<numcoord(coordfile);j++)
      readcoord(coordfile,grid+j,&config.resolution);
    closecoord(coordfile);
  }
  else
  {
    file=fopen(argv[i],"rb");
    if(file==NULL)
    {
      fprintf(stderr,"Error opening grid file '%s': %s.\n",argv[i],
              strerror(errno));
      return EXIT_FAILURE;
    }
    if(getfilesize(argv[i]) % (sizeof(short)*2)!=0)
      fprintf(stderr,"Warning: File size of '%s' is not multiple of %d.\n",argv[i],(int)(sizeof(float)*2));
    config.ngridcell=getfilesize(argv[i])/sizeof(short)/2;
    if(config.ngridcell==0)
    {
      fprintf(stderr,"Number of cells is zero in '%s'.\n",argv[i]);
      return EXIT_FAILURE;
    }
    grid=newvec(Coord,config.ngridcell);
    if(grid==NULL)
    {
      printallocerr("grid");
      return EXIT_FAILURE;
    }
    for(j=0;j<config.ngridcell;j++)
    {
      readintcoord(file,&intcoord,swap);
      grid[j].lat=intcoord.lat*0.01;
      grid[j].lon=intcoord.lon*0.01;
    }
    config.resolution.lat=cellsize_lat;
    config.resolution.lon=cellsize_lon;
    fclose(file);
  }
  file=fopen(outname,"wb");
  if(file==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",outname,strerror(errno));
    return EXIT_FAILURE;
  }
  for(j=i+1;j<argc;j++)
  {
    if(verbose)
      printf("%s\n",argv[j]); 
    if(openclimate_netcdf(&data,argv[j],var,units,&config))
    {
      fprintf(stderr,"Error opening '%s'.\n",argv[j]);
      return EXIT_FAILURE;
    }
    if(data.time_step==MISSING_TIME)
    {
      fprintf(stderr,"ERROR436: Time axis missing in '%s'.\n",argv[j]);
      return EXIT_FAILURE;
    }
    if(readdata(&data,file,grid,&config))
    {
      fprintf(stderr,"Error reading '%s'.\n",argv[j]);
      return EXIT_FAILURE;
    }
    nc_close(data.ncid);
  }
  fclose(file);
  return EXIT_SUCCESS;
#else
  fprintf(stderr,"ERROR401: NetCDF is not supported in this version of %s.\n",argv[0]);
  return EXIT_FAILURE;
#endif
} /* of 'main' */

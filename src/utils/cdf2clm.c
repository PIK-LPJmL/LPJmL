/**************************************************************************************/
/**                                                                                \n**/
/**                     c  d  f  2  c  l  m  .  c                                  \n**/
/**                                                                                \n**/
/**     Program converts NetCDF input data into CLM format                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef USE_UDUNITS
#define USAGE "Usage: %s [-units unit] [-var name] [-o filename] [-scale factor] [-float] gridfile netcdffile\n"
#else
#define USAGE "Usage: %s [-var name] [-o filename] [-scale factor] [-float] gridfile netcdffile\n"
#endif

#if defined(USE_NETCDF) || defined(USE_NETCDF4)
#include <netcdf.h>

static Bool readclimate2(Climatefile *file,    /* climate data file */
                         float data[],         /* pointer to data read in */
                         int year,             /* year */
                         const Coord coords[], /* coordinates */
                         const Config *config  /* LPJ configuration */
                        )                      /* returns TRUE on error */
{
  int i,cell,rc;
  float *f;
  double *d;
  int size;
  size_t offsets[3];
  size_t counts[3];
  size=(file->isdaily) ? NDAYYEAR : NMONTH;
  offsets[1]=offsets[2]=0;
  offsets[0]=year*size;
  if(file->isdaily && file->isleap)
    offsets[0]+=nleapyears(file->firstyear,year+file->firstyear);
  counts[0]=size;
  counts[1]=file->nlat;
  counts[2]=file->nlon;
  if(file->type==LPJ_FLOAT)
  {
    f=newvec(float,size*file->nlon*file->nlat);
    if(f==NULL)
    {
      printallocerr("data");
      nc_close(file->ncid);
      return TRUE;
    }
    if((rc=nc_get_vara_float(file->ncid,file->varid,offsets,counts,f)))
    {
      free(f);
      fprintf(stderr,"ERROR421: Cannot read float data: %s.\n",
             nc_strerror(rc)); 
      nc_close(file->ncid);
      return TRUE;
    }
  }
  else if(file->type==LPJ_DOUBLE)
  {
    d=newvec(double,size*file->nlon*file->nlat);
    if(d==NULL)
    {
      printallocerr("data");
      nc_close(file->ncid);
      return TRUE;
    }
    if((rc=nc_get_vara_double(file->ncid,file->varid,offsets,counts,d)))
    {
      free(d);
      fprintf(stderr,"ERROR421: Cannot read double data: %s.\n",
             nc_strerror(rc));
      nc_close(file->ncid);
      return TRUE;
    }
  }
  else
  {
    fprintf(stderr,"Datatype not supported.\n");
    nc_close(file->ncid);
    return TRUE;
  }
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(file->offset)
      offsets[1]=file->offset-(int)((coords[cell].lat-file->lat_min)/file->lat_res+0.5);
    else
      offsets[1]=(int)((coords[cell].lat-file->lat_min)/file->lat_res+0.5);
    if(file->is360 && coords[cell].lon<0)
      offsets[2]=(int)((360+coords[cell].lon-file->lon_min)/file->lon_res+0.5);
    else
      offsets[2]=(int)((coords[cell].lon-file->lon_min)/file->lon_res+0.5);
    if(offsets[1]>=file->nlat || offsets[2]>=file->nlon)
    {
      fprintf(stderr,"ERROR422: Invalid coordinate for cell %d (",
              cell);
      fprintcoord(stderr,coords+cell);
      fputs(") in data file.\n",stderr);
      free(f);
      nc_close(file->ncid);
      return TRUE;
    }
    for(i=0;i<size;i++)
    {
      if(file->type==LPJ_FLOAT)
      {
        if(f[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]==file->missing_value.f)
        {
          fprintf(stderr,"ERROR423: Missing value for cell=%d (",cell);
          fprintcoord(stderr,coords+cell);
          fprintf(stderr,") at %s %d.\n",(file->isdaily) ? "day" : "month",i+1);
          free(f);
          nc_close(file->ncid);
          return TRUE;
        }
        else if(isnan(f[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]))
        {
          fprintf(stderr,"ERROR423: Invalid value for cell=%d (",cell);
          fprintcoord(stderr,coords+cell);
          fprintf(stderr,") at %s %d.\n",(file->isdaily) ? "day" : "month",i+1);
          free(f);
          nc_close(file->ncid);
          return TRUE;
        }
        data[cell*size+i]=file->slope*f[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]+file->intercept;
      }
      else
      {
        if(d[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]==file->missing_value.d)
        {
          fprintf(stderr,"ERROR423: Missing value for cell=%d (",cell);
          fprintcoord(stderr,coords+cell);
          fprintf(stderr,") at %s %d.\n",(file->isdaily) ? "day" : "month",i+1);
          free(d);
          nc_close(file->ncid);
          return TRUE;
        }
        else if(isnan(d[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]))
        {
          fprintf(stderr,"ERROR423: Invalid value for cell=%d (",cell);
          fprintcoord(stderr,coords+cell);
          fprintf(stderr,") at %s %d.\n",(file->isdaily) ? "day" : "month",i+1);
          free(d);
          nc_close(file->ncid);
          return TRUE;
        }
        data[cell*size+i]=file->slope*d[file->nlon*(i*file->nlat+offsets[1])+offsets[2]]+file->intercept;
      }
    }
  }
  if(file->type==LPJ_FLOAT)
    free(f);
  else 
    free(d);
  return FALSE;
} /* of 'readclimate2' */

#endif

int main(int argc,char **argv)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  Coordfile coordfile;
  Climatefile climate;
  Config config;
  char *units,*var,*outname,*endptr;
  float scale,*data;
  Filename coord_filename;
  Coord *coords;
  Header header;
  FILE *file;
  int i,j,k,year;
  short *s;
  Bool isfloat;
  units=NULL;
  var=NULL;
  scale=1;
  isfloat=FALSE;
  outname="out.clm"; /* default file name for output */
  initconfig(&config);
  for(i=1;i<argc;i++)
  {
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
#ifdef USE_UDUNITS
      else if(!strcmp(argv[i],"-units"))
      {
        if(argc==i+1)
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
        if(argc==i+1)
        {
          fprintf(stderr,"Missing argument after option '-o'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        outname=argv[++i];
      }
      else if(!strcmp(argv[i],"-float"))
       isfloat=TRUE;
      else if(!strcmp(argv[i],"-scale"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Missing argument after option '-scale'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        scale=(float)strtod(argv[++i],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-scale'.\n",argv[i]);
          return EXIT_FAILURE;
        }
        if(scale==0)
        {
          fputs("Scale factor must not be zero.\n",stderr);
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
  }
  if(argc<i+2)
  {
    fprintf(stderr,"Missing arguments.\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  coord_filename.name=argv[i];
  coord_filename.fmt=CLM;
  coordfile=opencoord(&coord_filename,TRUE);
  if(coordfile==NULL)
    return EXIT_FAILURE;
  coords=newvec(Coord,numcoord(coordfile));
  if(coords==NULL)
  {
    printallocerr("coords");
    return EXIT_FAILURE;
  }
  config.ngridcell=numcoord(coordfile);
  getcellsizecoord(&header.cellsize_lon,&header.cellsize_lat,coordfile);
  config.resolution.lat=header.cellsize_lat;
  config.resolution.lon=header.cellsize_lon;
  for(j=0;j<numcoord(coordfile);j++)
    readcoord(coordfile,coords+j,&config.resolution); 
  closecoord(coordfile);
  header.nyear=0;
  header.ncell=config.ngridcell;
  header.scalar=scale;
  header.order=CELLYEAR;
  header.firstcell=0;
  header.datatype=(isfloat) ? LPJ_FLOAT : LPJ_SHORT; 
  file=fopen(outname,"wb");
  if(file==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",outname,strerror(errno));
    return EXIT_FAILURE;
  }
  fwriteheader(file,&header,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION);
  for(j=i+1;j<argc;j++)
  {
    /* printf("%s\n",argv[j]); */
    if(openclimate_netcdf(&climate,argv[j],var,units,&config))
    {
      fprintf(stderr,"Error opening '%s'.\n",argv[j]);
      return EXIT_FAILURE;
    }
    if(j==i+1)
    {
      header.firstyear=climate.firstyear;
      header.nbands=climate.isdaily ? NDAYYEAR : NMONTH;
      data=newvec(float,config.ngridcell*header.nbands);
      if(data==NULL)
      {
        printallocerr("data");
        return EXIT_FAILURE;
      }
      if(!isfloat)
      {
        s=newvec(short,config.ngridcell*header.nbands);
        if(s==NULL)
        {
          printallocerr("short");
          return EXIT_FAILURE;
        }
      }
    }
    else
    {
      if(climate.isdaily && header.nbands!=NDAYYEAR)
      {
        fprintf(stderr,"ERROR: daily output in '%s'.\n",argv[j]);
        return EXIT_FAILURE;
      }
      else if (!climate.isdaily && header.nbands!=NMONTH)
      {
        fprintf(stderr,"ERROR: monthly output in '%s'.\n",argv[j]);
        return EXIT_FAILURE;
      }
      if(climate.firstyear!=header.firstyear+header.nyear)
      {
        fprintf(stderr,"First year %d in '%s' not last year %d.\n",climate.firstyear,argv[j],header.firstyear+header.nyear);
        return EXIT_FAILURE;
      }
    }
    for(year=0;year<climate.nyear;year++)
    {
      if(readclimate2(&climate,data,year,coords,&config))
      {
        fprintf(stderr,"Error reading '%s' in year %d.\n",argv[j],year+climate.firstyear);
        return EXIT_FAILURE;
      }
      if(isfloat)
      {
        if(fwrite(data,sizeof(float),config.ngridcell*header.nbands,file)!=config.ngridcell*header.nbands)
        {
          fprintf(stderr,"Error writing data in '%s' in year %d.\n",outname,year+climate.firstyear);
          return EXIT_FAILURE;
        }
      }
      else
      { 
        for(k=0;k<config.ngridcell*header.nbands;k++)
        {
          if(round(data[k]/scale)<SHRT_MIN || round(data[k]/scale)>SHRT_MAX)
          {
            fprintf(stderr,"WARNING: Data overflow for cell %d ",k/header.nbands);
            fprintcoord(stderr,coords+k/header.nbands);
            fprintf(stderr,") at %s %d in %d.\n",(climate.isdaily) ? "day" : "month",(k % header.nbands)+1,climate.firstyear+year);
          } 
          s[k]=(short)round(data[k]/scale); 
        }
        if(fwrite(s,sizeof(short),config.ngridcell*header.nbands,file)!=config.ngridcell*header.nbands)
        {
          fprintf(stderr,"Error writing data in '%s' in year %d.\n",outname,year+climate.firstyear);
          return EXIT_FAILURE;
        }
      }
    } /* of for(year=0;...) */
    header.nyear+=climate.nyear;
    nc_close(climate.ncid);
  }
  rewind(file);
  fwriteheader(file,&header,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION);
  fclose(file);
  return EXIT_SUCCESS;
#else
  fprintf(stderr,"ERROR401: NetCDF is not supported in this version of %s.\n",argv[0]);
  return EXIT_FAILURE;
#endif
} /* of 'main' */

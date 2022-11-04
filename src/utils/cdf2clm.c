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
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef USE_UDUNITS
#define USAGE "Usage: %s [-v] [-units unit] [-var name] [-time name] [-o filename] [-scale factor] [-id s] [-version v] [-float] [-zero] [-json] gridfile netcdffile ...\n"
#else
#define USAGE "Usage: %s [-v] [-var name] [-o filename] [-scale factor] [-id s] [-version v] [-float] [-zero] [-json] gridfile netcdffile ...\n"
#endif

#if defined(USE_NETCDF) || defined(USE_NETCDF4)
#include <netcdf.h>

static void printindex(size_t i,Time time,size_t var_len)
{
  switch(time)
  {
    case DAY:
      fprintf(stderr,"at day %d",(int)(i/var_len)+1);
      if(var_len>1)
        fprintf(stderr," and item %d",(int)(i % var_len)+1);
      break;
    case MONTH:
      fprintf(stderr,"at month %d",(int)(i/var_len)+1);
      if(var_len>1)
        fprintf(stderr," and item %d",(int)(i % var_len)+1);
      break;
    case YEAR: case MISSING_TIME:
      if(var_len>1)
        fprintf(stderr,"at item %d",(int)(i % var_len)+1);
      break;
  }
} /* of 'printindex' */

static Bool readclimate2(Climatefile *file,    /* climate data file */
                         float data[],         /* pointer to data read in */
                         int year,             /* year */
                         const Coord coords[], /* coordinates */
                         Bool iszero,          /* set to zero if data is not found */
                         const Config *config  /* LPJ configuration */
                        )                      /* returns TRUE on error */
{
  int cell,rc;
  short *s;
  int *idata;
  float *f;
  double *d;
  int index,start;
  size_t i,size;
  size_t offsets[4];
  size_t counts[4];
  switch(file->time_step)
  {
    case DAY:
      size=NDAYYEAR;
      break;
    case MONTH:
      size=NMONTH;
      break;
    case YEAR: case MISSING_TIME:
      size=1;
      break;
  }
  if(file->time_step==MISSING_TIME)
    start=0;
  else
  {
    start=1;
    offsets[0]=year*size;
    if(isdaily(*file) && file->isleap)
      offsets[0]+=nleapyears(file->firstyear,year+file->firstyear);
    counts[0]=size;
  }
  if(file->var_len>1)
  {
    offsets[start]=0;
    counts[start]=file->var_len;
    index=start+1;
  }
  else
    index=start;
  offsets[index]=offsets[index+1]=0;
  counts[index]=file->nlat;
  counts[index+1]=file->nlon;
  switch(file->datatype)
  {
    case LPJ_FLOAT:
      f=newvec(float,size*file->var_len*file->nlon*file->nlat);
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
      break;
    case LPJ_DOUBLE:
      d=newvec(double,size*file->var_len*file->nlon*file->nlat);
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
      break;
    case LPJ_SHORT:
      s=newvec(short,size*file->var_len*file->nlon*file->nlat);
      if(s==NULL)
      {
        printallocerr("data");
        nc_close(file->ncid);
        return TRUE;
      }
      if((rc=nc_get_vara_short(file->ncid,file->varid,offsets,counts,s)))
      {
        free(s);
        fprintf(stderr,"ERROR421: Cannot read short data: %s.\n",
               nc_strerror(rc));
        nc_close(file->ncid);
        return TRUE;
      }
      break;
    case LPJ_INT:
      idata=newvec(int,size*file->var_len*file->nlon*file->nlat);
      if(idata==NULL)
      {
        printallocerr("data");
        nc_close(file->ncid);
        return TRUE;
      }
      if((rc=nc_get_vara_int(file->ncid,file->varid,offsets,counts,idata)))
      {
        free(idata);
        fprintf(stderr,"ERROR421: Cannot read int data: %s.\n",
               nc_strerror(rc));
        nc_close(file->ncid);
        return TRUE;
      }
      break;

    default:
      fprintf(stderr,"Datatype %s not supported.\n",typenames[file->datatype]);
      nc_close(file->ncid);
      return TRUE;
  }
  for(cell=0;cell<config->ngridcell;cell++)
  {
    if(file->offset)
      offsets[index]=file->offset-(int)((coords[cell].lat-file->lat_min)/file->lat_res+0.5);
    else
      offsets[index]=(int)((coords[cell].lat-file->lat_min)/file->lat_res+0.5);
    if(file->is360 && coords[cell].lon<0)
      offsets[index+1]=(int)((360+coords[cell].lon-file->lon_min)/file->lon_res+0.5);
    else
      offsets[index+1]=(int)((coords[cell].lon-file->lon_min)/file->lon_res+0.5);
    if(checkcoord(offsets+index,cell,coords+cell,file))
    {
      switch(file->datatype)
      {
        case LPJ_FLOAT:
          free(f);
          break;
        case LPJ_DOUBLE:
          free(d);
          break;
        case LPJ_INT:
          free(idata);
          break;
        case LPJ_SHORT:
          free(s);
          break;
      }
      nc_close(file->ncid);
      return TRUE;
    }
    for(i=0;i<size*file->var_len;i++)
    {
      switch(file->datatype)
      {
        case LPJ_FLOAT:
          if(f[file->nlon*(i*file->nlat+offsets[index])+offsets[index+1]]==file->missing_value.f)
          {
            fprintf(stderr,"ERROR423: Missing value for cell=%d (",cell);
            fprintcoord(stderr,coords+cell);
            fprintf(stderr,") ");
            printindex(i,file->time_step,file->var_len);
            fprintf(stderr,".\n");
            if(iszero)
              f[file->nlon*(i*file->nlat+offsets[index])+offsets[index+1]]=0;
            else
            {
              free(f);
              nc_close(file->ncid);
              return TRUE;
            }
          }
          else if(isnan(f[file->nlon*(i*file->nlat+offsets[index])+offsets[index+1]]))
          {
            fprintf(stderr,"ERROR423: Invalid value for cell=%d (",cell);
            fprintcoord(stderr,coords+cell);
            fprintf(stderr,") ");
            printindex(i,file->time_step,file->var_len);
            fprintf(stderr,".\n");
            if(iszero)
              f[file->nlon*(i*file->nlat+offsets[index])+offsets[index+1]]=0;
            else
            {
              free(f);
              nc_close(file->ncid);
              return TRUE;
            }
          }
          data[cell*size*file->var_len+i]=(float)(file->slope*f[file->nlon*(i*file->nlat+offsets[index])+offsets[index+1]]+file->intercept);
          break;
        case LPJ_DOUBLE:
          if(d[file->nlon*(i*file->nlat+offsets[index])+offsets[index+1]]==file->missing_value.d)
          {
            fprintf(stderr,"ERROR423: Missing value for cell=%d (",cell);
            fprintcoord(stderr,coords+cell);
            fprintf(stderr,") ");
            printindex(i,file->time_step,file->var_len);
            fprintf(stderr,".\n");
            if(iszero)
              d[file->nlon*(i*file->nlat+offsets[index])+offsets[index+1]]=0;
            else
            {
              free(d);
              nc_close(file->ncid);
              return TRUE;
            }
          }
          else if(isnan(d[file->nlon*(i*file->nlat+offsets[index])+offsets[index+1]]))
          {
            fprintf(stderr,"ERROR423: Invalid value for cell=%d (",cell);
            fprintcoord(stderr,coords+cell);
            fprintf(stderr,") ");
            printindex(i,file->time_step,file->var_len);
            fprintf(stderr,".\n");
            if(iszero)
              d[file->nlon*(i*file->nlat+offsets[index])+offsets[index+1]]=0;
            else
            {
              free(d);
              nc_close(file->ncid);
              return TRUE;
            }
          }
          data[cell*size*file->var_len+i]=(float)(file->slope*d[file->nlon*(i*file->nlat+offsets[index])+offsets[index+1]]+file->intercept);
          break;
        case LPJ_INT:
          if(idata[file->nlon*(i*file->nlat+offsets[index])+offsets[index+1]]==file->missing_value.i)
          {
            fprintf(stderr,"ERROR423: Missing value for cell=%d (",cell);
            fprintcoord(stderr,coords+cell);
            fprintf(stderr,") ");
            printindex(i,file->time_step,file->var_len);
            fprintf(stderr,".\n");
            if(iszero)
              idata[file->nlon*(i*file->nlat+offsets[index])+offsets[index+1]]=0;
            else
            {
              free(idata);
              nc_close(file->ncid);
              return TRUE;
            }
          }
          data[cell*size*file->var_len+i]=(float)(file->slope*idata[file->nlon*(i*file->nlat+offsets[index])+offsets[index+1]]+file->intercept);
          break;
        case LPJ_SHORT:
          if(s[file->nlon*(i*file->nlat+offsets[index])+offsets[index+1]]==file->missing_value.s)
          {
            fprintf(stderr,"ERROR423: Missing value for cell=%d (",cell);
            fprintcoord(stderr,coords+cell);
            fprintf(stderr,") ");
            printindex(i,file->time_step,file->var_len);
            fprintf(stderr,".\n");
            if(iszero)
              s[file->nlon*(i*file->nlat+offsets[index])+offsets[index+1]]=0;
            else
            {
              free(s);
              nc_close(file->ncid);
              return TRUE;
            }
          }
          data[cell*size*file->var_len+i]=(float)(file->slope*s[file->nlon*(i*file->nlat+offsets[index])+offsets[index+1]]+file->intercept);
          break;
      } /* of 'switch' */
    }
  }
  switch(file->datatype)
  {
    case LPJ_FLOAT:
      free(f);
      break;
    case LPJ_DOUBLE:
      free(d);
      break;
    case LPJ_INT:
      free(idata);
      break;
    case LPJ_SHORT:
      free(s);
      break;
  }
  return FALSE;
} /* of 'readclimate2' */

#endif

int main(int argc,char **argv)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  Coordfile coordfile;
  Climatefile climate;
  Config config;
  char *units,*var,*outname,*endptr,*time_name,*arglist,*descr;
  float scale,*data;
  Filename coord_filename;
  Coord *coords;
  Header header;
  FILE *file;
  int i,j,k,year,version;
  short *s;
  Bool isfloat,verbose,iszero,isjson;
  Time time;
  size_t var_len;
  char *id,*out_json;
  /* set default values */
  units=NULL;
  var=NULL;
  time_name=NULL;
  scale=1;
  isfloat=verbose=iszero=isjson=FALSE;
  outname="out.clm"; /* default file name for output */
  id=LPJ_CLIMATE_HEADER;
  version=LPJ_CLIMATE_VERSION;
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
      else if(!strcmp(argv[i],"-time"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Missing argument after option '-time'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        time_name=argv[++i];
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
      else if(!strcmp(argv[i],"-id"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Missing argument after option '-id'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        id=argv[++i];
      }
      else if(!strcmp(argv[i],"-float"))
        isfloat=TRUE;
      else if(!strcmp(argv[i],"-v"))
        verbose=TRUE;
      else if(!strcmp(argv[i],"-zero"))
        iszero=TRUE;
      else if(!strcmp(argv[i],"-json"))
        isjson=TRUE;
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
      else if(!strcmp(argv[i],"-version"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Missing argument after option '-version'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        version=strtol(argv[++i],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-version'.\n",argv[i]);
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
  header.timestep=1;
  header.datatype=(isfloat) ? LPJ_FLOAT : LPJ_SHORT;
  file=fopen(outname,"wb");
  if(file==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",outname,strerror(errno));
    return EXIT_FAILURE;
  }
  fwriteheader(file,&header,id,version);
  for(j=i+1;j<argc;j++)
  {
    if(openclimate_netcdf(&climate,argv[j],time_name,var,NULL,units,&config))
    {
      fprintf(stderr,"Error opening '%s'.\n",argv[j]);
      return EXIT_FAILURE;
    }
    if(verbose)
      printf("%s: ",argv[j]);
    if(j==i+1)
    {
      header.firstyear=climate.firstyear;
      switch(climate.time_step)
      {
        case DAY:
          header.nstep=NDAYYEAR;
          header.nbands=climate.var_len;
          if(verbose)
            printf("daily");
          break;
        case MONTH:
          header.nstep=NMONTH;
          header.nbands=climate.var_len;
          if(verbose)
            printf("monthly");
          break;
        case YEAR: case MISSING_TIME:
          header.nstep=1;
          header.nbands=climate.var_len;
          if(verbose)
            printf((climate.time_step==YEAR) ? "yearly" : "no");
          break;
      }
      if(verbose)
      {
        printf(" time step from %d",climate.firstyear);
        if(climate.slope!=1 || climate.intercept!=0)
          printf(", convert by %g*data%+g\n",climate.slope,climate.intercept);
        else
          printf("\n");
      }
      data=newvec(float,config.ngridcell*header.nbands*header.nstep);
      if(data==NULL)
      {
        printallocerr("data");
        return EXIT_FAILURE;
      }
      if(!isfloat)
      {
        s=newvec(short,config.ngridcell*header.nbands*header.nstep);
        if(s==NULL)
        {
          printallocerr("short");
          return EXIT_FAILURE;
        }
      }
    }
    else
    {
      if(time!=climate.time_step)
      {
        fprintf(stderr,"Time axis is different in '%s'.\n",argv[j]);
        return EXIT_FAILURE;
      }
      if (var_len!=climate.var_len)
      {
        fprintf(stderr,"Dimension is different in '%s'.\n",argv[j]);
        return EXIT_FAILURE;
      }
      if(climate.firstyear!=header.firstyear+header.nyear)
      {
        fprintf(stderr,"First year %d in '%s' not last year %d.\n",climate.firstyear,argv[j],header.firstyear+header.nyear);
        return EXIT_FAILURE;
      }
    }
    if(units==NULL)
      units=getattr_netcdf(&climate,"units");
    descr=getattr_netcdf(&climate,"long_name");
    time=climate.time_step;
    var_len=climate.var_len;
    for(year=0;year<climate.nyear;year++)
    {
      if(readclimate2(&climate,data,year,coords,iszero,&config))
      {
        fprintf(stderr,"Error reading '%s' in year %d.\n",argv[j],year+climate.firstyear);
        return EXIT_FAILURE;
      }
      if(isfloat)
      {
        if(fwrite(data,sizeof(float),config.ngridcell*header.nbands*header.nstep,file)!=config.ngridcell*header.nbands*header.nstep)
        {
          fprintf(stderr,"Error writing data in '%s' in year %d.\n",outname,year+climate.firstyear);
          return EXIT_FAILURE;
        }
      }
      else
      {
        for(k=0;k<config.ngridcell*header.nbands*header.nstep;k++)
        {
          if(round(data[k]/scale)<SHRT_MIN || round(data[k]/scale)>SHRT_MAX)
          {
            fprintf(stderr,"WARNING: Data overflow for cell %d ",k/header.nbands/header.nstep);
            fprintcoord(stderr,coords+k/header.nbands/header.nstep);
            fprintf(stderr,") at %s %d in %d.\n",isdaily(climate) ? "day" : "month",(k % (header.nbands*header.nstep))+1,climate.firstyear+year);
          }
          s[k]=(short)round(data[k]/scale);
        }
        if(fwrite(s,sizeof(short),config.ngridcell*header.nbands*header.nstep,file)!=config.ngridcell*header.nbands*header.nstep)
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
  if(version<4)
    header.nbands*=header.nstep;
  fwriteheader(file,&header,id,version);
  fclose(file);
  if(isjson)
  {
    out_json=malloc(strlen(outname)+strlen(".json")+1);
    if(out_json==NULL)
    {
      printallocerr("filename");
      return EXIT_FAILURE;
    }
    strcat(strcpy(out_json,outname),".json");
    arglist=catstrvec(argv,argc);
    file=fopen(out_json,"w");
    if(file==NULL)
    {
      printfcreateerr(out_json);
      return EXIT_FAILURE;
    }
    if(version<4)
      header.nbands/=header.nstep;
    fprintjson(file,outname,arglist,&header,NULL,NULL,NULL,0,var,units,descr,argv[i],LPJ_SHORT,CLM,id,FALSE,version);
    fclose(file);
  }
  return EXIT_SUCCESS;
#else
  fprintf(stderr,"ERROR401: NetCDF is not supported in this version of %s.\n",argv[0]);
  return EXIT_FAILURE;
#endif
} /* of 'main' */

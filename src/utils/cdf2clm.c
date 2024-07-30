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
#define USAGE "Usage: %s [-h] [-v] [-units unit] [-var name] [-time name] [-map name] [-o filename] [-scale factor] [-id s] [-version v] [-float] [-zero] [-json] gridfile netcdffile ...\n"
#else
#define USAGE "Usage: %s [-h] [-v] [-var name] [-time name] [-map name] [-o filename] [-scale factor] [-id s] [-version v] [-float] [-zero] [-json] gridfile netcdffile ...\n"
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
    default:
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
  short *s=NULL;
  int *idata=NULL;
  float *f=NULL;
  double *d=NULL;
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
    default:
      fprintf(stderr,"Time step of second not supported.\n");
      return TRUE;
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
        default:
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
        default:
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
    default:
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
  char *units,*var,*outname,*endptr,*time_name,*arglist,*long_name=NULL,*standard_name=NULL,*history=NULL,*source=NULL;
  char *map_name=NULL;
  Map *map=NULL;
  float scale,*data=NULL;
  Filename coord_filename;
  Coord *coords;
  Header header;
  FILE *file;
  int iarg,j,k,year,version;
  short *s=NULL;
  Bool isfloat,verbose,iszero,isjson;
  Time time=DAY;
  size_t var_len=0;
  char *id,*out_json;
  Attr *attrs=NULL;
  int n_attr=0,len;
  char name[NC_MAX_NAME];
  Filename grid_name;
  Type grid_type;
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
  for(iarg=1;iarg<argc;iarg++)
  {
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-h") || !strcmp(argv[iarg],"--help"))
      {
        printf("   cdf2clm (" __DATE__ ") Help\n"
               "   ==========================\n\n"
               "Convert NetCDF data into CLM input data for LPJmL version " LPJ_VERSION "\n\n");
        printf(USAGE
               "\nArguments:\n"
               "-h,--help     print this help text\n"
               "-v,--verbose  print name of NetCDF files\n"
               "-scale factor scaling factor for CLM files. Default is one\n"
#ifdef USE_UDUNITS
               "-units u      set unit to convert from NetCDF file\n"
#endif
               "-var name     variable  name in NetCDF file \n"
               "-time name    name of time in NetCDF file, default is 'time' or 'TIME'\n"
               "-id string    LPJ header string in clm file\n"
               "-version v    version of clm header, default is 3\n"
               "-float        write float values in clm file, default is short\n"
               "-zero         write zero values in clm file if data is not found\n"
               "-json         JSON metafile is created with suffix '.json'\n"
               "-o clmfile    filename of CLM data file written. Default is out.clm\n"
               "gridfile      filename of grid data file\n"
               "netcdffile    filename of NetCDF file(s) converted\n\n"
               "(C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file\n",
               argv[0]);
        return EXIT_SUCCESS;
      }
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
      else if(!strcmp(argv[iarg],"-time"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Missing argument after option '-time'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        time_name=argv[++iarg];
      }
#ifdef USE_UDUNITS
      else if(!strcmp(argv[iarg],"-units"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Missing argument after option '-units'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        units=argv[++iarg];
      }
#endif
      else if(!strcmp(argv[iarg],"-map"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Missing argument after option '-map'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        map_name=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-o"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Missing argument after option '-o'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        outname=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-id"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Missing argument after option '-id'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        id=argv[++iarg];
      }
      else if(!strcmp(argv[iarg],"-float"))
        isfloat=TRUE;
      else if(!strcmp(argv[iarg],"-v") || !strcmp(argv[iarg],"--verbose"))
        verbose=TRUE;
      else if(!strcmp(argv[iarg],"-zero"))
        iszero=TRUE;
      else if(!strcmp(argv[iarg],"-json"))
        isjson=TRUE;
      else if(!strcmp(argv[iarg],"-scale"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Missing argument after option '-scale'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        scale=(float)strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-scale'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
        if(scale==0)
        {
          fputs("Scale factor must not be zero.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-version"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Missing argument after option '-version'.\n"
                 USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        version=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-version'.\n",argv[iarg]);
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
  }
  if(argc<iarg+2)
  {
    fprintf(stderr,"Missing arguments.\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  if(isfloat && scale!=1)
  {
    fprintf(stderr,"Warning: Scaling set to %g but datatype is float, scaling set to 1.\n",
            scale);
    scale=1;
  }
  coord_filename.name=argv[iarg];
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
  grid_type=getcoordtype(coordfile);
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
  for(j=iarg+1;j<argc;j++)
  {
    if(openclimate_netcdf(&climate,argv[j],time_name,var,NULL,units,&config))
    {
      fprintf(stderr,"Error opening '%s'.\n",argv[j]);
      return EXIT_FAILURE;
    }
    if(verbose)
      printf("%s: ",argv[j]);
    if(j==iarg+1)
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
        case SECOND:
          fprintf(stderr,"Time step of second not supported in '%s'.\n",argv[j]);
          return EXIT_FAILURE;
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
      if(units==NULL)
        units=getattr_netcdf(climate.ncid,climate.varid,"units");
      if(var==NULL)
        var=getvarname_netcdf(&climate);
      long_name=getattr_netcdf(climate.ncid,climate.varid,"long_name");
      standard_name=getattr_netcdf(climate.ncid,climate.varid,"standard_name");
      history=getattr_netcdf(climate.ncid,NC_GLOBAL,"history");
      source=getattr_netcdf(climate.ncid,NC_GLOBAL,"source");
      if(map_name!=NULL)
      {
        map=readmap_netcdf(climate.ncid,map_name);
        if(map==NULL)
        {
          fprintf(stderr,"Map '%s' not found in '%s'.\n",map_name,argv[j]);
          map_name=NULL;
        }
        else
          map_name=BAND_NAMES;
      }
      if(nc_inq_natts(climate.ncid,&len))
        n_attr=0;
      else
      {
        attrs=newvec(Attr,len);
        if(attrs==NULL)
        {
          printallocerr("attrs");
          return EXIT_FAILURE;
        }
        n_attr=0;
        for(k=0;k<len;k++)
        {
          if(!nc_inq_attname(climate.ncid,NC_GLOBAL,k,name))
          {
            if(strcmp(name,"history") && strcmp(name,"source"))
            {
              attrs[n_attr].value=getattr_netcdf(climate.ncid,NC_GLOBAL,name);
              if(attrs[n_attr].value!=NULL)
                attrs[n_attr++].name=strdup(name);
            }
          }
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
    out_json=malloc(strlen(outname)+strlen(JSON_SUFFIX)+1);
    if(out_json==NULL)
    {
      printallocerr("filename");
      return EXIT_FAILURE;
    }
    strcat(strcpy(out_json,outname),JSON_SUFFIX);
    arglist=catstrvec(argv,argc);
    file=fopen(out_json,"w");
    if(file==NULL)
    {
      printfcreateerr(out_json);
      return EXIT_FAILURE;
    }
    if(version<4)
      header.nbands/=header.nstep;
    grid_name.name=argv[iarg];
    grid_name.fmt=CLM;
    fprintjson(file,outname,NULL,source,history,arglist,&header,map,map_name,attrs,n_attr,var,units,standard_name,long_name,&grid_name,grid_type,CLM,id,FALSE,version);
    fclose(file);
  }
  return EXIT_SUCCESS;
#else
  fprintf(stderr,"ERROR401: NetCDF is not supported in this version of %s.\n",argv[0]);
  return EXIT_FAILURE;
#endif
} /* of 'main' */

/**************************************************************************************/
/**                                                                                \n**/
/**                     a  r  r  2  c  l  m  .  c                                  \n**/
/**                                                                                \n**/
/**     Program converts 2-D float arrays into CLM format                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/
#include "lpj.h"

#define USAGE "Usage: arr2clm [-firstyear y] [-version v] [-float] [-scale s] [-id s] [-invalid i] [-r v] gridfile arrfile clmfile\n"

int main(int argc,char **argv)
{
  int iarg,i,version,count,nlon,nlat;
  Bool isfloat;
  Coordfile coordfile;
  Coord *coords;
  Coord resolution;
  Filename coord_filename;
  Header header;
  FILE *in,*out;
  float *data,value,invalid,replace;
  short svalue;
  char *id,*endptr;
  isfloat=FALSE;
  header.scalar=1;
  header.firstyear=1901;
  header.firstcell=0;
  header.nbands=1;
  header.order=CELLYEAR;
  id=LPJ_CLIMATE_HEADER;
  version=LPJ_CLIMATE_VERSION;
  invalid=replace=-9999;
  for(iarg=1;iarg<argc;iarg++)
  {
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-float"))
        isfloat=TRUE;
      else if(!strcmp(argv[iarg],"-scale"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-scale'.\n"
                  USAGE);
          return EXIT_FAILURE;
        }
        header.scalar=(float)strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-scale'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
        if(header.scalar==0)
        {
          fputs("Scale factor must not be zero.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-r"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-r'.\n"
                  USAGE);
          return EXIT_FAILURE;
        }
        replace=(float)strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-r'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-invalid"))
      {
        if(argc==iarg+1)
        {
          fprintf(stderr,"Error: Missing argument after option '-invalid'.\n"
                  USAGE);
          return EXIT_FAILURE;
        }
        replace=invalid=(float)strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-invalid'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-firstyear"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Error: Missing argument after option '-firstyear'.\n"
                  USAGE);
          return EXIT_FAILURE;
        }
        header.firstyear=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-firstyear'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-version"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Error: Missing argument after option '-version'.\n"
                  USAGE);
          return EXIT_FAILURE;
        }
        version=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Error: Invalid number '%s' for option '-version'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-id"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Missing argument after option '-id'.\n"
                 USAGE);
          return EXIT_FAILURE;
        }
        id=argv[++iarg];
      }
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n",argv[iarg]);
        fputs(USAGE,stderr);
      }
    }
    else
     break;
  }
  if(argc<iarg+3)
  {
    fprintf(stderr,"Missing arguments.\n"
            USAGE);
    return EXIT_FAILURE;
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
  getcellsizecoord(&header.cellsize_lon,&header.cellsize_lat,coordfile);
  resolution.lat=header.cellsize_lat;
  resolution.lon=header.cellsize_lon;
  nlon=(int)(360/resolution.lon);
  nlat=(int)(180/resolution.lat);
  header.ncell=numcoord(coordfile);
  for(i=0;i<numcoord(coordfile);i++)
    readcoord(coordfile,coords+i,&resolution);
  closecoord(coordfile);

  in=fopen(argv[iarg+1],"rb");
  if(in==NULL)
  {
    printfopenerr(argv[iarg+1]);
    return EXIT_FAILURE;
  }
  header.datatype=(isfloat) ? LPJ_FLOAT : LPJ_SHORT;
  header.nyear=0;
  out=fopen(argv[iarg+2],"wb");
  if(out==NULL)
  {
    printfcreateerr(argv[iarg+2]);
    return EXIT_FAILURE;
  }
  fwriteheader(out,&header,id,version);
  data=newvec(float,nlon*nlat);
  if(data==NULL)
  {
    printallocerr("data");
    return EXIT_FAILURE;
  }
  if(getfilesizep(in) % (sizeof(float)*nlat*nlon))
    fprintf(stderr,"Warning: File size of '%s' is not multiple of %d x %d.\n",
            argv[iarg+1],nlon,nlat);
  while(fread(data,sizeof(float),nlat*nlon,in)==nlat*nlon)
  {
    count=0;
    for(i=0;i<header.ncell;i++)
    {
      value=data[nlon*(int)((90-resolution.lat*0.5-coords[i].lat)/resolution.lat)+(int)((coords[i].lon+180-resolution.lon*0.5)/resolution.lon)];
      if(value==invalid)
      {
        value=replace;
        count++;
      }
      if(isfloat)
        fwrite(&value,sizeof(float),1,out);
      else
      {
        if(round(value/header.scalar)<SHRT_MIN || round(value/header.scalar)>SHRT_MAX)
         fprintf(stderr,"Warning: Data overflow %g for cell %d.\n ",value/header.scalar,i);
        svalue=(short)round(value/header.scalar);
        fwrite(&svalue,sizeof(short),1,out);
      }
    }
    if(count)
      fprintf(stderr,"Warning: %d invalid cells in year %d, set to %g.\n",
              count,header.firstyear+header.nyear,replace);
    header.nyear++;
  }
  printf("Number of years: %d\n",header.nyear);
  rewind(out);
  fwriteheader(out,&header,id,version);
  fclose(out);
  fclose(in);
  return EXIT_SUCCESS;
} /* of 'main' */

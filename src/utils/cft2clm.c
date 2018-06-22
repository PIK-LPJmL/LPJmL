/**************************************************************************************/
/**                                                                                \n**/
/**                  c  f  t  2  c  l  m  .  c                                     \n**/
/**                                                                                \n**/
/**     Program converts cftfile to LPJ *.clm file                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <sys/stat.h>
#include "lpj.h"

#define NBAND 26
#define NYEAR 306
#define FIRSTYEAR 1700 

#define CFT2CLM_VERSION "1.0.003"
#define USAGE "Usage: %s [-h] [-swap] [-nbands n] [-firstyear s] [-nyear y] [-lastyear e] cftfile outfile\n"

int main(int argc,char **argv)
{
  FILE *ifile,*ofile;
  short *data;
  int i,n,year,count,band;
  Header header;
  Bool swap;
  struct stat filestat;
  char *endptr;
  swap=FALSE;
  header.nyear=NYEAR;
  header.firstyear=FIRSTYEAR;
  header.order=CELLYEAR;
  header.firstcell=0;
  header.nbands=NBAND;
  header.cellsize_lon=header.cellsize_lat=0.5;
  header.datatype=LPJ_SHORT;
  header.scalar=0.001;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-h"))
      {
        printf("%s " CFT2CLM_VERSION " (" __DATE__ ") - adds header to cftfile for lpj C version\n",argv[0]);
        printf(USAGE
               "-h           print this help text\n" 
               "-swap        change byte order in cftfile\n"
               "-nbands n    number of bands, default is %d\n"
               "-firstyear s first year, default is %d\n"
               "-lastyear e  last year, default is %d\n"
               "-nyear y     number of years, default is %d\n"
               "cftfile      filename of input cft data file\n"
               "outfile      filename of output cft data file\n",argv[0],NBAND,FIRSTYEAR,FIRSTYEAR+NYEAR-1,NYEAR);
        return EXIT_SUCCESS;
      }
      else if(!strcmp(argv[i],"-swap"))
        swap=TRUE;
      else if(!strcmp(argv[i],"-nbands"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Missing argument after option '-nbands'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        header.nbands=strtol(argv[++i],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-nbands'.\n",argv[i]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[i],"-nyear"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Missing argument after option '-nyear'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        header.nyear=strtol(argv[++i],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-nyear'.\n",argv[i]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[i],"-firstyear"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Missing argument after option '-firstyear'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        header.firstyear=strtol(argv[++i],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-firstyear'.\n",argv[i]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[i],"-lastyear"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Missing argument after option '-lastyear'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        header.nyear=strtol(argv[++i],&endptr,10)-header.firstyear+1;
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-lastyear'.\n",argv[i]);
          return EXIT_FAILURE;
        }
      }
      else
      {
        fprintf(stderr,"Error: invalid option '%s'.\n",argv[i]);
        fprintf(stderr,USAGE,argv[0]);
        return EXIT_FAILURE;
      }
    }
    else 
      break;
  if(argc<i+2)
  {
    fprintf(stderr,"Filenames missing.\n");
    fprintf(stderr,USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  ifile=fopen(argv[i],"rb");
  if(ifile==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s\n",argv[i],strerror(errno));
    return EXIT_FAILURE;
  }
  fstat(fileno(ifile),&filestat);
  n=filestat.st_size/sizeof(short)/header.nyear;
  if(filestat.st_size % (sizeof(short)*header.nyear*header.nbands))
    fprintf(stderr,"Warning: File size is multiple of nbands * nyear\n"); 
  printf("Number of cells: %d\n",n/header.nbands);
  ofile=fopen(argv[i+1],"wb");
  if(ofile==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s\n",argv[i+1],strerror(errno));
    return EXIT_FAILURE;
  }
  header.ncell=n/header.nbands;
  fwriteheader(ofile,&header,LPJ_LANDUSE_HEADER,LPJ_LANDUSE_VERSION);

  data=(short *)malloc(n*sizeof(short));
  if(data==NULL)
  {
    printallocerr("data");
    return EXIT_FAILURE;
  }
  for(year=0;year<header.nyear;year++)
  {
    if(freadshort(data,n,swap,ifile)!=n)
    {
      fprintf(stderr,"Unexpected end of file in year %d.\n",year+header.firstyear);
      return EXIT_FAILURE;
    }
    count=0;
    for(band=0;band<n;band++)
      if(data[band]>0)
        count++;

    printf("year:%d count:%d\n",year,count);

    fwrite(data,sizeof(short),n,ofile);
  }
  fclose(ifile);
  free(data);
  fclose(ofile);
  return EXIT_SUCCESS;
} /* of 'main' */

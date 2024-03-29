/**************************************************************************************/
/**                                                                                \n**/
/**                      c  r  u  2  c  l  m  .  c                                 \n**/
/**                                                                                \n**/
/**     Converts CRU data files into LPJ climate data files                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#undef USE_MPI /* no MPI */

#include "lpj.h"

#define NYEAR 103  /* default value for number of years */
#define FIRSTYEAR 1901 /* default value for first year */
#define FIRSTCELL 0
#define NCELL 67420
#define USAGE    "Usage: cru2clm [-h] [-firstyear first] [-lastyear last]\n"\
                 "       [-nyear n] [-scale s] [-firstcell f] [-ncell n] [-nbands nb] [-swap]\n"\
                 "       [-yearcell] crufile clmfile\n"

typedef short Data[NMONTH];

int main(int argc,char **argv)
{
  FILE *file;
  Data *data;
  char *endptr;
  Header header;
  long int k;
  int i,j,n;
  Bool swap;
  header.nyear=NYEAR;
  header.firstyear=FIRSTYEAR;
  header.order=CELLYEAR;
  header.firstcell=FIRSTCELL;
  header.ncell=NCELL;
  header.nbands=NMONTH;
  header.cellsize_lon=header.cellsize_lat=0.5;
  header.datatype=LPJ_SHORT;
  header.scalar=1;
  swap=FALSE;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-h"))
      {
        printf("cru2clm (" __DATE__ ") - convert cru data files to\n"
               "       clm data files for LPJmL version " LPJ_VERSION "\n\n");
        printf(USAGE
               "\nArguments:\n"
               "-h               print this help text\n" 
               "-firstyear first first year in cru file (default is %d)\n"
               "-lastyear last   last year in cru file\n"
               "-nyear n         number of years in cru file (default is %d)\n"
               "-scale s         set scale to s, default is 1\n"
               "-firstcell       first cell in data file (default %d)\n"
               "-ncell           number of cells in data file (default is %d)\n"
               "-nbands          number of bands in data/output file (default is %d)\n"
               "-swap            change byte order in cru file\n"
               "-yearcell        does not revert order in cru file\n"
               "crufile          filename of cru data file\n"
               "clmfile          filename of clm data file\n\n"
               "(C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file\n",
               FIRSTYEAR,NYEAR,FIRSTCELL,NCELL,NMONTH);
        return EXIT_SUCCESS;
      }
      else if(!strcmp(argv[i],"-nyear"))
      {
        if(i==argc-1)
        {
          fputs("Argument missing after '-nyear' option.\n"
                USAGE,stderr);
          return EXIT_FAILURE;
        }
        header.nyear=strtol(argv[++i],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-nyear'.\n",
                  argv[i]);
          return EXIT_FAILURE;
        }
        if(header.nyear<1)
        {
          fprintf(stderr,"Number of years=%d must be greater than zero.\n",header.nyear);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[i],"-firstyear"))
      {
        if(i==argc-1)
        {
          fputs("Argument missing after '-firstyear' option.\n"
                USAGE,stderr);
          return EXIT_FAILURE;
        }
        header.firstyear=strtol(argv[++i],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-firstyear'.\n",
                  argv[i]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[i],"-lastyear"))
      {
        if(i==argc-1)
        {
          fputs("Argument missing after '-lastyear' option.\n"
                USAGE,stderr);
          return EXIT_FAILURE;
        }
        header.nyear=strtol(argv[++i],&endptr,10)-header.firstyear+1;
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-lastyear'.\n",
                  argv[i]);
          return EXIT_FAILURE;
        }
        if(header.nyear<1)
        {
          fprintf(stderr,"Last year=%d less than first year=%d.\n",header.firstyear+header.nyear-1,header.firstyear);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[i],"-scale"))
      {
        if(i==argc-1)
        {
          fputs("Argument missing after '-scale' option.\n"
                USAGE,stderr);
          return EXIT_FAILURE;
        }
        header.scalar=(float)strtod(argv[++i],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-scale'.\n",
                  argv[i]);
          return EXIT_FAILURE;
        }
        if(header.scalar<=0)
        {
          fprintf(stderr,"Scalar=%g must be greater than zero.\n",header.scalar);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[i],"-firstcell"))
      {
        if(i==argc-1)
        {
          fputs("Argument missing after '-firstcell' option.\n"
                USAGE,stderr);
          return EXIT_FAILURE;
        }
        header.firstcell=strtol(argv[++i],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-firstcell'.\n",
                  argv[i]);
          return EXIT_FAILURE;
        }
        if(header.firstcell<1)
        {
          fprintf(stderr,"First cell=%d must be greater than zero.\n",header.firstcell);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[i],"-ncell"))
      {
        if(i==argc-1)
        {
          fputs("Argument missing after '-ncell' option.\n"
                USAGE,stderr);
          return EXIT_FAILURE;
        }
        header.ncell=strtol(argv[++i],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-ncell'.\n",
                  argv[i]);
          return EXIT_FAILURE;
        }
        if(header.ncell<1)
        {
          fprintf(stderr,"Number of cells=%d must be greater than zero.\n",header.ncell);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[i],"-nbands"))
      {
        if(i==argc-1)
        {
          fputs("Argument missing after '-nbands' option.\n"
                USAGE,stderr);
          return EXIT_FAILURE;
        }
        header.nbands=strtol(argv[++i],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-nbands'.\n",
                  argv[i]);
          return EXIT_FAILURE;
        }
        if(header.nbands<1)
        {
          fprintf(stderr,"Number of bands=%d must be greater than zero.\n",header.nbands);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[i],"-swap"))
        swap=TRUE;
      else if(!strcmp(argv[i],"-yearcell"))
        header.order=YEARCELL;
      else
      {
        fprintf(stderr,"Error: invalid option '%s'.\n",argv[i]);
        fprintf(stderr,USAGE);
        return EXIT_FAILURE;
      }
    }
    else 
      break;
  if(argc<i+2)
  {
    fprintf(stderr,"Filenames missing.\n");
    fprintf(stderr,USAGE);
    return EXIT_FAILURE;
  }
  file=fopen(argv[i],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s\n",argv[i],strerror(errno));
    return EXIT_FAILURE;
  }
  n=getfilesizep(file)/header.nyear/sizeof(Data);
  printf("Number of cells: %d\n",n);
  data=(Data *)malloc(n*sizeof(Data)*header.nyear);
  if(data==NULL)
  {
    fprintf(stderr,"Error allocating memory.\n");
    return EXIT_FAILURE;
  }
  if(fread(data,sizeof(Data),n*header.nyear,file)!=n*header.nyear)
  {
    fprintf(stderr,"Error reading data from '%s'.\n",argv[i]);
    return EXIT_FAILURE;
  }
  if(swap)
    for(k=0;k<n*header.nyear;k++)
      for(j=0;j<NMONTH;j++)
        data[k][j]=swapshort(data[k][j]); 
  fclose(file);
  file=fopen(argv[i+1],"wb");
  if(file==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s\n",argv[i+1],strerror(errno));
    return EXIT_FAILURE;
  }
  header.ncell=n;
  fwriteheader(file,&header,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION);
  if(header.order==CELLYEAR)
  {
    for(i=0;i<header.nyear;i++)
      for(j=0;j<n;j++)
        fwrite(data+j*header.nyear+i,sizeof(Data),1,file);
  }
  else
    fwrite(data,sizeof(Data),header.nyear*n,file);
  free(data);
  fclose(file);
  return EXIT_SUCCESS;
} /* of 'main' */

/**************************************************************************************/
/**                                                                                \n**/
/**                  g  r  i  d  2  c  l  m  .  c                                  \n**/
/**                                                                                \n**/
/**     Converts grid data files into LPJ clm data files                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#undef USE_MPI
#include "lpj.h"
#include <sys/stat.h>

#define GRID2CLM_VERSION "1.0.002"
#define USAGE "Usage: %s [-h] [-swap] [-scale s] [-cellsize c] gridfile outfile\n"

int main(int argc,char **argv)
{
  FILE *file;
  short *data;
  int i,n;
  Header header;
  Bool swap;
  struct stat filestat;
  char *endptr;
  swap=FALSE;
  header.nyear=0;
  header.firstyear=0;
  header.order=0;
  header.firstcell=0;
  header.nbands=2;
  header.cellsize_lon=header.cellsize_lat=0.5;
  header.datatype=LPJ_SHORT;
  header.scalar=0.01;

  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-h"))
      {
        printf("%s " GRID2CLM_VERSION " (" __DATE__ ") - adds header to gridfile for LPJmL C version\n",argv[0]);
        printf(USAGE
               "-h             print this help text\n" 
               "-swap          change byte order in gridfile\n"
               "-scale s       set scale factor, default is %g\n"
               "-cellsize c    set cell size, default is %g\n"
               "gridfile       filename of binary grid data file\n"
               "outfile        filename of CLM grid data file\n",
               argv[0],header.scalar,header.cellsize_lon);
        return EXIT_SUCCESS;
      }
      else if(!strcmp(argv[i],"-swap"))
        swap=TRUE;
      else if(!strcmp(argv[i],"-scale"))
      {
        if(i==argc-1)
        {
          fprintf(stderr,"Argument missing after '-scale' option.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        header.scalar=(float)strtod(argv[++i],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for '-scale' option.\n",argv[i]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[i],"-cellsize"))
      {
        if(i==argc-1)
        {
          fprintf(stderr,"Argument missing after '-cellsize' option.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        header.cellsize_lon=header.cellsize_lat=(float)strtod(argv[++i],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for '-cellsize' option.\n",argv[i]);
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
    fprintf(stderr,"Filenames missing.\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  file=fopen(argv[i],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s\n",argv[i],strerror(errno));
    return EXIT_FAILURE;
  }
  fstat(fileno(file),&filestat);
  n=filestat.st_size/sizeof(short);
  if(filestat.st_size % (sizeof(short)*2))
    fprintf(stderr,"Warning: file size is not multiple of 2*short.\n"); 
  printf("Number of cells: %d\n",n/2);
  data=(short *)malloc(n*sizeof(short));
  if(data==NULL)
  {
    fclose(file);
    printallocerr("data");
    return EXIT_FAILURE;
  }
  freadshort(data,n,swap,file);
  fclose(file);
  file=fopen(argv[i+1],"wb");
  if(file==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s\n",argv[i+1],strerror(errno));
    return EXIT_FAILURE;
  }
  header.ncell=n/2;
  fwriteheader(file,&header,LPJGRID_HEADER,LPJGRID_VERSION);
  if(fwrite(data,sizeof(short),n,file)!=n)
  {
    fprintf(stderr,"Error writing data to '%s': %s.\n",
            argv[i+1],strerror(errno));
    return EXIT_FAILURE;
  }
  free(data);
  fclose(file);
  return EXIT_SUCCESS;
} /* of 'main' */

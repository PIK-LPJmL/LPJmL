/**************************************************************************************/
/**                                                                                \n**/
/**                      t  x  t  2  g  r  i  d  .  c                              \n**/
/**                                                                                \n**/
/**     Program converts text data to grid clm file format. Data must be           \n**/
/**     in the following format:                                                   \n**/
/**                                                                                \n**/
/**     Id,Lon,Lat,X_CRU,Y_CRU                                                     \n**/
/**     1,-179.75,-16.25,1,148                                                     \n**/
/**     2,..                                                                       \n**/
/**     or in a format specified by the -fmt option                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define TXT2GRID_VERSION "1.0.003"
#define USAGE "Usage: txt2grid [-h] [-fmt s] [-skip n] [-cellsize size] [-float] gridfile clmfile\n"

int main(int argc,char **argv)
{
  FILE *file,*gridfile;
  const char *fmt;
  Coord grid;
  String line;
  int i,nskip,n;
  struct
  {
    float lon,lat;
  } coord;
  Header header;
  char *endptr;
  Bool isfloat;
  fmt="%*d,%f,%f,%*d,%*d";
  nskip=1;
  header.cellsize_lon=header.cellsize_lat=0.5;
  isfloat=FALSE;
  for(i=1;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-h"))
      {
        printf("txt2grid " TXT2GRID_VERSION " (" __DATE__ ") - convert text file to\n"
             "         clm grid file for lpj C version\n\n"
             USAGE
             "\nArguments:\n"
             "-h             print this help text\n" 
             "-fmt s         format string for text input, default is '%s'\n"
             "-cellsize size cell size, default is %g\n"
             "-float         write float data, default is short\n"
             "-skip n        skip first n lines, default is one\n" 
             "gridfile       filename of grid text file\n"
             "clmfile        filename of clm data file\n",fmt,header.cellsize_lon);
        return EXIT_SUCCESS;
      }
      if(!strcmp(argv[i],"-fmt"))
      {
        if(i==argc-1)
        {
           fputs("Argument missing after '-fmt' option.\n"
                 USAGE,stderr);
           return EXIT_FAILURE;
        }
        fmt=argv[++i];
      }
      else if(!strcmp(argv[i],"-float"))
        isfloat=TRUE;
      else if(!strcmp(argv[i],"-cellsize"))
      {
        if(i==argc-1)
        {
           fputs("Argument missing after '-cellsize' option.\n"
                 USAGE,stderr);
           return EXIT_FAILURE;
        }
        header.cellsize_lon=header.cellsize_lat=(float)strtod(argv[++i],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for '-cellsize' option.\n",argv[i]);
          return EXIT_FAILURE;
        }
      }

      else if(!strcmp(argv[i],"-skip"))
      {
        if(i==argc-1)
        {
           fputs("Argument missing after '-skip' option.\n"
                 USAGE,stderr);
           return EXIT_FAILURE;
        }
        nskip=strtol(argv[++i],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for '-skip' option.\n",argv[i]);
          return EXIT_FAILURE;
        }
      }
      else
      {
        fprintf(stderr,"Error: invalid option '%s'.\n"
                USAGE,argv[i]);
        return EXIT_FAILURE;
      }
    }
    else 
      break;
  if(argc<i+2)
  {
    fputs("Filenames missing.\n"
          USAGE,stderr);
    return EXIT_FAILURE;
  }
  file=fopen(argv[i],"r");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[i],strerror(errno));
    return EXIT_FAILURE;
  }
  for(n=0;n<nskip;n++)
    if(fgets(line,STRING_LEN,file)==NULL)
    {
      fprintf(stderr,"Error skipping file header in '%s'.\n",argv[i]);
      return EXIT_FAILURE;
    }
  header.ncell=0;
  header.nbands=2;
  header.order=CELLYEAR;
  header.firstcell=0;
  header.firstyear=0;
  header.nyear=1;
  if(isfloat)
  {
   header.scalar=1;
   header.datatype=LPJ_FLOAT;
  }
  else
  {
    header.scalar=0.01;
    header.datatype=LPJ_SHORT;
  }
  gridfile=fopen(argv[i+1],"wb");
  if(gridfile==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[i+1],strerror(errno));
    return EXIT_FAILURE;
  }
  fwriteheader(gridfile,&header,LPJGRID_HEADER,LPJGRID_VERSION);
  while(fscanf(file,fmt,&coord.lon,&coord.lat)==2)
  {
    if(isfloat)
      fwrite(&coord,sizeof(coord),1,gridfile);
    else
    {
      grid.lon=coord.lon;
      grid.lat=coord.lat;
      writecoord(gridfile,&grid);
    }
    header.ncell++;
  }
  if(header.ncell==0)
    fprintf(stderr,"Error: No grid cells written to '%s'.\n",argv[i+1]);
  else
    printf("Number of grid cells: %d\n",header.ncell);
  rewind(gridfile);
  fwriteheader(gridfile,&header,LPJGRID_HEADER,LPJGRID_VERSION);
  fclose(gridfile);
  fclose(file);
  return EXIT_SUCCESS;
} /* of 'main' */

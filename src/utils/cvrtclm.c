/**************************************************************************************/
/**                                                                                \n**/
/**                       c  v  r  t  c  l  m  .  c                                \n**/
/**                                                                                \n**/
/**     Program converts CLM file to latest version 3                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include <sys/stat.h>

#define USAGE "Usage: %s [-scale s] [-type {byte|short|int|float|double}] [-cellsize size] infile outfile\n"

#define BUFSIZE (1024*1024) /* size of read buffer */

int main(int argc,char **argv)
{
  FILE *infile,*outfile;
  Header header;
  int version;
  String id;
  const char *progname;
  char *endptr;
  int index;
  size_t i,size;
  struct stat filestat;
  void *buffer;
  Type datatype;
  float scalar,cellsize;
  Bool swap;
  /* set default values */
  datatype=LPJ_SHORT;
  scalar=1;
  cellsize=0.5;
  progname=strippath(argv[0]);
  for(index=1;index<argc;index++)
    if(argv[index][0]=='-')
    {
      if(!strcmp(argv[index],"-scale"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-scale'.\n");
          return EXIT_FAILURE;
        }
        scalar=(float)strtod(argv[++index],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-scale'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(scalar<=0)
        {
          fputs("Scale factor less than or equal to zero.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-type"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-type'.\n");
          return EXIT_FAILURE;
        }
        i=findstr(argv[++index],typenames,5);
        if(i==NOT_FOUND)
        {
          fprintf(stderr,"Invalid argument '%s' for option '-type'.\n"
                  USAGE,argv[index],progname);
          return EXIT_FAILURE;
        }
        datatype=(Type)i;
      }
      else if(!strcmp(argv[index],"-cellsize"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-cellsize'.\n");
          return EXIT_FAILURE;
        }
        cellsize=(float)strtod(argv[++index],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-cellsize'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(cellsize<=0)
        {
          fputs("Cell size less than or equal to zero.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[index],progname);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<index+2)
  {
    fprintf(stderr,"Argument(s) missing.\n"
            USAGE,progname);
    return EXIT_FAILURE;
  }
  if(!strcmp(argv[index],argv[index+1]))
  {
    fputs("Error: source and destination filename are the same.\n",stderr);
    return EXIT_FAILURE;
  }
  infile=fopen(argv[index],"rb");
  if(infile==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[index],strerror(errno));
    return EXIT_FAILURE;
  }
  version=READ_VERSION;
  if(freadanyheader(infile,&header,&swap,id,&version))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",argv[index]);
    return EXIT_FAILURE;
  }
  if(version==1)
  {
    header.scalar=scalar;
    header.cellsize_lon=header.cellsize_lat=cellsize;
  }
  if(version<3)
    header.datatype=datatype;
  fstat(fileno(infile),&filestat);
  if(filestat.st_size!=(long long)header.ncell*header.nbands*header.nyear*typesizes[header.datatype]+headersize(id,version))
  {
     fprintf(stderr,"Error: File size of '%s' is not multiple of nyear*nbands*ncell.\n",argv[index]);
     fclose(infile);
     return EXIT_FAILURE;
  }
  outfile=fopen(argv[index+1],"wb");
  if(outfile==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[index+1],strerror(errno));
    return EXIT_FAILURE;
  }
  fwriteheader(outfile,&header,id,3);
  buffer=malloc(BUFSIZE);
  if(buffer==NULL)
  {
    printallocerr("buffer");
    return EXIT_FAILURE;
  }
  size=filestat.st_size-headersize(id,version);
  for(i=0;i<size / BUFSIZE;i++)
  {
    switch(typesizes[header.datatype])
    {
      case 2:
        freadshort(buffer,BUFSIZE/2,swap,infile);
        break;
      case 4:
        freadint(buffer,BUFSIZE/4,swap,infile);
        break;
      case 8:
        freadlong(buffer,BUFSIZE/8,swap,infile);
        break;
      default:
        fread(buffer,1,BUFSIZE,infile);
    }
    if(fwrite(buffer,1,BUFSIZE,outfile)!=BUFSIZE)
    {
      fprintf(stderr,"Error writing data in '%s'.\n",argv[index+1]);
      return EXIT_FAILURE;
    }
  }
  if(size % BUFSIZE>0)
  {
    switch(typesizes[header.datatype])
    {
      case 2:
        freadshort(buffer,(size % BUFSIZE)/2,swap,infile);
        break;
      case 4:
        freadint(buffer,(size % BUFSIZE)/4,swap,infile);
        break;
      case 8:
        freadlong(buffer,(size % BUFSIZE)/8,swap,infile);
        break;
      default:
        fread(buffer,1,size % BUFSIZE,infile);
    }
    if(fwrite(buffer,1,size % BUFSIZE,outfile)!=size % BUFSIZE)
    {
      fprintf(stderr,"Error writing data in '%s'.\n",argv[index+1]);
      return EXIT_FAILURE;
    }
  }
  free(buffer);
  fclose(infile);
  fclose(outfile);
  return EXIT_SUCCESS;
} /* of 'main' */

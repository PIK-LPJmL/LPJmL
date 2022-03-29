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

#define USAGE "Usage: %s [-4] [-scale s] [-type {byte|short|int|float|double}] [-timestep n] [-cellsize size] [-swapnstep] infile outfile\n"

#define BUFSIZE (1024*1024) /* size of read buffer */

int main(int argc,char **argv)
{
  FILE *infile,*outfile;
  Header header;
  int version,new_version,timestep;
  String id;
  const char *progname;
  char *endptr;
  int iarg,*index;
  size_t i,size;
  long long filesize;
  void *buffer;
  Type datatype;
  float scalar,cellsize;
  Bool swap,swapnstep;
  /* set default values */
  datatype=LPJ_SHORT;
  scalar=1;
  cellsize=0.5;
  new_version=3;
  swapnstep=FALSE;
  timestep=1;
  progname=strippath(argv[0]);
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-scale"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-scale'.\n");
          return EXIT_FAILURE;
        }
        scalar=(float)strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-scale'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
        if(scalar<=0)
        {
          fputs("Scale factor less than or equal to zero.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-4"))
        new_version=4;
      else if(!strcmp(argv[iarg],"-swapnstep"))
        swapnstep=TRUE;
      else if(!strcmp(argv[iarg],"-type"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-type'.\n");
          return EXIT_FAILURE;
        }
        i=findstr(argv[++iarg],typenames,5);
        if(i==NOT_FOUND)
        {
          fprintf(stderr,"Invalid argument '%s' for option '-type'.\n"
                  USAGE,argv[iarg],progname);
          return EXIT_FAILURE;
        }
        datatype=(Type)i;
      }
      else if(!strcmp(argv[iarg],"-cellsize"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-cellsize'.\n");
          return EXIT_FAILURE;
        }
        cellsize=(float)strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-cellsize'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
        if(cellsize<=0)
        {
          fputs("Cell size less than or equal to zero.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-timestep"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-timestep'.\n");
          return EXIT_FAILURE;
        }
        timestep=(float)strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-timestep'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
        if(timestep<=0)
        {
          fputs("Time step less than or equal to zero.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[iarg],progname);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  if(argc<iarg+2)
  {
    fprintf(stderr,"Argument(s) missing.\n"
            USAGE,progname);
    return EXIT_FAILURE;
  }
  if(!strcmp(argv[iarg],argv[iarg+1]))
  {
    fputs("Error: source and destination filename are the same.\n",stderr);
    return EXIT_FAILURE;
  }
  infile=fopen(argv[iarg],"rb");
  if(infile==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[iarg],strerror(errno));
    return EXIT_FAILURE;
  }
  version=READ_VERSION;
  if(freadanyheader(infile,&header,&swap,id,&version,TRUE))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",argv[iarg]);
    return EXIT_FAILURE;
  }
  if(version==1)
  {
    header.scalar=scalar;
    header.cellsize_lon=header.cellsize_lat=cellsize;
  }
  if(version<3)
    header.datatype=datatype;
  if(new_version==4 && swapnstep)
  {
     header.nstep=header.nbands;
     header.nbands=1;
  }
  header.timestep=timestep;
  filesize=getfilesizep(infile);
  if((header.order==CELLINDEX && filesize!=sizeof(int)*header.ncell+(long long)header.ncell*header.nbands*header.nstep*header.nyear*typesizes[header.datatype]+headersize(id,version)) ||
     (header.order!=CELLINDEX && filesize!=(long long)header.ncell*header.nbands*header.nstep*header.nyear*typesizes[header.datatype]+headersize(id,version)))
  {
     fprintf(stderr,"Error: File size of '%s' does not match nyear*nbands*nstep*ncell.\n",argv[iarg]);
     fclose(infile);
     return EXIT_FAILURE;
  }
  outfile=fopen(argv[iarg+1],"wb");
  if(outfile==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[iarg+1],strerror(errno));
    return EXIT_FAILURE;
  }
  fwriteheader(outfile,&header,id,new_version);
  buffer=malloc(BUFSIZE);
  if(buffer==NULL)
  {
    printallocerr("buffer");
    return EXIT_FAILURE;
  }
  size=filesize-headersize(id,version);
  if(header.order==CELLINDEX)
  {
    index=newvec(int,header.ncell);
    if(index==NULL)
    {
      printallocerr("index");
      return EXIT_FAILURE;
    }
    size-=sizeof(int)*header.ncell;
    freadint(index,header.ncell,swap,infile);
    if(fwrite(index,sizeof(int),header.ncell,outfile)!=header.ncell)
    {
      fprintf(stderr,"Error writing data in '%s'.\n",argv[iarg+1]);
      return EXIT_FAILURE;
    }
  }
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
      fprintf(stderr,"Error writing data in '%s'.\n",argv[iarg+1]);
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
      fprintf(stderr,"Error writing data in '%s'.\n",argv[iarg+1]);
      return EXIT_FAILURE;
    }
  }
  free(buffer);
  fclose(infile);
  fclose(outfile);
  return EXIT_SUCCESS;
} /* of 'main' */

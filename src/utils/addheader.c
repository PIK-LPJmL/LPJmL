/**************************************************************************************/
/**                                                                                \n**/
/**                       a  d  d  h  e  a  d  e  r  .  c                          \n**/
/**                                                                                \n**/
/**     Program adds CLM header to binary file                                     \n**/
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

#define USAGE "Usage: %s [-swap] [-nyear n] [-firstyear n] [-lastyear n] [-ncell n]\n       [-firstcell n] [-nbands n] [-order n] [-version n] [-cellsize s]\n       [-scale s] [-id s] [-type {byte|short|int|float|double}] binfile clmfile\n"

#define BUFSIZE (1024*1024) /* size of read buffer */

int main(int argc,char **argv)
{
  FILE *infile,*outfile;
  Header header;
  int version;
  char *id;
  const char *progname;
  char *endptr;
  int i,index;
  size_t len;
  struct stat filestat;
  void *buffer;
  Bool swap;
  progname=strippath(argv[0]);
  /* set default values for header */
  version=LPJ_CLIMATE_VERSION; 
  id=LPJ_CLIMATE_HEADER;
  header.firstyear=1901;
  header.ncell=67420;
  header.firstcell=0;
  header.nyear=109;
  header.nbands=12;
  header.order=CELLYEAR;
  header.scalar=1;
  header.datatype=LPJ_SHORT;
  len=typesizes[header.datatype];
  header.cellsize_lon=header.cellsize_lat=0.5;
  swap=FALSE;
  for(index=1;index<argc;index++)
    if(argv[index][0]=='-')
    {
      if(!strcmp(argv[index],"-firstyear"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-firstyear'.\n");
          return EXIT_FAILURE;
        }
        header.firstyear=strtol(argv[++index],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-firstyear'.\n",argv[index]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-lastyear"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-lastyear'.\n");
          return EXIT_FAILURE;
        }
        header.nyear=strtol(argv[++index],&endptr,10)-header.firstyear+1;
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-lastyear'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(header.nyear<=0)
        {
          fprintf(stderr,"Invalid value %d for last year less than first year %d.\n",header.firstyear-1+header.nyear,header.firstyear);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-nyear"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-nyear'.\n");
          return EXIT_FAILURE;
        }
        header.nyear=strtol(argv[++index],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-nyear'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(header.nyear<=0)
        {
          fputs("Number of year less than one.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-ncell"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-ncell'.\n");
          return EXIT_FAILURE;
        }
        header.ncell=strtol(argv[++index],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-ncell'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(header.ncell<=0)
        {
          fputs("Number of cells less than one.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-firstcell"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-firstcell'.\n");
          return EXIT_FAILURE;
        }
        header.firstcell=strtol(argv[++index],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-firstcell'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(header.nyear<=0)
        {
          fputs("Number of years less than one.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-nbands"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-nbands'.\n");
          return EXIT_FAILURE;
        }
        header.nbands=strtol(argv[++index],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-nbands'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(header.nbands<=0)
        {
          fputs("Number of bands less than one.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-cellsize"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-cellsize'.\n");
          return EXIT_FAILURE;
        }
        header.cellsize_lon=header.cellsize_lat=(float)strtod(argv[++index],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-cellsize'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(header.cellsize_lon<=0)
        {
          fputs("Cell size less than or equal to zero.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-scale"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-scale'.\n");
          return EXIT_FAILURE;
        }
        header.scalar=(float)strtod(argv[++index],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-scale'.\n",argv[index]);
          return EXIT_FAILURE;
        }
        if(header.scalar<=0)
        {
          fputs("Scale factor less than or equal to zero.\n",stderr);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-order"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-order'.\n");
          return EXIT_FAILURE;
        }
        header.order=strtol(argv[++index],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-order'.\n",argv[index]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[index],"-version"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-version'.\n");
          return EXIT_FAILURE;
        }
        version=strtol(argv[++index],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-version'.\n",argv[index]);
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
        header.datatype=(Type)i;
        len=typesizes[header.datatype];
      }
      else if(!strcmp(argv[index],"-id"))
      {
        if(index==argc-1)
        {
          fprintf(stderr,"Argument missing for option '-id'.\n");
          return EXIT_FAILURE;
        }
        id=argv[++index];
        if(strncmp(id,"LPJ",3))
          fputs("Warning: Header string does not start with 'LPJ'.\n",stderr);
      }
      else if(!strcmp(argv[index],"-swap"))
        swap=TRUE;
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
  fstat(fileno(infile),&filestat);
  if(filestat.st_size!=(long long)header.ncell*header.nbands*header.nyear*len)
  {
     fprintf(stderr,"Error: File size of '%s' is not multiple of nyear*nbands*ncell*%d.\n",argv[index],(int)len);
     fclose(infile);
     return EXIT_FAILURE;
  }
  outfile=fopen(argv[index+1],"wb");
  if(outfile==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s.\n",argv[index+1],strerror(errno));
    return EXIT_FAILURE;
  }
  fwriteheader(outfile,&header,id,version);
  buffer=malloc(BUFSIZE);
  if(buffer==NULL)
  {
    printallocerr("buffer");
    return EXIT_FAILURE;
  }
  for(i=0;i<filestat.st_size / BUFSIZE;i++)
  {
    switch(len)
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
  if(filestat.st_size % BUFSIZE>0)
  {
    switch(len)
    {
      case 2:
        freadshort(buffer,(filestat.st_size % BUFSIZE)/2,swap,infile);
        break;
      case 4:
        freadint(buffer,(filestat.st_size % BUFSIZE)/4,swap,infile);
        break;
      case 8:
        freadlong(buffer,(filestat.st_size % BUFSIZE)/8,swap,infile);
        break;
      default:
        fread(buffer,1,filestat.st_size % BUFSIZE,infile);
    }
    if(fwrite(buffer,1,filestat.st_size % BUFSIZE,outfile)!=filestat.st_size % BUFSIZE)
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

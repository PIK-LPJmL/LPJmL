/**************************************************************************************/
/**                                                                                \n**/
/**                 c  u  t  c  l  m  .  c                                         \n**/
/**                                                                                \n**/
/**     Program cuts input from CLM file with new start year                       \n**/
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

#define USAGE "Usage: cutclm [-longheader] [-end] year src.clm dst.clm\n"

int main(int argc,char **argv)
{
  FILE *file,*out;
  char *endptr;
  Header header;
  Bool swap,istail;
  int version,year,index,idata,i;
  long long size,ldata;
  struct stat filestat;
  short sdata;
  Byte bdata;
  String id;
  istail=FALSE;
  version=READ_VERSION;
  for(index=1;index<argc;index++)
  {
    if(argv[index][0]=='-')
    {
      if(!strcmp(argv[index],"-end"))
        istail=TRUE;
      else if(!strcmp(argv[index],"-longheader"))
        version=2;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n"
                USAGE,argv[index]);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  }
  if(argc<index+3)
  {
    fprintf(stderr,"Argument(s) missing.\n"
            USAGE);
    return EXIT_FAILURE;
  }
  year=strtol(argv[index],&endptr,10);
  if(*endptr!='\0')
  {
    fprintf(stderr,"Invalid number '%s' for year.\n",argv[index]);
    return EXIT_FAILURE;
  }
  file=fopen(argv[index+1],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s\n",argv[index+1],strerror(errno));
    return EXIT_FAILURE;
  }
  if(freadanyheader(file,&header,&swap,id,&version))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",
            argv[index+1]);
    return EXIT_FAILURE;
  }
  if(version==3)
    size=typesizes[header.datatype];
  else
  {
    fstat(fileno(file),&filestat);
    size=(filestat.st_size-headersize(id,version))/header.ncell/header.nbands/header.nyear;
    printf("Size of data: %Ld bytes\n",size);
    if(size!=1 && size!=2 && size!=4 && size!=8)
    {
      fprintf(stderr,"Invalid size of data=%Ld.\n",size);
      return EXIT_FAILURE;
    }
  }
  if(year<header.firstyear || year>=header.firstyear+header.nyear)
  {
    fprintf(stderr,"Invalid year %d, must be in [%d,%d].\n",year,header.firstyear,header.firstyear+header.nyear-1);
    return EXIT_FAILURE;
  }

  out=fopen(argv[index+2],"wb");
  if(out==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s\n",argv[index+2],strerror(errno));
    return EXIT_FAILURE;
  }
  if(istail)
    header.nyear=year-header.firstyear+1;
  else
  {
    if(fseek(file,size*(year-header.firstyear)*header.nbands*header.ncell,SEEK_CUR))
    {
      fprintf(stderr,"Error seeking in '%s' to year %d.\n",argv[index+1],year);
      return EXIT_FAILURE;
    }
    header.nyear-=year-header.firstyear;
    header.firstyear=year;
  }
  fwriteheader(out,&header,id,version);
  switch(size)
  {
    case 1:
      for(i=0;i<header.nyear*header.nbands*header.ncell;i++)
      {
        if(fread(&bdata,1,1,file)!=1)
        {
          fprintf(stderr,"Error reading input from '%s'.\n",argv[index+1]);
          return EXIT_FAILURE;
        }
        fwrite(&bdata,1,1,out);
      }
      break;
    case 2:
      for(i=0;i<header.nyear*header.nbands*header.ncell;i++)
      {
        if(freadshort(&sdata,1,swap,file)!=1)
        {
          fprintf(stderr,"Error reading input from '%s'.\n",argv[index+1]);
          return EXIT_FAILURE;
        }
        fwrite(&sdata,sizeof(short),1,out);
      }
      break;
    case 4:
      for(i=0;i<header.nyear*header.nbands*header.ncell;i++)
      {
        if(freadint(&idata,1,swap,file)!=1)
        {
          fprintf(stderr,"Error reading input from '%s'.\n",argv[index+1]);
          return EXIT_FAILURE;
        }
        fwrite(&idata,sizeof(int),1,out);
      }
      break;
    case 8:
      for(i=0;i<header.nyear*header.nbands*header.ncell;i++)
      {
        if(freadlong(&ldata,1,swap,file)!=1)
        {
          fprintf(stderr,"Error reading input from '%s'.\n",argv[index+1]);
          return EXIT_FAILURE;
        }
        fwrite(&ldata,sizeof(long long),1,out);
      }
      break;
  } /* of switch */
  fclose(file);
  fclose(out);
  return EXIT_SUCCESS;
} /* of 'main' */

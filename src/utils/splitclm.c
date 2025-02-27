/**************************************************************************************/
/**                                                                                \n**/
/**                 s  p  l  i  t  c  l  m  .  c                                   \n**/
/**                                                                                \n**/
/**     Program cuts specific bands out of CLM file                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE "Usage: splitclm [-longheader] first last src.clm dst.clm\n"

int main(int argc,char **argv)
{
  FILE *file,*out;
  char *endptr;
  Header header;
  Bool swap;
  int version,first,last,index,*idata,i,nbands;
  long long size,*ldata,filesize;
  short *sdata;
  Byte *bdata;
  String id;
  version=READ_VERSION;
  for(index=1;index<argc;index++)
  {
    if(argv[index][0]=='-')
    {
      if(!strcmp(argv[index],"-longheader"))
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
  if(argc<index+4)
  {
    fprintf(stderr,"Argument(s) missing.\n"
            USAGE);
    return EXIT_FAILURE;
  }
  first=strtol(argv[index],&endptr,10);
  if(*endptr!='\0')
  {
    fprintf(stderr,"Invalid number '%s' for first.\n",argv[index]);
    return EXIT_FAILURE;
  }
  if(first<=0)
  {
    fprintf(stderr,"Invalid number %d for first, must be >0.\n",first);
    return EXIT_FAILURE;
  }
  last=strtol(argv[index+1],&endptr,10);
  if(*endptr!='\0')
  {
    fprintf(stderr,"Invalid number '%s' for last.\n",argv[index]);
    return EXIT_FAILURE;
  }
  if(last<first)
  {
    fprintf(stderr,"Invalid number %d for last band, must be >=%d.\n",last,first);
    return EXIT_FAILURE;
  }
  if(!strcmp(argv[index+2],argv[index+3]))
  {
    fputs("Error: source and destination filename are the same.\n",stderr);
    return EXIT_FAILURE;
  }
  file=fopen(argv[index+2],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s\n",argv[index+2],strerror(errno));
    return EXIT_FAILURE;
  }
  if(freadanyheader(file,&header,&swap,id,&version,TRUE))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",
            argv[index+2]);
    return EXIT_FAILURE;
  }
  if(version>CLM_MAX_VERSION)
  {
    fprintf(stderr,"Error: Unsupported version %d in '%s', must be less than %d.\n",
            version,argv[index+2],CLM_MAX_VERSION+1);
    return EXIT_FAILURE;
  }
  if(version>=3)
    size=typesizes[header.datatype];
  else
  {
    filesize=getfilesizep(file);
    size=(filesize-headersize(id,version))/header.ncell/header.nbands/header.nyear/header.nstep;
    printf("Size of data: %Ld bytes\n",size);
    if(size!=1 && size!=2 && size!=4 && size!=8)
    {
      fprintf(stderr,"Invalid size of data=%Ld.\n",size);
      return EXIT_FAILURE;
    }
    if((filesize-headersize(id,version)) % ((long long)header.ncell*header.nbands*header.nyear*header.nstep)!=0)
      fprintf(stderr,"Warning: file size of '%s' is not multiple of ncell*nbands*nstep*nyear.\n",argv[index+2]);
  }
  if(first>header.nbands)
  {
    fprintf(stderr,"Invalid first band %d, must be in [1,%d].\n",
            first,header.nbands);
    return EXIT_FAILURE;
  }
  if(last>header.nbands)
  {
    fprintf(stderr,"Invalid last band %d, must be in [%d,%d].\n",
            last,first,header.nbands);
    return EXIT_FAILURE;
  }
  out=fopen(argv[index+3],"wb");
  if(out==NULL)
  {
    fprintf(stderr,"Error creating '%s': %s\n",argv[index+3],strerror(errno));
    return EXIT_FAILURE;
  }
  nbands=header.nbands;
  header.nbands=last-first+1;
  first--;
  fwriteheader(out,&header,id,version);
  switch(size)
  {
    case 1:
      bdata=malloc(nbands);
      check(bdata);
      for(i=0;i<header.nyear*header.nstep*header.ncell;i++)
      {
        if(fread(&bdata,1,nbands,file)!=nbands)
        {
          fprintf(stderr,"Error reading input from '%s'.\n",argv[index+2]);
          return EXIT_FAILURE;
        }
        if(fwrite(bdata+first,1,header.nbands,out)!=header.nbands)
        {
          fprintf(stderr,"Error writing output to '%s': %s.\n",
                  argv[index+3],strerror(errno));
          return EXIT_FAILURE;
        }
      }
      break;
    case 2:
      sdata=newvec(short,nbands);
      check(sdata);
      for(i=0;i<header.nyear*header.nstep*header.ncell;i++)
      {
        if(freadshort(sdata,nbands,swap,file)!=nbands)
        {
          fprintf(stderr,"Error reading input from '%s'.\n",argv[index+2]);
          return EXIT_FAILURE;
        }
        if(fwrite(sdata+first,sizeof(short),header.nbands,out)!=header.nbands)
        {
          fprintf(stderr,"Error writing output to '%s': %s.\n",
                  argv[index+3],strerror(errno));
          return EXIT_FAILURE;
        }
      }
      break;
    case 4:
      idata=newvec(int,nbands);
      check(idata);
      for(i=0;i<header.nyear*header.nstep*header.ncell;i++)
      {
        if(freadint(idata,nbands,swap,file)!=nbands)
        {
          fprintf(stderr,"Error reading input from '%s'.\n",argv[index+2]);
          return EXIT_FAILURE;
        }
        if(fwrite(idata+first,sizeof(int),header.nbands,out)!=header.nbands)
        {
          fprintf(stderr,"Error writing output to '%s': %s.\n",
                  argv[index+3],strerror(errno));
          return EXIT_FAILURE;
        }
      }
      break;
    case 8:
      ldata=newvec(long long,nbands);
      check(ldata);
      for(i=0;i<header.nyear*header.nstep*header.ncell;i++)
      {
        if(freadlong(ldata,nbands,swap,file)!=nbands)
        {
          fprintf(stderr,"Error reading input from '%s'.\n",argv[index+2]);
          return EXIT_FAILURE;
        }
        if(fwrite(ldata+first,sizeof(long long),header.nbands,out)!=header.nbands)
        {
          fprintf(stderr,"Error writing output to '%s': %s.\n",
                  argv[index+3],strerror(errno));
          return EXIT_FAILURE;
        }
      }
      break;
  } /* of switch */
  fclose(file);
  fclose(out);
  return EXIT_SUCCESS;
} /* of 'main' */

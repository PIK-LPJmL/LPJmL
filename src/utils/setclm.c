/**************************************************************************************/
/**                                                                                \n**/
/**                       s  e  t  c  l  m  .  c                                   \n**/
/**                                                                                \n**/
/**     Program sets header of clm file                                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE "Usage: %s id|version|order|firstyear|nyear|firstcell|ncell|nbands value filename\n"

int main(int argc,char **argv)
{
  FILE *file;
  Header header;
  String s;
  int version;
  char *id;
  Bool swap;
  const char *progname;
  char *endptr;
  progname=strippath(argv[0]);
  if(argc<4)
  {
    fprintf(stderr,"Error: argument(s) missing.\n"
            USAGE,progname);
    return EXIT_FAILURE;
  }
  file=fopen(argv[3],"r+b");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s\n",argv[3],strerror(errno));
    return EXIT_FAILURE;
  }
  version=READ_VERSION;
  if(freadanyheader(file,&header,&swap,s,&version))
  {
    fprintf(stderr,"Error reading header in '%s', perhaps not a clm file.\n",
            argv[3]);
    return EXIT_FAILURE;
  }
  id=NULL;
  if(!strcmp(argv[1],"id"))
  {
    id=argv[2];
    if(strlen(id)!=strlen(s))
    {
      fprintf(stderr,"Length of new id '%s' is different from '%s'.\n",
              id,s);
      return EXIT_FAILURE;
    }
  }
  else if(!strcmp(argv[1],"version"))
  {
    version=strtol(argv[2],&endptr,10);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for version.\n",argv[2]);
      return EXIT_FAILURE;
    }
  }
  else if(!strcmp(argv[1],"order"))
  {
    header.order=strtol(argv[2],&endptr,10);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for order.\n",argv[2]);
      return EXIT_FAILURE;
    }
  }
  else if(!strcmp(argv[1],"firstyear"))
  {
    header.firstyear=strtol(argv[2],&endptr,10);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for firstyear.\n",argv[2]);
      return EXIT_FAILURE;
    }
  }
  else if(!strcmp(argv[1],"nyear"))
  {
    header.nyear=strtol(argv[2],&endptr,10);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for nyear.\n",argv[2]);
      return EXIT_FAILURE;
    }
  }
  else if(!strcmp(argv[1],"firstcell"))
  {
    header.firstcell=strtol(argv[2],&endptr,10);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for firstcell.\n",argv[2]);
      return EXIT_FAILURE;
    }
  }
  else if(!strcmp(argv[1],"ncell"))
  {
    header.ncell=strtol(argv[2],&endptr,10);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for ncell.\n",argv[2]);
      return EXIT_FAILURE;
    }
  }
  else if(!strcmp(argv[1],"nbands"))
  {
    header.nbands=strtol(argv[2],&endptr,10);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for nbands.\n",argv[2]);
      return EXIT_FAILURE;
    }
  }
  else if(!strcmp(argv[1],"scalar"))
  {
    header.scalar=(float)strtod(argv[2],&endptr);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for scalar.\n",argv[2]);
      return EXIT_FAILURE;
    }
  }
  else
  {
    fclose(file);
    fprintf(stderr,"Invalid header item '%s'.\n"
            USAGE,argv[1],progname);
    return EXIT_FAILURE;
  }
  if(id==NULL)
    id=s;
  if(swap)
  {
    version=swapint(version);
    header.order=swapint(header.order);
    header.firstyear=swapint(header.firstyear);
    header.nyear=swapint(header.nyear);
    header.firstcell=swapint(header.firstcell);
    header.ncell=swapint(header.ncell);
    header.nbands=swapint(header.nbands);
  }
  rewind(file);
  if(fwrite(id,strlen(id),1,file)!=1)
    return EXIT_FAILURE;
  if(fwrite(&version,sizeof(version),1,file)!=1)
    return EXIT_FAILURE;
  if(fwrite(&header,sizeof(Header_old),1,file)!=1)
    return EXIT_FAILURE;
  fclose(file); 
  return EXIT_SUCCESS;
} /* of 'main' */

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

#define USAGE "Usage: %s id|version|order|firstyear|nyear|firstcell|ncell|nbands|nstep|type|cellsize|cellsize_lon|cellsize_lat value filename\n"

static Bool writeheader(FILE *file,int *header,int size,Bool swap)
{
  int i;
  if(swap)
    for(i=0;i<size;i++)
      header[i]=swapint(header[i]);
  return (fwrite(header,sizeof(int),size,file)!=size);
} /* of 'writeheader' */

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
  int index,size;
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
  if(freadanyheader(file,&header,&swap,s,&version,TRUE))
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
      header.order=findstr(argv[2],ordernames,4);
      if(header.order==NOT_FOUND)
      {
        fprintf(stderr,"Invalid number '%s' for order.\n",argv[2]);
        return EXIT_FAILURE;
      }
      header.order++;
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
  else if(!strcmp(argv[1],"nstep"))
  {
    if(version<4)
    {
      fprintf(stderr,"Version=%d for nstep of '%s' must be >3.\n",version,argv[3]);
      return EXIT_FAILURE;
    }
    header.nstep=strtol(argv[2],&endptr,10);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for nstep.\n",argv[2]);
      return EXIT_FAILURE;
    }
  }
  else if(!strcmp(argv[1],"-type"))
  {
    if(version<3)
    {
      fprintf(stderr,"Version=%d for type of '%s' must be >2.\n",version,argv[3]);
      return EXIT_FAILURE;
    }
    index=findstr(argv[2],typenames,5);
    if(index==NOT_FOUND)
    {
      fprintf(stderr,"Invalid argument '%s' for type.\n",argv[2]);
      return EXIT_FAILURE;
    }
    header.datatype=(Type)index;
  }
  else if(!strcmp(argv[1],"scalar"))
  {
    if(version<2)
    {
      fprintf(stderr,"Version=%d for scalar of '%s' must be >1.\n",version,argv[3]);
      return EXIT_FAILURE;
    }
    header.scalar=(float)strtod(argv[2],&endptr);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for scalar.\n",argv[2]);
      return EXIT_FAILURE;
    }
  }
  else if(!strcmp(argv[1],"cellsize"))
  {
    if(version<2)
    {
      fprintf(stderr,"Version=%d for cellsize of '%s' must be >1.\n",version,argv[3]);
      return EXIT_FAILURE;
    }
    header.cellsize_lon=header.cellsize_lat=(float)strtod(argv[2],&endptr);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for cellsize.\n",argv[2]);
      return EXIT_FAILURE;
    }
  }
  else if(!strcmp(argv[1],"cellsize_lon"))
  {
    if(version<3)
    {
      fprintf(stderr,"Version=%d for cellsize_lon of '%s' must be >2.\n",version,argv[3]);
      return EXIT_FAILURE;
    }
    header.cellsize_lon=(float)strtod(argv[2],&endptr);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for cellsize_lon.\n",argv[2]);
      return EXIT_FAILURE;
    }
  }
  else if(!strcmp(argv[1],"cellsize_lat"))
  {
    if(version<3)
    {
      fprintf(stderr,"Version=%d for cellsize_lat of '%s' must be >2.\n",version,argv[3]);
      return EXIT_FAILURE;
    }
    header.cellsize_lat=(float)strtod(argv[2],&endptr);
    if(*endptr!='\0')
    {
      fprintf(stderr,"Invalid number '%s' for cellsize_lat.\n",argv[2]);
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
  if(swap)
     version=swapint(version);
  if(fwrite(&version,sizeof(version),1,file)!=1)
    return EXIT_FAILURE;
  switch(version)
  {
     case 1:
       size=sizeof(Header_old)/sizeof(int);
       break;
     case 2:
       size=sizeof(Header2)/sizeof(int);
       break;
     case 3:
       size=sizeof(Header3)/sizeof(int);
       break;
     case 4:
       size=sizeof(Header)/sizeof(int);
       break;
  }
  if(writeheader(file,(int *)&header,size,swap))
    return EXIT_FAILURE;
  fclose(file); 
  return EXIT_SUCCESS;
} /* of 'main' */

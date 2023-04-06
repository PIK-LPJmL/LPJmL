/**************************************************************************************/
/**                                                                                \n**/
/**                       s  t  a  t  c  l  m  .  c                                \n**/
/**                                                                                \n**/
/**     Program prints minimum, maximum and average value of clm files             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE "Usage: %s [-scale s] [-verbose] [-csv] [-type {byte|short|int|float|double}] filename ...\n"

int main(int argc,char **argv)
{
  Header header;
  String id;
  FILE *file;
  long long size;
  Bool swap,verbose,iscsv;
  int version,i,j,cell,iarg,year,index;
  float fmin,fmax,favg,avg,cavg,fmin2,fmax2,scale;
  float *vec;
  char *endptr;
  Type datatype;
  scale=1;
  verbose=iscsv=FALSE;
  datatype=LPJ_SHORT;
  for(iarg=1;iarg<argc;iarg++)
  {
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-scale"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Argument missing after '-scale' option.\n"
                USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        scale=(float)strtod(argv[++iarg],&endptr);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid value '%s' for option '-scale'.\n",
                  argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else if(!strcmp(argv[iarg],"-type"))
      {
        if(iarg==argc-1)
        {
          fprintf(stderr,"Argument missing after option '-type'.\n"
                  USAGE,argv[0]);
          return EXIT_FAILURE;
        }
        index=findstr(argv[++iarg],typenames,5);
        if(index==NOT_FOUND)
        {
          fprintf(stderr,"Invalid argument '%s' for option '-type'.\n"
                  USAGE,argv[iarg],argv[0]);
          return EXIT_FAILURE;
        }
        datatype=(Type)index;
      }
      else if(!strcmp(argv[iarg],"-verbose"))
        verbose=TRUE;
      else if(!strcmp(argv[iarg],"-csv"))
        iscsv=TRUE;
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n",argv[iarg]);
        fprintf(stderr,USAGE,argv[0]);
        return EXIT_FAILURE;
      }
    }
    else
     break;
  }
  if(argc<iarg+1)
  {
    fprintf(stderr,"Missing argument.\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  if(iscsv)
  {
    if(verbose)
      printf("filename,year,min,max,avg\n");
    else
      printf("filename,min,max,avg\n");
  }
  for(i=iarg;i<argc;i++)
  {
    version=READ_VERSION;
    file=fopen(argv[i],"rb");
    if(file==NULL)
    {
      fprintf(stderr,"Error opening '%s': %s\n",argv[iarg],strerror(errno));
      return EXIT_FAILURE;
    }
    if(freadanyheader(file,&header,&swap,id,&version,TRUE))
      return EXIT_FAILURE;
    if(version<2)
      header.scalar=scale;
    if(version<3)
      header.datatype=datatype;
    size=getfilesizep(file)-headersize(id,version);
    if(size!=typesizes[header.datatype]*header.ncell*header.nbands*header.nyear*header.nstep)
      fprintf(stderr,"Warning: File length of '%s' does not match header, differs by %lld bytes.\n",
              argv[i],llabs(size-(long long)typesizes[header.datatype]*header.ncell*header.nbands*header.nstep*header.nyear));
    fmin=HUGE_VAL;
    fmax=-HUGE_VAL;
    favg=0;
    vec=newvec(float,header.nstep*header.nbands);
    for(year=0;year<header.nyear;year++)
    {
      cavg=0;
      fmin2=HUGE_VAL;
      fmax2=-HUGE_VAL;
      for(cell=0;cell<header.ncell;cell++)
      {
        avg=0;
        if(readfloatvec(file,vec,header.scalar,header.nstep*header.nbands,swap,header.datatype))
        {
          fprintf(stderr,"Unexpected end of file in '%s'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
        for(j=0;j<header.nstep*header.nbands;j++)
        {
          fmin=min(fmin,vec[j]);
          fmax=max(fmax,vec[j]);
          fmin2=min(fmin2,vec[j]);
          fmax2=max(fmax2,vec[j]);
          avg+=vec[j];
        }
        cavg+=avg/header.nstep/header.nbands;
      }
      favg+=cavg/header.ncell;
      if(verbose)
      {
        if(iscsv)
          printf("%s,%d,%g,%g,%g\n",argv[i],year+header.firstyear,fmin2,fmax2,cavg/header.ncell);
        else
        {
          if(argc-iarg>1)
            printf("%s: ",argv[i]);
          printf("year=%d, min=%g, max=%g, avg=%g\n",year+header.firstyear,fmin2,fmax2,cavg/header.ncell);
        }
      }
    }
    fclose(file);
    free(vec);
    if(iscsv)
    {
      if(verbose)
        printf("%s,0,%g,%g,%g\n",argv[i],fmin,fmax,favg/header.nyear);
      else
        printf("%s,%g,%g,%g\n",argv[i],fmin,fmax,favg/header.nyear);
    }
    else
    {
      if(argc-iarg>1)
        printf("%s: ",argv[i]);
      printf("min=%g, max=%g, avg=%g\n",fmin,fmax,favg/header.nyear);
    }
  }
  return EXIT_SUCCESS;
} /* of 'main' */

/**************************************************************************************/
/**                                                                                \n**/
/**                       c  m  p  b  i  n  .  c                                   \n**/
/**                                                                                \n**/
/**     Program compares two binary output files                                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE "Usage: %s [-metafile] [-verbose] [-csv] file1.bin file2.bin\n"

int main(int argc,char **argv)
{
  long n,nmax,count;
  size_t offset;
  long long size1,size2;
  FILE *file1,*file2;
  float val1,val2;
  double diff,max;
  Header header1,header2;
  Bool ismeta,swap1,swap2,verbose,iscsv;
  int iarg;
  ismeta=swap1=swap2=verbose=iscsv=FALSE;
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-metafile"))
        ismeta=TRUE;
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
  if(argc<iarg+2)
  {
    fprintf(stderr,"Missing arguments.\n"
            USAGE,argv[0]);
    return EXIT_FAILURE;
  }
  if(ismeta)
  {
    header1.scalar=1;
    header1.cellsize_lon=header1.cellsize_lat=0.5;
    header1.firstyear=1901;
    header1.firstcell=0;
    header1.nyear=1;
    header1.nbands=1;
    header1.nstep=1;
    header1.timestep=1;
    header1.datatype=LPJ_FLOAT;
    header1.order=CELLSEQ;
    header1.scalar=1;
    file1=openmetafile(&header1,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,&swap1,&offset,argv[iarg],TRUE);
    if(file1==NULL)
      return EXIT_FAILURE;
    if(fseek(file1,offset,SEEK_CUR))
    {
      fprintf(stderr,"Error seeking in '%s' to offset %lu.\n",argv[iarg],offset);
      return EXIT_FAILURE;
    }
    header2.scalar=1;
    header2.cellsize_lon=header2.cellsize_lat=0.5;
    header2.firstyear=1901;
    header2.firstcell=0;
    header2.nyear=1;
    header2.nbands=1;
    header2.nstep=1;
    header2.timestep=1;
    header2.datatype=LPJ_FLOAT;
    header2.order=CELLSEQ;
    header2.scalar=1;
    file2=openmetafile(&header2,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,&swap2,&offset,argv[iarg+1],TRUE);
    if(file2==NULL)
      return EXIT_FAILURE;
    if(fseek(file2,offset,SEEK_CUR))
    {
      fprintf(stderr,"Error seeking in '%s' to offset %lu.\n",argv[iarg+1],offset);
      return EXIT_FAILURE;
    }
    if(header1.nyear!=header2.nyear)
    {
      fprintf(stderr,"Number of years in '%s'=%d differs from number of years in '%s'=%d.\n",
              argv[iarg],header1.nyear,argv[iarg+1],header1.nyear);
      header1.nyear=min(header1.nyear,header2.nyear);
    }
    if(header1.ncell!=header2.ncell)
    {
      fprintf(stderr,"Number of cells in '%s'=%d differs from number of cells in '%s'=%d.\n",
              argv[iarg],header1.ncell,argv[iarg+1],header1.ncell);
      return EXIT_FAILURE;
    }
    if(header1.nbands!=header2.nbands)
    {
      fprintf(stderr,"Number of bands in '%s'=%d differs from number of bands in '%s'=%d.\n",
              argv[iarg],header1.nbands,argv[iarg+1],header1.nbands);
      return EXIT_FAILURE;
    }
    if(header1.nstep!=header2.nstep)
    {
      fprintf(stderr,"Number of steps in '%s'=%d differs from number of steps in '%s'=%d.\n",
              argv[iarg],header1.nstep,argv[iarg+1],header1.nstep);
      return EXIT_FAILURE;
    }
    if(header1.datatype!=LPJ_FLOAT)
    {
      fprintf(stderr,"Datatype in '%s' is %s, must be float.\n",argv[iarg],typenames[header1.datatype]);
      return EXIT_FAILURE;
    }
    if(header2.datatype!=LPJ_FLOAT)
    {
      fprintf(stderr,"Datatype in '%s' is %s, must be float.\n",argv[iarg+1],typenames[header2.datatype]);
      return EXIT_FAILURE;
    }
    count=(long)header1.nyear*header1.ncell*header1.nbands*header1.nstep/header1.timestep;
  }
  else
  {
    file1=fopen(argv[iarg],"rb");
    if(file1==NULL)
    {
      fprintf(stderr,"Error opening '%s': %s\n",argv[iarg],strerror(errno));
      return EXIT_FAILURE;
    }
    file2=fopen(argv[iarg+1],"rb");
    if(file2==NULL)
    {
      fprintf(stderr,"Error opening '%s': %s\n",argv[iarg+1],strerror(errno));
      return EXIT_FAILURE;
    }
    size1=getfilesizep(file1);
    if(size1==0)
    {
      fprintf(stderr,"File '%s' is empty.\n",argv[iarg]);
      return EXIT_FAILURE;
    }
    if(size1 % sizeof(float))
    {
      fprintf(stderr,"File size of '%s' is not multiple of float size.\n",argv[iarg+1]);
      return EXIT_FAILURE;
    }
    size2=getfilesizep(file2);
    if(size2==0)
    {
      fprintf(stderr,"File '%s' is empty.\n",argv[iarg+1]);
      return EXIT_FAILURE;
    }
    if(size2 % sizeof(float))
    {
      fprintf(stderr,"File size of '%s' is not multiple of float size.\n",argv[iarg+1]);
      return EXIT_FAILURE;
    }
    if(size1!=size2)
    {
      fprintf(stderr,"File size of '%s'=%lld differs from file size of '%s'=%lld.\n",argv[iarg],size1,argv[iarg+1],size2);
      size1=min(size1,size2);
    }
    count=size1/sizeof(float);
  }
  max=0;
  nmax=0;
  diff=0;
  for(n=0;n<count;n++)
  {
    freadfloat(&val1,1,swap1,file1);
    freadfloat(&val2,1,swap2,file2);
    if(max<fabs(val1-val2))
    {
      max=fabs(val1-val2);
      nmax=n;
    }
    if(verbose && val1!=val2)
    {
      if(ismeta)
        printf("year=%ld,nstep=%ld,nbands=%ld, cell=%ld, ",
               n/header1.ncell/header1.nstep/header1.nbands+header1.firstyear,
               (n/header1.ncell/header1.nbands) % header1.nstep,
               (n/header1.ncell) % header1.nbands,
               n % header1.ncell);
      else
        printf("cell %ld, ",n);
      printf("val1=%g, val2=%g, diff=%g\n",val1,val2,fabs(val1-val2));
    }
    diff+=fabs(val1-val2);
  }
  if(max==0)
  {
    if(iscsv)
      printf((ismeta) ? "0,0,0,0,0,0,0\n" : "0,0,0,0\n");
    else
      printf("Files are identical.\n");
  }
  else
  {
    if(ismeta)
      printf((iscsv) ? "%g,%ld,%ld,%ld,%ld,%g,%g\n" : "max=%g at year=%ld,nstep=%ld,nbands=%ld, cell=%ld, sum=%g,avg=%g\n",
             max,
             nmax/header1.ncell/header1.nstep/header1.nbands+header1.firstyear,
             (nmax/header1.ncell/header1.nbands) % header1.nstep,
             (nmax/header1.ncell) % header1.nbands,
             nmax % header1.ncell,
             diff,diff/n);
    else
      printf((iscsv) ? "%g,%ld,%g,%g\n" : "max=%g at %ld, sum=%g,avg=%g\n",max,nmax,diff,diff/n);
  }
  return EXIT_SUCCESS;
} /* of 'main' */

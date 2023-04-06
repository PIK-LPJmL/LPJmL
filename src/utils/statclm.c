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

#define USAGE "Usage: %s [-scale s] [-verbose] filename\n"

int main(int argc,char **argv)
{
  Header header;
  String id;
  FILE *file;
  Bool swap,verbose;
  int version,i,j,cell,iarg;
  float fmin,fmax,favg,avg,cavg,fmin2,fmax2,scale;
  float *vec;
  char *endptr;
  scale=1;
  verbose=FALSE;
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
      else if(!strcmp(argv[iarg],"-verbose"))
        verbose=TRUE;
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
  version=READ_VERSION;
  file=fopen(argv[iarg],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s\n",argv[iarg],strerror(errno));
    return EXIT_FAILURE;
  }
  if(freadanyheader(file,&header,&swap,id,&version,TRUE))
    return EXIT_FAILURE;
  if(version<2)
    header.scalar=scale;
  fmin=HUGE_VAL;
  fmax=-HUGE_VAL;
  favg=0;
  vec=newvec(float,header.nstep*header.nbands);
  for(i=0;i<header.nyear;i++)
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
      printf("year=%d, min=%g, max=%g, avg=%g\n",i+header.firstyear,fmin2,fmax2,cavg/header.ncell);
  }
  printf("min=%g, max=%g, avg=%g\n",fmin,fmax,favg/header.nyear);
  return EXIT_SUCCESS;
} /* of 'main' */

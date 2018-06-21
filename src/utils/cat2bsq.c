/**************************************************************************************/
/**                                                                                \n**/
/**                      c a t 2 b s q  .  c                                       \n**/
/**                                                                                \n**/
/**     Program  concatenates  output files from the LPJmL model                   \n**/
/**     to one output file.                                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
/*#include <sys/types.h>*/
#include <sys/stat.h>
#include "types.h"
#include "errmsg.h"

#define USAGE "Usage: cat2bsq [-type {byte|short|int|float|double}] [-o outfile] [-bands n] infilename ...\n"

int main(int argc,char **argv)
{
  FILE *ofp;
  FILE **ifp;
  void *data;
  int i,j,k,p,*n,runs,bands,sum;
  char *outfile;
  struct stat filestat;
  k=LPJ_FLOAT;
  bands=103;
  outfile=NULL;
  for(i=0;i<argc;i++)
    if(argv[i][0]=='-')
    {
      if(!strcmp(argv[i],"-type"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Argument missing.\n");
          fprintf(stderr,USAGE);
          return EXIT_FAILURE;
        }
        else
        {
          k=findstr(argv[++i],typenames,5);
          if(k==NOT_FOUND)
          {
            fprintf(stderr,"Invalid argument '%s' for option '-type'.\n",argv[i]);
            fprintf(stderr,USAGE);
            return EXIT_FAILURE;
          }
        }
      }
      else if(!strcmp(argv[i],"-o"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Argument missing.\n");
          fprintf(stderr,USAGE);
          return EXIT_FAILURE;
        }
        else
         outfile=argv[++i];
      }
      else if(!strcmp(argv[i],"-bands"))
      {
        if(argc==i+1)
        {
          fprintf(stderr,"Argument missing.\n");
          fprintf(stderr,USAGE);
          return EXIT_FAILURE;
        }
        else
         bands=atoi(argv[++i]);
      }
      else
      {
        fprintf(stderr,"Invalid option '%s'.\n",argv[i]);
        fprintf(stderr,USAGE);
        return EXIT_FAILURE;
      }
    }
    else
      break;
  runs=argc-i;
  if(runs==0)
  {
    fprintf(stderr,"Argument is missing\n" USAGE);
    return EXIT_FAILURE;
  }
  if(outfile==NULL)
    ofp=stdout;
  else
  {
    ofp=fopen(outfile,"wb");
    if(ofp==NULL)
    {
      fprintf(stderr,"Error opening '%s': %s\n",outfile,strerror(errno));
      return EXIT_FAILURE;
    }
  }
  ifp=newvec(FILE *,runs);
  n=(int *)malloc(runs*sizeof(int));
  for(j=0;j<runs;j++)
  {

    if(!(ifp[j]=fopen(argv[i+j],"rb")))
    {
      fprintf(stderr,"Error: File open failed on path %s: %s\n",argv[i+j],strerror(errno));
      return EXIT_FAILURE;
    }
    fstat(fileno(ifp[j]),&filestat);
    if(filestat.st_size % (bands*typesizes[k]))
    {
      fprintf(stderr,"Error: Filesize=%zd is not divisible without remainder=%d, bands=%d, size=%d.",
           filestat.st_size,(int)(filestat.st_size % (bands*typesizes[k])),bands,(int)typesizes[k]);
      return EXIT_FAILURE;
    }
    /* calculate number of pixels in each chunk and store it in array n[i] */
    n[j]=filestat.st_size/bands/typesizes[k];
  }

  for(p=0;p<bands;p++)
  {
    for(i=0;i<runs;i++)
    {
      if(outfile!=NULL && p==0) 
        printf("run: %d n: %d\n",i,n[i]);
      fseek(ifp[i],typesizes[k]*n[i]*p,SEEK_SET);
      data=malloc(n[i]*typesizes[k]);
      if(data==NULL)
      {
        printallocerr("data");
        return EXIT_FAILURE;
      }
      fread(data,typesizes[k],n[i],ifp[i]);
      fwrite(data,typesizes[k],n[i],ofp);
      free(data);
    }
  }
  if(outfile!=NULL)
  {
    sum=0;
    for(i=0;i<runs;i++)
      sum+=n[i];
    printf("Number of cells: %d\n",sum); 
  }
  for(i=0;i<runs;i++) 
    fclose(ifp[i]);
  fclose(ofp);
  return EXIT_SUCCESS;
} /* of 'main' */

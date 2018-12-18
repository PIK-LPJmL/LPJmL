/**************************************************************************************/
/**                                                                                \n**/
/**                      m  a  n  a  g  e  2  j  s  .  c                           \n**/
/**                                                                                \n**/
/**     Converts management parameter files in *.conf format into JSON format.     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "types.h"

int main(int argc,char **argv)
{
  int i,j,n;
  LPJfile file,file2;
  String s;
  if(argc<3)
  {
    fprintf(stderr,"Error: Missing arguments.\n"
           "Usage: %s laimax.par manage.par\n",argv[0]);
    return EXIT_FAILURE;
  }
  file.file.file=fopen(argv[1],"r");
  if(file.file.file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[1],strerror(errno));
    return EXIT_FAILURE;
  }
  file.isjson=FALSE;
  fscanint(&file,&n,"n",FALSE,ERR);
  file2.file.file=fopen(argv[2],"r");
  if(file2.file.file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[2],strerror(errno));
    return EXIT_FAILURE;
  }
  file2.isjson=FALSE;
  fscanint(&file2,&n,"n",FALSE,ERR);
  printf("\"countrypar\" :\n"
         "[\n");
  for(i=0;i<n;i++)
  {
    fscanstring(&file2,s,"",FALSE,ERR);
    fscanstring(&file2,s,"",FALSE,ERR);
    fscanstring(&file,s,"",FALSE,ERR);
    printf("  { \"id\" : %s,",s);
    fscanstring(&file,s,"",FALSE,ERR);
    printf(" \"name\" : \"%s\",",s);
    printf(" \"laimax\" : [");
    for(j=0;j<12;j++)
    {
      fscanstring(&file,s,"",FALSE,ERR);
      printf(" %s",s);
      if(j<11)
        printf(",");
    }
    fscanstring(&file2,s,"",FALSE,ERR);
    printf("], \"laimax_tempcer\" : %s,",s);
    fscanstring(&file2,s,"",FALSE,ERR);
    printf(" \"laimax_maize\" : %s,",s);
    fscanstring(&file2,s,"",FALSE,ERR);
    fscanstring(&file,s,"",FALSE,ERR);
    printf(" \"default_irrig_system\" : %s}",s);
    if(i<n-1)
      printf(",\n");
    else 
      printf("\n");
  }
  printf("],\n");
  return EXIT_SUCCESS;
} /* of 'main' */

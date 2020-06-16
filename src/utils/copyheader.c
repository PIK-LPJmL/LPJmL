/**************************************************************************************/
/**                                                                                \n**/
/**                    c  o  p  y  h  e  a  d  e  r  .  c                          \n**/
/**                                                                                \n**/
/**     Program copies CLM header to another CLM file                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int main(int argc,char **argv)
{
  Header header,header2;
  int version,version2;
  String id,id2;
  Bool swap,swap2;
  FILE *file;
  if(argc<3)
  {
    fprintf(stderr,"Missing arguments.\n"
           "Usage: %s clmfile clmoutfile\n",argv[0]);
    return EXIT_FAILURE;
  }
  file=fopen(argv[1],"rb");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[1],strerror(errno));
    return EXIT_FAILURE;
  }
  version=READ_VERSION;
  if(freadanyheader(file,&header,&swap,id,&version))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",argv[1]);
    return EXIT_FAILURE;
  }
  fclose(file);
  file=fopen(argv[2],"w+b");
  if(file==NULL)
  {
    fprintf(stderr,"Error opening '%s': %s.\n",argv[2],strerror(errno));
    return EXIT_FAILURE;
  }
  version2=READ_VERSION;
  if(freadanyheader(file,&header2,&swap2,id2,&version2))
  {
    fprintf(stderr,"Error reading header in '%s'.\n",argv[2]);
    return EXIT_FAILURE;
  }
  if(version!=version2)
    fprintf(stderr,"Warning: file versions differ.\n");
  if(strlen(id)!=strlen(id2))
    fprintf(stderr,"Warning: id lengths differ.\n");
  if(swap2)
    fprintf(stderr,"Warning: data has to be swapped.\n");
  rewind(file);
  fwriteheader(file,&header,id,version);
  fclose(file);
  return EXIT_SUCCESS;
} /* of 'main' */

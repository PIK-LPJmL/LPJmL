/**************************************************************************************/
/**                                                                                \n**/
/**             g  e  t  h  e  a  d  e  r  s  i  z  e  .  c                        \n**/
/**                                                                                \n**/
/**     Program prints header size of  CLM files                                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define USAGE "Usage: headersize [-version v] clmfile ...\n"

int main(int argc,char **argv)
{
  char *endptr;
  int i,iarg,version,version_set,rc;
  Bool swap;
  version_set=READ_VERSION;
  Header header;
  FILE *file;
  String id;
  for(iarg=1;iarg<argc;iarg++)
    if(argv[iarg][0]=='-')
    {
      if(!strcmp(argv[iarg],"-version"))
      {
        if(argc-1==iarg)
        {
          fprintf(stderr,"Error Argument missing for option '-version'.\n"
                  USAGE);
          return EXIT_FAILURE;
        }
        version_set=strtol(argv[++iarg],&endptr,10);
        if(*endptr!='\0')
        {
          fprintf(stderr,"Invalid number '%s' for option '-version'.\n",argv[iarg]);
          return EXIT_FAILURE;
        }
      }
      else
      {
        fprintf(stderr,"Error: Invalid option '%s'.\n"
                USAGE,argv[iarg]);
        return EXIT_FAILURE;
      } 
    }
    else
      break;
  if(iarg==argc)
  {
    fputs("Error: Argument missing.\n"
          USAGE,stderr);
    return EXIT_FAILURE;
  }
  rc=EXIT_SUCCESS;
  for(i=iarg;i<argc;i++)
  {
    file=fopen(argv[i],"rb");
    if(file==NULL)
    {
      fprintf(stderr,"Error opening '%s': %s.\n",argv[i],strerror(errno));
      rc=EXIT_FAILURE;
      continue;
    }
    version=version_set;
    if(freadanyheader(file,&header,&swap,id,&version))
    {
      fprintf(stderr,"Error reading header in '%s'.\n",argv[i]);
      fclose(file);
      rc=EXIT_FAILURE;
      continue;
    }
    fclose(file);
    if(argc-iarg>1)
      printf("%s: ",argv[i]);
    printf("%zu\n",headersize(id,version));
  }
  return rc;
} /* of 'main' */

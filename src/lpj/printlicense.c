/**************************************************************************************/
/**                                                                                \n**/
/**                    p  r  i  n  t  l  i  c  e  n  s  e  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints license file $LPJROOT/LICENSE                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define LICENSE_NAME "LICENSE" /* name of license file */

void printlicense(void)
{
  char *lpjroot,*path;
  String line;
  FILE *file,*in;
  lpjroot=getenv(LPJROOT);
  if(lpjroot==NULL)
  {
    fprintf(stderr,"WARNING019: Environment variable '%s' not set.\n",
            LPJROOT);
    path=strdup(LICENSE_NAME);
    if(path==NULL)
    { 
      printallocerr("path");
      return;
    }
  }
  else
  {
    path=malloc(strlen(lpjroot)+2+strlen(LICENSE_NAME));
    if(path==NULL)
    { 
      printallocerr("path");
      return;
    }
    strcpy(path,lpjroot);
    strcat(path,"/");
    strcat(path,LICENSE_NAME);
  }
  file=popen("more","w");
  if(file==NULL)
    file=stdout;
  in=fopen(path,"r");
  if(in==NULL)
    fprintf(stderr,"Error opening license file '%s': %s.\n",path,strerror(errno));
  else
  {
    while(fgets(line,STRING_LEN,in)!=NULL)
      fputs(line,file);
    fclose(in);
  } 
  free(path);
  if(file!=stdout)
    pclose(file);
} /* of 'printlicense' */

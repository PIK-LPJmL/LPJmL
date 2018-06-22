/**************************************************************************************/
/**                                                                                \n**/
/**                   g  e  t  p  a  t  h  .  c                                    \n**/
/**                                                                                \n**/
/**     Function extracts path from filename                                       \n**/
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
#include <string.h>
#include "types.h"

char *getpath(const char *name /**< filename */
             )                 /** \return pointer to allocated path */
{
  int i;
  char *path;
  for(i=strlen(name)-1;i>=0;i--)
#ifdef _WIN32
    if(name[i]=='/' || name[i]=='\\' || name[i]==':')
    /* check for slash, backslash and ':' of drive */
#else
    if(name[i]=='/') /* check for slash only */
#endif
      break;
  if(i>=0)
  {
    path=malloc(i+2);
    if(path==NULL)
      return NULL; /* could not allocate memory, return NULL */
    strncpy(path,name,i+1);
    path[i+1]='\0';
  }
  else
    path=strdup(".");
  return path;
} /* of 'getpath' */

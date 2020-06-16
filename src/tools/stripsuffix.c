/**************************************************************************************/
/**                                                                                \n**/
/**                   s  t  r  i  p  s  u  f  f  i  x  .  c                        \n**/
/**                                                                                \n**/
/**     Function strips suffix from filename                                       \n**/
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

char *stripsuffix(const char *name /* filename with suffix */
                 )                 /* returns pointer to filename */
{
  int i;
  char *new;
#ifdef SAFE
  if(name==NULL)
    return NULL;
#endif
  for(i=strlen(name)-1;i>=0;i--)
    if(name[i]=='.')
      break;
  if(i<0) /* no suffix found */
    i=strlen(name);
  new=malloc(i+1);
  if(new==NULL)
    return NULL;
  strncpy(new,name,i);
  new[i]='\0';
  return new;
} /* of 'stripsuffix' */

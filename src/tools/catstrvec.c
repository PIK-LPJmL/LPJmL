/**************************************************************************************/
/**                                                                                \n**/
/**                  c  a  t  s  t  r  v  e  c  .  c                               \n**/
/**                                                                                \n**/
/**     Function concatenates string vector                                        \n**/
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

char *catstrvec(const char * const *vec,int n)
{
  int i;
  size_t len;
  char *s;
  if(n<1)
    return strdup("");
  len=0;
  for(i=0;i<n;i++)
    len+=strlen(vec[i])+1;
  s=malloc(len);
  if(s==NULL)
    return NULL;
  strcpy(s,vec[0]);
  for(i=1;i<n;i++)
  {
    strcat(s," ");
    strcat(s,vec[i]);
  }
  return s;
} /* of 'catstrvec' */

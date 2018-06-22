/**************************************************************************************/
/**                                                                                \n**/
/**               m  k  f  i  l  e  n  a  m  e  .  c                               \n**/
/**                                                                                \n**/
/**     Function changes string 'filename.[aaaa-bbbb].suffix' into                 \n**/
/**     'filename.[%d].suffix'                                                     \n**/
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

char *mkfilename(const char *filename /**< filename with format 'filename.[aaaa-bbbb].suffix' */
                )                     /** \return 'filename.[%d].suffix' or NULL */
{
  int i,j;
  char *new;
  if(filename==NULL)
    return NULL;
  /* find the first occurrence of opening '[' */
  for(i=0;filename[i]!='[';i++)
    if(filename[i]=='\0') /* end of string reached? */
      return NULL;
  /* find the first occurrence of closing ']' */
  for(j=i;filename[j]!=']';j++)
    if(filename[j]=='\0') /* end of string reached? */
      return NULL;
  new=malloc(i+2+strlen(filename)-j);
  if(new==NULL)
    return NULL;
  strncpy(new,filename,i);
  new[i]='\0';
  strcat(new,"%d");
  strcat(new,filename+j+1);
  return new;
} /* of 'mkfilename' */

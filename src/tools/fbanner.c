/**************************************************************************************/
/**                                                                                \n**/
/**                   f  b  a  n  n  e  r  .  c                                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints copyright notice                                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "types.h"

void fbanner(FILE *file,            /**< pointer to text file */
             const char * const *s, /**< array of strings to be displayed */
             int n,                 /**< size of string array */
             int width              /**< width of banner */)
{
  int i,len;
  frepeatch(file,'*',width);
  putc('\n',file);
  for(i=0;i<n;i++)
  {
    len=strlen(s[i]);
    fputs("****",file);
    frepeatch(file,' ',(width-8-len)/2);
    fputs(s[i],file);
    frepeatch(file,' ',(width-8-len)/2+(len+width) %2);
    fputs("****\n",file);
  }
  frepeatch(file,'*',width);
  putc('\n',file);
} /* of 'fbanner' */

/**************************************************************************************/
/**                                                                                \n**/
/**                      f  i  n  d  s  t  r  .  c                                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function finds string in array of strings                                  \n**/
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

int findstr(const char *s,            /**< string to be find in array */
            const char *const *array, /**< array of strings */
            int size                  /**< size of array */
           )                          /** \return index of string found or NOT_FOUND */
{
  int i;
  if(s!=NULL && array!=NULL)
  {
    for(i=0;i<size;i++)
      if(!strcmp(s,array[i]))
        return i;
  }
  return NOT_FOUND;
} /* of 'findstr' */

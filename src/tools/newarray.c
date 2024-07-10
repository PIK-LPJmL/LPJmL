/**************************************************************************************/
/**                                                                                \n**/
/**                     n  e  w  a  r  r  a  y  .  c                               \n**/
/**                                                                                \n**/
/** Function allocates storage for an array with bounds [lo,hi]                    \n**/
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
#include "types.h"

void *newarray(size_t size, /**< size of array element */
               int lo,      /**< lower bound of array */
               int hi       /**< upper bound of array */
              )             /** \return pointer to array or NULL */
{
  void *ptr;
  ptr=malloc(size*(hi-lo+1));
  if(ptr==NULL)
    return NULL;
  return ptr-lo*size;
} /* of 'newarray' */

/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  i  n  t  a  r  r  a  y  .  c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads an integer array from a LPJ file                            \n**/
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
#include <math.h>
#include <json-c/json.h>
#include "types.h"

Bool fscanintarray(LPJfile *file,    /**< pointer to LPJ file */
                   int value[],      /**< integer array to read */
                   int size,         /**< size of array */
                   const char *key,  /**< name of array */
                   Verbosity verb    /**< verbosity level (NO_ERR,ERR,VERB) */
                  )
{
  int i;
  struct json_object *array,*item;
  if(!json_object_object_get_ex(file,key,&array))
  {
    if(verb)
      fprintf(stderr,"ERROR225: Name '%s' for array not found.\n",key);
    return TRUE;
  }
  if(json_object_get_type(array)!=json_type_array)
  {
    if(verb)
      fprintf(stderr,"ERROR226: Type of '%s' is not an array.\n",key);
    return TRUE;
  }
  if(json_object_array_length(array)!=size)
  {
    if(verb)
      fprintf(stderr,"ERROR227: Size of %s=%zu is not %d.\n",
              key,json_object_array_length(array),size);
    return TRUE;
  }
  if (verb >= VERB)
    printf("\"%s\" : [",key);
  for(i=0;i<size;i++)
  {
    item =json_object_array_get_idx(array,i);
    if(json_object_get_type(item)!=json_type_int)
    {
      if(verb)
        fprintf(stderr,"ERROR226: Type of item %d in array %s is not int.\n",i,key);
      return TRUE;
    }
    value[i]=json_object_get_int(item);
    if (verb >= VERB)
    {
      printf(" %d",value[i]);
      if(i<size-1)
        printf(",");
    }
  }
  if (verb >= VERB)
    printf("]\n");
  return FALSE;
} /* of 'fscanintarray' */ 

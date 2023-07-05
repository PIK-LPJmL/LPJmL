/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  a  r  r  a  y  . c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads an array from a LPJ file                                    \n**/
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

LPJfile *fscanarray(LPJfile *file,    /**< pointer to a LPJ file             */
                    int *size,        /**< size of array                     */
                    const char *name, /**< name of variable                  */
                    Verbosity verb    /**< verbosity level (NO_ERR,ERR,VERB) */
                  )                   /** \return array object or NULL on error */

{
  struct json_object *item;
  if(!json_object_object_get_ex(file,name,&item))
  {
    if(verb)
      fprintf(stderr,"ERROR225: Name '%s' not found.\n",name);
    return NULL;
  }
  if(json_object_get_type(item)!=json_type_array)
  {
    if(verb)
      fprintf(stderr,"ERROR226: Type of '%s' is not an array.\n",name);
    return NULL;
  }
  *size=json_object_array_length(item);
  return item;
} /* of 'fscanarray' */

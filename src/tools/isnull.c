/**************************************************************************************/
/**                                                                                \n**/
/**                                  i  s  n  u  l  l  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function determines whether JSON object is null                            \n**/
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

Bool isnull(const LPJfile *file, /**< pointer to a LPJ file             */
            const char *key      /**< key or NULL */
           )                     /** \return TRUE if JSON object is null */
{
  struct json_object *item;
  if(key==NULL)
    return json_object_get_type(file)==json_type_null;
  if(!json_object_object_get_ex(file,key,&item))
    return FALSE;
  return json_object_get_type(item)==json_type_null;
} /* of 'isnull' */

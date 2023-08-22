/**************************************************************************************/
/**                                                                                \n**/
/**                               i  s  i  n  t  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function determines whether JSON keyword is of type int                    \n**/
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
#include <json-c/json.h>
#include "types.h"

Bool isint(const LPJfile *file, /**< pointer to LPJ file */
           const char *name     /**< variable name or NULL */
          )                     /** \return TRUE if type is int */
{
  struct json_object *item;
  if(name==NULL)
    return (json_object_get_type(file)==json_type_int);
  if(!json_object_object_get_ex(file,name,&item))
    return FALSE;
  return (json_object_get_type(item)==json_type_int);
} /* of 'isint' */

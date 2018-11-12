/**************************************************************************************/
/**                                                                                \n**/
/**                   i  s  b  o  o  l  e  a  n  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function determines whether JSON keyword is of type boolean                \n**/
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
#ifdef USE_JSON
#include <json-c/json.h>
#endif
#include "types.h"

Bool isboolean(const LPJfile *file, /**< pointer to LPJ file */
               const char *name     /**< variable name */
              )                     /** \return TRUE if type is bool */
{
#ifdef USE_JSON
  struct json_object *item;
  if(file->isjson)
  {
    if(!json_object_object_get_ex(file->file.obj,name,&item))
      return FALSE;
    return (json_object_get_type(item)==json_type_boolean);
  }
  else
   return FALSE;
#else
   return FALSE;
#endif
} /* of 'isboolean' */

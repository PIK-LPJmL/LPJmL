/**************************************************************************************/
/**                                                                                \n**/
/**                   i  s  k  e  y  d  e  f  i  n  e  d  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function checks whether key is defined in JSON file                        \n**/
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

Bool iskeydefined(const LPJfile *file, /**< pointer to LPJ file */
                  const char *name     /**< variable name */
                 )                     /** \return TRUE if variable name exists */ 
{
#ifdef USE_JSON
  return (file->isjson && json_object_object_get_ex(file->file.obj,name,NULL));
#else
  return FALSE; /* feature is not supported for old '*.conf' files */
#endif
} /* of 'iskeydefined' */

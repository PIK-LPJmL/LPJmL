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
#ifdef USE_JSON
#include <json-c/json.h>
#endif
#include "types.h"

Bool isnull(const LPJfile *file /**< pointer to a LPJ file             */
           )                    /** \return TRUE if JSON object is null */
{
#ifdef USE_JSON
  return json_object_get_type(file->file.obj)==json_type_null;
#else
  return FALSE;
#endif
} /* of 'isnull' */

/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  a  r  r  a  y  i  n  d  e  x  .  c            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function gets array element from json object                               \n**/
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

LPJfile *fscanarrayindex(const LPJfile *file, /**< pointer to a LPJ file */
                         int index            /**< index in array */
                        )                     /** \return array element or NULL on error */
{
  return json_object_array_get_idx(file,index);
} /* of 'fscanarrayindex' */

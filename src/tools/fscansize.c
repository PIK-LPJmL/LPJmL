/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  s  i  z  e  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads a size_t value from a text file                             \n**/
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

Bool fscansize(LPJfile *file,    /**< pointer to LPJ file */
               size_t *value,    /**< value to be read from file */
               const char *name, /**< variable name */
               Bool with_default, /**< allow default value */
               Verbosity verb    /**< verbosity level (NO_ERR,ERR,VERB) */
              )                  /** \return TRUE on error */
{
  struct json_object *item;
  if(!json_object_object_get_ex(file,name,&item))
  {
    if(with_default)
    {
      if(verb)
        fprintf(stderr,"WARNING027: Name '%s' for size not found, set to %zu.\n",name,*value);
      return FALSE;
    }
    else
    {
      if(verb)
        fprintf(stderr,"ERROR225: Name '%s' for size not found.\n",name);
      return TRUE;
    }
  }
  if(json_object_get_type(item)!=json_type_int)
  {
    if(verb)
      fprintf(stderr,"ERROR226: Name '%s' not of type int.\n",name);
    return TRUE;
  }
  *value=json_object_get_int(item);
  if (verb >= VERB)
    printf("\"%s\" : %zu\n",name,*value);
  return FALSE;
} /* of 'fscansize' */

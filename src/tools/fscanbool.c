/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  b  o  o  l  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads a boolean value from a LPJ file                             \n**/
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

Bool fscanbool(LPJfile *file,    /**< pointer to LPJ file */
               Bool *value,      /**< boolean to be read from file */
               const char *name, /**< variable name */
               Bool withdefault, /**< allow default values */ 
               Verbosity verb    /**< verbosity level (NO_ERR,ERR,VERB) */
             )                   /** \return TRUE on error */
{
  struct json_object *item;
  if(!json_object_object_get_ex(file,name,&item))
  {
    if(withdefault)
    {
      if(verb)
        fprintf(stderr,"WARNING027: Name '%s' for boolean not found, set to %s.\n",name,bool2str(*value));
      return FALSE;
    }
    else
    {
      if(verb)
        fprintf(stderr,"ERROR225: Name '%s' for boolean not found.\n",name);
      return TRUE;
    }
  }
  if(json_object_get_type(item)!=json_type_boolean)
  {
    if(verb)
      fprintf(stderr,"ERROR226: Name '%s' not of type boolean.\n",name);
    return TRUE;
  }
  *value=json_object_get_boolean(item);
  if (verb >= VERB)
    printf("\"%s\" : %s\n", name, bool2str(*value));
  return FALSE;
} /* of 'fscanbool' */

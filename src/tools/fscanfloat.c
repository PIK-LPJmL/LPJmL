/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  f  l  o  a  t  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads a float value from a text file                              \n**/
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

Bool fscanfloat(LPJfile *file,    /**< pointer to LPJ file */
                float *value,     /**< float to be read from file */
                const char *name, /**< name of variable */
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
        fprintf(stderr,"WARNING027: Name '%s' for float not found, set to %g.\n",name,*value);
      return FALSE;
    }
    else
    {
      if(verb)
        fprintf(stderr,"ERROR225: Name '%s' for real not found.\n",name);
      return TRUE;
    }
  }
  if(json_object_get_type(item)!=json_type_double)
  {
    if(verb)
      fprintf(stderr,"ERROR226: Type of '%s' is not real.\n",name);
    return TRUE;
  }
  *value=(float)json_object_get_double(item);
  if (verb >= VERB)
    printf("\"%s\" : %g\n",name,*value);
  return FALSE;
} /* of 'fscanfloat' */

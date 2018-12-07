/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  s  t  r  u  c  t  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
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

Bool fscanstruct(const LPJfile *file, /**< pointer to LPJ file               */
                 LPJfile  *s,         /**< pointer to LPJ file struct        */
                 const char *name,    /**< name of variable                  */
                 Verbosity verb       /**< verbosity level (NO_ERR,ERR,VERB) */
                )                     /** \return TRUE on error              */
{
#ifdef USE_JSON
  struct json_object *item;
  if(file->isjson)
  {
    if(!json_object_object_get_ex(file->file.obj,name,&item))
    {
      if(verb)
        fprintf(stderr,"ERROR225: Name '%s' for object not found.\n",name);
      return TRUE;
    }
    if(json_object_get_type(item)!=json_type_object)
    {
      if(verb)
        fprintf(stderr,"ERROR226: Type of '%s' is not an object.\n",name);
      return TRUE;
    }
    s->isjson=TRUE;
    s->file.obj=item;
    return FALSE;
  }
#endif
  *s=*file;
  return FALSE;
} /* of 'fscanstruct' */

/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  a  r  r  a  y  i  n  d  e  x  .  c            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads a real value from a text file                               \n**/
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

Bool fscanarrayindex(const LPJfile *file, /**< pointer to a LPJ file             */
                     LPJfile  *s,   /**< real value read from file         */
                     int index,     /**< index in array                    */
                     Verbosity verb /**< verbosity level (NO_ERR,ERR,VERB) */
                    )               /** \return TRUE on error              */
{
#ifdef USE_JSON
  struct json_object *item;
  if(file->isjson)
  {
    item =json_object_array_get_idx(file->file.obj,index);
    if(item==NULL)
    {
      if(verb)
        fprintf(stderr,"ERROR101: Invalid index %d.\n",index);
      return TRUE;
    }
    s->isjson=TRUE;
    s->file.obj=item;
    return FALSE;
  }
#endif
  *s=*file;
  return FALSE;
} /* of 'fscanarrayindex' */

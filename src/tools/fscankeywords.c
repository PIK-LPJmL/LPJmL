/**************************************************************************************/
/**                                                                                \n**/
/**                   f  s  c  a  n  k  e  y  w  o  r  d  s  .  c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads keyword from a text file                                    \n**/
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

Bool fscankeywords(LPJfile *file,    /**< pointer to LPJ file */
                   int *value,       /**< integer to be read from file */
                   const char *name, /**< variable name */
                   const char *const *array, /**< array of keywords defined */
                   int size,          /**< size of array */
                   Bool with_default, /**< allow default value */
                   Verbosity verb    /**< verbosity level (NO_ERR,ERR,VERB) */
                  )                  /** \return TRUE on error */
{
  const char *str;
  int i;
  struct json_object *item;
  if(!json_object_object_get_ex(file,name,&item))
  {
    if(with_default)
    {
      if(verb)
        fprintf(stderr,"WARNING027: Name '%s' for keyword not found, set to \"%s\".\n",
                name,array[*value]);
      return FALSE;
    }
    else
    {
      if(verb)
        fprintf(stderr,"ERROR225: Name '%s' for keyword not found.\n",name);
      return TRUE;
    }
  }
  if(json_object_get_type(item)==json_type_string)
  {
    str=json_object_get_string(item);
    *value=findstr(str,array,size);
    if(*value==NOT_FOUND)
    {
      if(verb)
      {
        fprintf(stderr,"ERROR166: Keyword '%s' not defined for name '%s', must be",str,name);
        for(i=0;i<size;i++)
        {
          fprintf(stderr," \"%s\"",array[i]);
          if(i!=size-1)
            fputc(',',stderr);
        }
        fputs(".\n",stderr);
      }
      return TRUE;
    }
    return FALSE;
  }
  if(json_object_get_type(item)!=json_type_int)
  {
    if(verb)
      fprintf(stderr,"ERROR226: Name '%s' not of type int.\n",name);
    return TRUE;
  }
  *value=json_object_get_int(item);
  if(*value<0 || *value>=size)
  {
    if(verb)
    {
      fprintf(stderr,"ERROR226: Invalid value %d for name '%s', must be in [0,%d] or",*value,name,size-1);
      for(i=0;i<size;i++)
      {
        fprintf(stderr," \"%s\"",array[i]);
        if(i!=size-1)
          fputc(',',stderr);
      }
      fputs(".\n",stderr);
    }
    return TRUE;
  }
  if (verb >= VERB)
    printf("\"%s\" : %d\n", name, *value);
  return FALSE;
} /* of 'fscankeywords' */

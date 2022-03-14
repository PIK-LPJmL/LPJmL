/**************************************************************************************/
/**                                                                                \n**/
/**             f  s  c  a  n  s  t  r  i  n  g  a  r  r  a  y  .  c               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads a string array from a LPJ file                              \n**/
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
#include "lpj.h"

List *fscanstringarray(LPJfile *file,   /**< pointer to LPJ file */
                       const char *key, /**< name of array */
                       Verbosity verb   /**< verbosity level (NO_ERR,ERR,VERB) */
                      )                 /** \return array of strings or NULL */
{
  List *list;
  int i,size;
  struct json_object *array,*item;
  if(!json_object_object_get_ex(file,key,&array))
  {
    if(verb)
      fprintf(stderr,"ERROR225: Name '%s' for array not found.\n",key);
    return NULL;
  }
  if(json_object_get_type(array)!=json_type_array)
  {
    if(verb)
      fprintf(stderr,"ERROR226: Type of '%s' is not an array.\n",key);
    return NULL;
  }
  size=json_object_array_length(array);
  if (verb >= VERB)
    printf("\"%s\" : [",key);
  list=newlist(size);
  if(list==NULL)
  {
    printallocerr("array");
    return NULL;
  }
  for(i=0;i<size;i++)
  {
    item =json_object_array_get_idx(array,i);
    if(json_object_get_type(item)==json_type_null)
      getlistitem(list,i)=NULL;
    else if(json_object_get_type(item)==json_type_string)
    {
      getlistitem(list,i)=strdup(json_object_get_string(item));
      if(getlistitem(list,i)==NULL)
        printallocerr("array");
    }
    else
    {
      if(verb)
        fprintf(stderr,"ERROR226: Type of item %d in array '%s' is not string.\n",i,key);
      return NULL;
    }
    if (verb >= VERB)
    {
      printf(" \"%s\"",(getlistitem(list,i)==NULL) ? "null" : (char *)getlistitem(list,i));
      if(i<size-1)
        printf(",");
    }
  }
  if (verb >= VERB)
    printf("]\n");
  return list;
} /* of 'fscanstringarray' */

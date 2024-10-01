/**************************************************************************************/
/**                                                                                \n**/
/**                     f  s  c  a  n  m  a  p  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads a string or double array from a LPJ file                    \n**/
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

Map *fscanmap(LPJfile *file,   /**< pointer to LPJ file */
              const char *key, /**< name of array */
              Verbosity verb   /**< verbosity level (NO_ERR,ERR,VERB) */
             )                 /** \return array of strings or NULL */
{
  Map *map;
  double *value;
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
  map=new(Map);
  map->isfloat=FALSE;
  size=json_object_array_length(array);
  if (verb >= VERB)
    printf("\"%s\" : [",key);
  map->list=newlist(size);
  if(map->list==NULL)
  {
    printallocerr("array");
    free(map);
    return NULL;
  }
  for(i=0;i<size;i++)
    getlistitem(map->list,i)=NULL;
  for(i=0;i<size;i++)
  {
    item =json_object_array_get_idx(array,i);
    if(json_object_get_type(item)==json_type_null)
    {
      if(map->isfloat)
      {
        if(verb)
          fprintf(stderr,"ERROR226: Type of item %d in array '%s' is null, not float.\n",i,key);
        freemap(map);
        return NULL;
     }
    }
    else if(json_object_get_type(item)==json_type_string)
    {
      if(i && map->isfloat)
      {
        if(verb)
          fprintf(stderr,"ERROR226: Type of item %d in array '%s' is not float.\n",i,key);
        freemap(map);
        return NULL;
      }
      getlistitem(map->list,i)=strdup(json_object_get_string(item));
      if(getlistitem(map->list,i)==NULL)
      {
        printallocerr("array");
        freemap(map);
        return NULL;
      }
    }
    else if(json_object_get_type(item)==json_type_double)
    {
      if(i && !map->isfloat)
      {
        if(verb)
          fprintf(stderr,"ERROR226: Type of item %d in array '%s' is not string.\n",i,key);
        freemap(map);
        return NULL;
      }
      map->isfloat=TRUE;
      value=new(double);
      if(value==NULL)
      {
        printallocerr("value");
        freemap(map);
        return NULL;
      }
      *value=json_object_get_double(item);
      getlistitem(map->list,i)=value;
    }
    else if(json_object_get_type(item)==json_type_int)
    {
      if(i && !map->isfloat)
      {
        if(verb)
          fprintf(stderr,"ERROR226: Type of item %d in array '%s' is not string.\n",i,key);
        freemap(map);
        return NULL;
      }
      map->isfloat=TRUE;
      value=new(double);
      if(value==NULL)
      {
        printallocerr("value");
        freemap(map);
        return NULL;
      }
      *value=json_object_get_int(item);
      getlistitem(map->list,i)=value;
    }
    else
    {
      if(verb)
        fprintf(stderr,"ERROR226: Type of item %d in array '%s' is not string or float.\n",i,key);
      freemap(map);
      return NULL;
    }
    if (verb >= VERB)
    {
      if(map->isfloat)
        printf(" %g",*((double *)getlistitem(map->list,i)));
      else
        printf(" \"%s\"",(getlistitem(map->list,i)==NULL) ? "null" : (char *)getlistitem(map->list,i));
      if(i<size-1)
        printf(",");
    }
  }
  if (verb >= VERB)
    printf("]\n");
  return map;
} /* of 'fscanmap' */

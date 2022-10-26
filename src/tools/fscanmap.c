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
#ifdef USE_JSON
#include <json-c/json.h>
#endif
#include "lpj.h"

Map *fscanmap(LPJfile *file,   /**< pointer to LPJ file */
              const char *key, /**< name of array */
              Verbosity verb   /**< verbosity level (NO_ERR,ERR,VERB) */
             )                 /** \return array of strings or NULL */
{
  String name;
  Map *map;
  double *value;
  int i,size;
#ifdef USE_JSON
  struct json_object *array,*item;
  if(file->isjson)
  {
    if(!json_object_object_get_ex(file->file.obj,key,&array))
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
      return NULL;
    }
    for(i=0;i<size;i++)
    {
      item =json_object_array_get_idx(array,i);
      if(json_object_get_type(item)==json_type_null)
      {
        if(map->isfloat)
        {
          if(verb)
            fprintf(stderr,"ERROR226: Type of item %d in array '%s' is null, not float.\n",i,key);
          return NULL;
       }
        getlistitem(map->list,i)=NULL;
      }
      else if(json_object_get_type(item)==json_type_string)
      {
        if(i && map->isfloat)
        {
          if(verb)
            fprintf(stderr,"ERROR226: Type of item %d in array '%s' is not float.\n",i,key);
          return NULL;
        }
        getlistitem(map->list,i)=strdup(json_object_get_string(item));
        if(getlistitem(map->list,i)==NULL)
          printallocerr("array");
      }
      else if(json_object_get_type(item)==json_type_double)
      {
        if(i && !map->isfloat)
        {
          if(verb)
            fprintf(stderr,"ERROR226: Type of item %d in array '%s' is not string.\n",i,key);
          return NULL;
        }
        map->isfloat=TRUE;
        value=new(double);
        if(value==NULL)
        {
          printallocerr("value");
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
          return NULL;
        }
        map->isfloat=TRUE;
        value=new(double);
        if(value==NULL)
        {
          printallocerr("value");
          return NULL;
        }
        *value=json_object_get_int(item);
        getlistitem(map->list,i)=value;
      }
      else
      {
        if(verb)
          fprintf(stderr,"ERROR226: Type of item %d in array '%s' is not string or float.\n",i,key);
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
  }
#endif
  map=new(Map);
  if(fscanint(file,&size,"size",FALSE,verb))
    return NULL;
  map->list=newlist(size);
  map->isfloat=FALSE;
  if(map->list==NULL)
  {
    printallocerr("array");
    return NULL;
  }
  for(i=0;i<size;i++)
  {
    if(fscanstring(file,name,key,FALSE,verb))
       return NULL;
    getlistitem(map->list,i)=strdup(name);
    if(getlistitem(map->list,i)==NULL)
      printallocerr("array");
  }
  return map;
} /* of 'fscanmap' */

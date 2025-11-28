/**************************************************************************************/
/**                                                                                \n**/
/**                       l  i  s  t  .  c                                         \n**/
/**                                                                                \n**/
/**     C implementation of a resizeable array of generic pointers                 \n**/
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
#include "errmsg.h"
#include "list.h"
#include "types.h"

List *newlist(int size /**< size of list */
             )         /** \return pointer to list of size size */
{
  List *list;
  list=(List *)malloc(sizeof(List));
  if(list==NULL)
    return NULL;
  list->n=size;
  if(size==0)
    list->data=NULL;
  else
  {
    list->data=malloc(size*sizeof(void *));
    if(list->data==NULL)
    {
      free(list);
      list=NULL;
    }
  }
  return list;
} /* of 'newlist' */

int addlistitem(List *list, /**< pointer to list */
                void *item  /**< item to be added at end of list */
               )            /** \return updated length of list or 0 in case of error */
{
  void **ptr;
  ptr=(void **)realloc(list->data,sizeof(void *)*(list->n+1));
  if(ptr==NULL)
    return 0;
  list->data=ptr;
  list->data[list->n++]=item;
  return list->n;
} /* of 'addlistitem' */

int dellistitem(List *list, /**< pointer to list */
                int index   /**< index of item to be deleted from list */
               )            /** \return updated length of list */
{
  /* does not check for empty list or index out of range if SAFE not defined */
#ifdef SAFE
  if(isempty(list))
    fail(IS_EMPTY_ERR,TRUE,TRUE,"list is empty in dellistitem()");
  if(index<0 || index>=list->n)
    fail(OUT_OF_RANGE_ERR,TRUE,TRUE,"index=%d out of range in dellistitem()",index);
#endif
  list->n--;
  list->data[index]=list->data[list->n];
  if(isempty(list))
  {
    free(list->data);
    list->data=NULL;
  }
  else
    list->data=(void **)realloc(list->data,sizeof(void *)*list->n);
  return list->n;
} /* of 'dellistitem' */

void freelist(List *list /**<  pointer to list */
             )
{
  /* functions frees memory of list */
  if(!isempty(list))
    free(list->data);
  free(list);
} /* of 'freelist' */

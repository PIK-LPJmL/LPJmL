/**************************************************************************************/
/**                                                                                \n**/
/**                       i  n  t  l  i  s  t  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of a resizeable integer array                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "errmsg.h"
#include "intlist.h"

void initintlist(Intlist *list /**< pointer to integer list */)
{
  list->n=0;
  list->index=NULL;
} /* of 'initintlist' */

Bool copyintlist(Intlist *dst,const Intlist *src)
{
  int i;
  dst->n=src->n;
  if(dst->n)
  {
    dst->index=newvec(int,dst->n);
    if(dst->index==NULL)
    {
      dst->n=0;
      return TRUE;
    }
    else
      for(i=0;i<dst->n;i++)
        dst->index[i]=src->index[i];
  }
  else
    dst->index=NULL;
  return FALSE;
} /* of 'copyintlist' */

int addintlistitem(Intlist *list, /**< pointer to integer list */
                   int item       /**< item to be added at end of list */
                  )               /** \return updated length of list or zero */

{
  int *newptr;
  newptr=(int *)realloc(list->index,sizeof(int)*(list->n+1));
  /* check, whether realloc was successful */
  if(newptr==NULL)
    return 0; /* no, return zero */
  list->index=newptr;
  list->index[list->n]=item;
  list->n++;
  return list->n;
} /* of 'addintlistitem' */

int delintlistitem(Intlist *list, /**< pointer to integer list */
                   int index      /**< index of item to be deleted from list */
                  )               /** \return updated length of list */

{
  /* does not check for empty list if SAFE not defined */
#ifdef SAFE
  if(isemptyintlist(list))
    fail(IS_EMPTY_ERR,TRUE,"list is empty in delintlistitem()");
  if(index<0 || index>=list->n)
    fail(OUT_OF_RANGE_ERR,TRUE,"index=%d out of range in delintlistitem()",index);
#endif
  list->n--;
  list->index[index]=list->index[list->n];
  if(isemptyintlist(list))
  {
    free(list->index);
    list->index=NULL;
  }
  else
    list->index=(int *)realloc(list->index,sizeof(int)*list->n);
  return list->n;
} /* of 'delintlistitem' */

void emptyintlist(Intlist *list /**< pointer to integer list */
                 )
{
  if(!isemptyintlist(list))
    free(list->index);
  list->index=NULL;
  list->n=0;
} /* of 'emptyintlist' */

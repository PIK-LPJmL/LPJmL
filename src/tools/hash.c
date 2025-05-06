/**************************************************************************************/
/**                                                                                \n**/
/**                          h  a  s  h  .  h                                      \n**/
/**                                                                                \n**/
/**     C implementation of a hash                                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "hash.h"

struct hashitem
{
  void *data;
  void *key;
  struct hashitem *next;
};

struct hash
{
  int size;                                 /**< hash size */
  int count;                                /**< number of objects in hash */
  struct hashitem **array;                  /**< hash array */
  int (*hashfcn)(const void *,int);         /**< hash funtion */
  int (*cmpfcn)(const void *,const void *); /**< compare function */
  void (*freefcn)(void *);                  /**< function to deallocate data */
};

Hash newhash(int size, /**< size of hash */
             int (*hashfcn)(const void *,int),
             int (*cmpfcn)(const void *,const void *),
             void (*freefcn)(void *)
            ) /** \return allocated hash or NULL on error */
{
  int i;
  Hash hash;
  hash=new(struct hash);
  if(hash==NULL)
    return NULL;
  hash->size=size;
  hash->count=0;
  hash->hashfcn=hashfcn;
  hash->cmpfcn=cmpfcn;
  hash->freefcn=freefcn;
  if(size)
  {
    hash->array=newvec(struct hashitem *,size);
    if(hash->array==NULL)
    {
      free(hash);
      return NULL;
    }
    for(i=0;i<size;i++)
      hash->array[i]=NULL;
  }
  else
    hash->array=NULL;
  return hash;
} /* of 'newhash' */

int addhashitem(Hash hash, /**< pointer to hash */
                void *key, /**< key for data */
                void *data /**< pointer to data to store */
               )           /** \return number of objects in hash or 0 on error */
{
  /* Function adds object to hash */
  int index;
  struct hashitem *item;
  if(key!=NULL)
  {
    index=(*hash->hashfcn)(key,hash->size);
    item=new(struct hashitem);
    if(item==NULL)
      return 0;
    item->key=key;
    item->data=data;
    item->next=hash->array[index];
    hash->array[index]=item;
    hash->count++;
    return hash->count;
  }
  return 0;
} /* of 'addhashitem' */

Bool removehashitem(Hash hash,const void *key)
{
  /* Function removes object from hash */
  int index;
  struct hashitem *item,*prev;
  if(key!=NULL)
  {
    index=(*hash->hashfcn)(key,hash->size);
    for(item=prev=hash->array[index];item!=NULL;item=item->next)
    {
      if((*hash->cmpfcn)(key,item->key)==0)
      {
        if(item==prev)
          hash->array[index]=item->next;
        else
          prev->next=item->next;
        free(item->key);
        (*hash->freefcn)(item->data);
        free(item);
        hash->count--;
        return FALSE;
      }
      prev=item;
    }
  }
  return TRUE;
} /* of 'removehashitem' */

int gethashcount(const Hash hash)
{
  /* Function returns number of objects stored in hash */
  return hash->count;
} /* of 'gethashcount' */

void *gethashitem(Hash hash,      /**< pointer to hash */
                  const void *key /**< key to find in hash */
                 )                /** \return pointer to data found or NULL */
{
  /* Function gets object from hash */
  int index;
  struct hashitem *item;
  if(key!=NULL)
  {
    index=(*hash->hashfcn)(key,hash->size);
    for(item=hash->array[index];item!=NULL;item=item->next)
      if((*hash->cmpfcn)(key,item->key)==0)
        return item->data;
  }
  return NULL;
} /* of 'gethashitem' */

Hashitem *hash2array(const Hash hash /**< pointer to hash */
                    )                /** \return allocated array of names or NULL on error */
{
  Hashitem *hashitem;
  struct hashitem *item,*next;
  int i,count=0;
  hashitem=newvec(Hashitem,hash->count);
  if(hashitem==NULL)
    return NULL;
  for(i=0;i<hash->size;i++)
  {
    for(item=hash->array[i];item!=NULL;item=next)
    {
      hashitem[count].key=item->key;
      hashitem[count].data=item->data;
      next=item->next;
      count++;
    }
  }
  return hashitem;
} /* of 'hash2array' */

void deletehash(Hash hash)
{
  /* Function removes all obects in hash */
  int i;
  struct hashitem *item,*next;
  hash->count=0;
  for(i=0;i<hash->size;i++)
  {
    for(item=hash->array[i];item!=NULL;item=next)
    {
       next=item->next;
       free(item->key);
       (*hash->freefcn)(item->data);
       free(item);
    }
    hash->array[i]=NULL;
  }
} /* of 'deletehash' */

void freehash(Hash hash)
{
  /* Function deallocates all memory of hash */
  int i;
  struct hashitem *item,*next;
  if(hash!=NULL)
  {
    for(i=0;i<hash->size;i++)
      for(item=hash->array[i];item!=NULL;item=next)
      {
         next=item->next;
         free(item->key);
         (*hash->freefcn)(item->data);
         free(item);
      }
    if(hash->size)
      free(hash->array);
    free(hash);
  }
} /* of 'freehash' */

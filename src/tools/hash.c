/**************************************************************************************/
/**                                                                                \n**/
/**                          h  a  s  h  .  h                                      \n**/
/**                                                                                \n**/
/**     C implementation of a hash table to store key/value items in a dictionary  \n**/
/**     key string is converted via hash function to index in an array. If hash    \n**/
/**     value has been already used, key/values is added to a linked list          \n**/
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
#include <string.h>
#include "types.h"
#include "hash.h"

struct hashitem
{
  void *data;            /**< data stored for key */
  char *key;             /**< key */
  struct hashitem *next; /**< pointer to next key with same hash value or NULL */
};

struct hash
{
  int size;                                 /**< hash size */
  int count;                                /**< number of objects in hash */
  struct hashitem **array;                  /**< hash array of linked lists */
  int (*hashfcn)(const char *,int);         /**< hash function */
  void (*freefcn)(void *);                  /**< function to deallocate data */
}; /**< Definition of opaque datatype Hash */

Hash newhash(int size,                         /**< size of hash */
             int (*hashfcn)(const char *,int), /**< hash function */
             void (*freefcn)(void *)           /**< function to deallocate data */
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
  hash->freefcn=freefcn;
  if(size>0)
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
  {
    free(hash);
    return NULL;
  }
  return hash;
} /* of 'newhash' */

int addhashitem(Hash hash, /**< pointer to hash */
                char *key, /**< key for data */
                void *data /**< pointer to data to store */
               )           /** \return number of objects in hash or 0 on error */
{
  /* Function adds object to hash */
  int index;
  struct hashitem *item;
  if(key!=NULL)
  {
    /* calculate hash from key */
    index=(*hash->hashfcn)(key,hash->size);
    /* add item to the linnked list */
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

Bool removehashitem(Hash hash,      /**< pointer to hash */
                    const char *key /**< key of item to remove from hash */
                   )                /** \return TRUE on error */
{
  /* Function removes object from hash */
  int index;
  struct hashitem *item,*prev;
  if(key!=NULL)
  {
    index=(*hash->hashfcn)(key,hash->size);
    for(item=prev=hash->array[index];item!=NULL;item=item->next)
    {
      if(!strcmp(key,item->key))
      {
        /* key found delete from linked list */
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

int gethashcount(const Hash hash /**< pointer to hash */
                )                /** \return number of stored items in hash */
{
  /* Function returns number of objects stored in hash */
  return hash->count;
} /* of 'gethashcount' */

void *gethashitem(Hash hash,      /**< pointer to hash */
                  const char *key /**< key to find in hash */
                 )                /** \return pointer to data found or NULL */
{
  /* Function gets object from hash */
  int index;
  struct hashitem *item;
  if(key!=NULL)
  {
    index=(*hash->hashfcn)(key,hash->size);
    for(item=hash->array[index];item!=NULL;item=item->next)
      if(!strcmp(key,item->key))
        return item->data;
  }
  return NULL;
} /* of 'gethashitem' */

Hashitem *hash2array(const Hash hash /**< pointer to hash */
                    )                /** \return allocated array of names or NULL on error */
{
  Hashitem *hashitem;
  struct hashitem *item;
  int i,count=0;
  hashitem=newvec(Hashitem,hash->count);
  if(hashitem==NULL)
    return NULL;
  for(i=0;i<hash->size;i++)
  {
    for(item=hash->array[i];item!=NULL;item=item->next)
    {
      hashitem[count].key=item->key;
      hashitem[count].data=item->data;
      count++;
    }
  }
  return hashitem;
} /* of 'hash2array' */

void deletehash(Hash hash)
{
  /* Function removes all objects in hash */
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
  if(hash!=NULL)
  {
    deletehash(hash);
    free(hash->array);
    free(hash);
  }
} /* of 'freehash' */

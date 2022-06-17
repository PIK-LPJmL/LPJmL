/**************************************************************************************/
/**                                                                                \n**/
/**                       q  u  e  u  e  .  c                                      \n**/
/**                                                                                \n**/
/**     C implementation of a Real valued delay queue                              \n**/
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
#include "types.h"
#include "swap.h"
#include "queue.h"

struct queue
{
  Real *data; /* data array */
  int count;  /* number of Real values per queue element */
  int size;   /* length of queue */
  int first;  /* index of first element in queue */
}; /* definition of opaque datatype Queue */


Queue newqueue(int count, /* number of Real values per queue element */
               int size   /* length of queue */
              )           /* returns NULL on error */
{
  Queue queue;
  int i;
  /* size of queue must be >0 */
  if(size<1 || count<1)
    return NULL;
  queue=new(struct queue);
  if(queue==NULL)
    return NULL;
  /* allocate data */
  queue->data=newvec(Real,size*count);
  if(queue->data==NULL)
  {
     free(queue);
     return NULL;
  }
  /* initialize queue with zeros */
  for(i=0;i<size*count;i++)
    queue->data[i]=0;
  queue->size=size;
  queue->count=count;
  queue->first=size-1;
  return queue;
} /* of 'newqueue' */

Bool fwritequeue(FILE *file,const Queue queue)
{
  fwrite(&queue->size,sizeof(int),1,file);
  fwrite(&queue->count,sizeof(int),1,file);
  fwrite(&queue->first,sizeof(int),1,file);
  return fwrite(queue->data,sizeof(Real),queue->size*queue->count,file)!=queue->size*queue->count;
} /* of 'fwritequeue' */

void fprintqueue(FILE *file,const Queue queue)
{
  int i,j;
  for(j=0;j<queue->count;j++)
  {
    fprintf(file,"%d:",j);
    for(i=0;i<queue->size;i++)
      fprintf(file," %g",queue->data[((queue->first+i) % queue->size)*queue->count+j]);
    fputc('\n',file);
  }
} /* of 'fprintqueue' */

Queue freadqueue(FILE *file,Bool swap)
{
  Queue queue;
  queue=new(struct queue);
  if(queue==NULL)
    return NULL;
  if(freadint1(&queue->size,swap,file)!=1)
  {
    free(queue);
    return NULL;
  }
  if(freadint1(&queue->count,swap,file)!=1)
  {
    free(queue);
    return NULL;
  }
  if(freadint1(&queue->first,swap,file)!=1)
  {
    free(queue);
    return NULL;
  }
  if(queue->first<0 || queue->first>=queue->size)
  {
    free(queue);
    return NULL;
  }
  queue->data=newvec(Real,queue->size*queue->count);
  if(queue->data==NULL)
  {
    free(queue);
    return NULL;
  }
  if(freadreal(queue->data,queue->size*queue->count,swap,file)!=queue->size*queue->count)
  {
    free(queue->data);
    free(queue);
    return NULL;
  }
  return queue;
} /* of 'freadqueue' */

Bool skipqueue(FILE *file, /**< pointer to binary file */
               Bool swap   /**< byte order has to be swapped */
              )            /** \return TRUE on error */
{
  int size,count;
  if(freadint1(&size,swap,file)!=1)
    return TRUE;
  if(freadint1(&count,swap,file)!=1)
    return TRUE;
  return fseek(file,sizeof(int)+sizeof(Real)*size*count,SEEK_CUR);
} /* of 'skipqueue' */

void getqueue(const Queue queue, /* pointer to queue */
              Real val[],        /* values to be read from queue */
              int index          /* index of requested queue element */
             )
{
  int i;
  for(i=0;i<queue->count;i++)
    val[i]=queue->data[((queue->first+index) % queue->size)*queue->count+i];
} /* of 'getqueue' */

void setqueue(Queue queue,      /* pointer to queue */
              const Real val[], /* values to written to queue */
              int index         /* index of requested queue element */
             )
{
  int i;
  for(i=0;i<queue->count;i++)
    queue->data[((queue->first+index) % queue->size)*queue->count+i]=val[i];
} /* of 'setqueue' */

int queuesize(const Queue queue)
{
  return queue->size;
} /* of 'queuesize' */

void putqueue(Queue queue,     /* pointer to queue */
              const Real val[] /* values to be put in queue */
             )
{
  int i;
  /*
   * move index of first element and store val there
   */
  queue->first=(queue->first-1+queue->size) % queue->size;
  for(i=0;i<queue->count;i++)
    queue->data[queue->first*queue->count+i]=val[i];
} /* of 'putqueue' */

Real sumqueue(const Queue queue, /* pointer to queue */
              int index          /* index of queue element summed up */
             )                   /* returns total sum */
{
  int i;
  Real sum;
  sum=0;

  for(i=0;i<queue->size;i++)
    sum+=queue->data[i*queue->count+index];
  return sum;
} /* of 'sumqueue' */

void allsumqueue(const Queue queue, /* pointer to queue */
                 Real sum[]         /* sum of queue elements */
                )
{
  int i,j,k;
  /* initialize array to zero */
  for(i=0;i<queue->count;i++)
    sum[i]=0;
  k=0;
  for(i=0;i<queue->size;i++)
    for(j=0;j<queue->count;j++)
      sum[j]+=queue->data[k++];
} /* of 'allsumqueue' */

void freequeue(Queue queue)
{
  if(queue!=NULL)
  {
    free(queue->data);
    free(queue);
  }
} /* of 'freequeue' */

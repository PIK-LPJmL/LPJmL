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
#include "hash.h"
#include "bstruct.h"
#include "swap.h"
#include "errmsg.h"
#include "queue.h"

struct queue
{
  Real *data; /**< data array */
  int count;  /**< number of Real values per queue element */
  int size;   /**< length of queue */
  int first;  /**< index of first element in queue */
}; /**< definition of opaque datatype Queue */


Queue newqueue(int count, /**< number of Real values per queue element */
               int size   /**< length of queue */
              )           /** \return pointer to queue or NULL on error */
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

Bool fwritequeue(Bstruct file,    /**< pointer to restart file */
                const char *name, /**< name of object */
                const Queue queue /**< pointer to queue written */
               )                  /** \return TRUE on error */
{
  int i,j;
  bstruct_writeint(file,"count",queue->count);
  bstruct_writebeginarray(file,name,queue->size*queue->count);
  for(j=0;j<queue->count;j++)
    for(i=0;i<queue->size;i++)
      if(bstruct_writereal(file,NULL,queue->data[((queue->first+i) % queue->size)*queue->count+j]))
        return TRUE;
  return bstruct_writeendarray(file);
} /* of 'fwritequeue' */

void fprintqueue(FILE *file,       /**< pointer to text file */
                 const Queue queue /**< pointer to queue */
                )
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

Queue freadqueue(Bstruct file,    /**< pointer to restart file */
                 const char *name /**< name of object */
                )                 /** \return pointer to queue read or NULL */
{
  Queue queue;
  int size;
  queue=new(struct queue);
  if(queue==NULL)
  {
    printallocerr("queue");
    return NULL;
  }
  if(bstruct_readint(file,"count",&queue->count))
  {
    free(queue);
    return NULL;
  }
  queue->data=bstruct_readvarrealarray(file,name,&size);
  if(queue->data==NULL)
  {
    free(queue);
    return NULL;
  }
  queue->size=size/queue->count;
  queue->first=0;
  return queue;
} /* of 'freadqueue' */

void getqueue(const Queue queue, /**< pointer to queue */
              Real val[],        /**< values to be read from queue */
              int index          /**< index of requested queue element */
             )
{
  int i;
  for(i=0;i<queue->count;i++)
    val[i]=queue->data[((queue->first+index) % queue->size)*queue->count+i];
} /* of 'getqueue' */

void setqueue(Queue queue,      /**< pointer to queue */
              const Real val[], /**< values to written to queue */
              int index         /**< index of requested queue element */
             )
{
  int i;
  for(i=0;i<queue->count;i++)
    queue->data[((queue->first+index) % queue->size)*queue->count+i]=val[i];
} /* of 'setqueue' */

int queuesize(const Queue queue /**< pointer to queue */
             )                  /** \return size of queue */
{
  return queue->size;
} /* of 'queuesize' */

void putqueue(Queue queue,     /**< pointer to queue */
              const Real val[] /**< values to be put in queue */
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

Real sumqueue(const Queue queue, /**< pointer to queue */
              int index          /**< index of queue element summed up */
             )                   /** \return total sum */
{
  int i;
  Real sum;
  sum=0;

  for(i=0;i<queue->size;i++)
    sum+=queue->data[i*queue->count+index];
  return sum;
} /* of 'sumqueue' */

Real weighted_fireduration_from_queue(const Queue queue) /**< pointer to queue**/
{
  int i;
  Real weightsum=0;
  Real sum=0;

  for(i=0;i<queue->size;i++){
    weightsum+=queue->data[queue->count*(i+1)-1] * queue->data[i*queue->count];
    sum+=queue->data[i*queue->count];
  }
  return (sum > 0.0) ?  weightsum/sum : 0.0;
}

Real max_fireduration_from_queue(const Queue queue) /**< pointer to queue**/
{
  int i;
  Real max=0;
  for(i=0;i<queue->size;i++){
    max=max(max,queue->data[queue->count*(i+1)-1]);
  }
  return max;
}

void allsumqueue(const Queue queue, /**< pointer to queue */
                 Real sum[]         /** \return sum of queue elements */
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

void freequeue(Queue queue /**< pointer to queue */
              )
{
  if(queue!=NULL)
  {
    free(queue->data);
    free(queue);
  }
} /* of 'freequeue' */

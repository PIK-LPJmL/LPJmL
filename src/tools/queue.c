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
  Real *data; /**< data array */
  int size;   /**< size of queue */
  int first;  /**< index of first element in queue */
}; /* definition of opaque datatype Queue */

Queue newqueue(int size /**< size of queue */
              )         /** \return pointer to queue or NULL on error */
{
  Queue queue;
  int i;
  /* size of queue must be >0 */
  if(size<1)
    return NULL;
  /* allocate data */
  queue=new(struct queue);
  if(queue==NULL)
    return NULL;
  queue->data=newvec(Real,size);
  if(queue->data==NULL)
  {
    free(queue);
    return NULL;
  }
  /* initialize queue with zeros */
  for(i=0;i<size;i++)
    queue->data[i]=0;
  queue->size=size;
  queue->first=size-1;
  return queue;
} /* of 'newqueue' */

Bool fwritequeue(FILE *file, /**< pointer to binary file */
                 const Queue queue /**< pointer to queue written */
                )                  /** \return TRUE on error */
{
  fwrite(&queue->size,sizeof(int),1,file);
  fwrite(&queue->first,sizeof(int),1,file);
  return fwrite(queue->data,sizeof(Real),queue->size,file)!=queue->size;
} /* of 'fwritequeue' */

void fprintqueue(FILE *file,       /**< pointer to text file */
                 const Queue queue /**< pointer to queue to print */
                )
{
  int i;
  for(i=0;i<queue->size;i++)
    fprintf(file," %g",queue->data[(queue->first+i) % queue->size]);
} /* of 'fprintqueue' */

Queue freadqueue(FILE *file, /**< pointer to binary file */
                 Bool swap   /**< byte order has to be swapped */
                )            /** \return pointer to queue read or NULL */
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
  queue->data=newvec(Real,queue->size);
  if(queue->data==NULL)
  {
    free(queue);
    return NULL;
  }
  if(freadreal(queue->data,queue->size,swap,file)!=queue->size)
  {
    free(queue->data);
    free(queue);
    return NULL;
  }
  return queue; 
} /* of 'freadqueue' */
 
Real getqueue(const Queue queue, /**< pointer to queue */
              int i              /**< index of requested queue element */
             )                   /** \return first element in queue */
{
  return queue->data[(queue->first+i) % queue->size];
} /* of 'getqueue' */

int queuesize(const Queue queue /**< pointer to queue */
              )                 /** \return size of queue */
{
  return queue->size;
} /* of 'queuesize' */

void putqueue(Queue queue, /**< pointer to queue */
              Real val     /**< value to be put in queue */
             )
{
  /*
   * move index of first element and store val there
   */
  queue->first=(queue->first-1+queue->size) % queue->size;
  queue->data[queue->first]=val;
} /* of 'putqueue' */

Real sumqueue(const Queue queue /**< pointer to queue */
             )                  /** \return total sum */
{
  int i;
  Real sum;
  sum=0;

  for(i=0;i<queue->size;i++)
    sum+=queue->data[i];
  return sum;
} /* of 'sumqueue' */

void freequeue(Queue queue /**< pointer to queue */
              )
{
  if(queue!=NULL)
  {
    free(queue->data);
    free(queue);
  }
} /* of 'freequeue' */

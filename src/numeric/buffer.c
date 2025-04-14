/**************************************************************************************/
/**                                                                                \n**/
/**                     b  u  f  f  e  r  .  c                                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
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
#include <errno.h>
#include "types.h"
#include "bstruct.h"   /* Definition of datatype Real  */
#include "errmsg.h"
#include "buffer.h"

struct buffer
{
  int index;  /**< index to last element in buffer */
  int n;      /**< actual size of buffer  */
  int size;   /**< maximum size of buffer */
  Real sum;   /**< sum of all values in buffer */
  Real *data; /**< data storage */
}; /* definition of opaque datatype Buffer */

Buffer newbuffer(int size /**< size of buffer */
                )         /** \return allocated buffer or NULL */
{
  Buffer buffer;
  buffer=new(struct buffer);
  if(buffer==NULL)
    return NULL;
  buffer->size=size;
  buffer->n=buffer->index=0;
  if((buffer->data=newvec(Real,size))==NULL)
  {
    free(buffer);
    return NULL;
  }
  buffer->sum=0;
  return buffer;
} /* of 'newbuffer' */

void updatebuffer(Buffer buffer, /**< pointer to buffer */
                  Real val       /**< value to be added to buffer */
                 )               /** \return average value */
{
  if(buffer->n<buffer->size) /* actual buffer size less than maximum buffer size? */
  {
    /* yes, add item to buffer */
    buffer->data[buffer->n]=val;
    buffer->n++;
    buffer->sum+=val;
  }
  else
  {
    /* no, replace oldest item in buffer by new item */
    buffer->sum-=buffer->data[buffer->index];
    buffer->data[buffer->index]=val;
    buffer->index=(buffer->index+1)% buffer->size;
    buffer->sum+=val;
  }
} /* of 'updatebuffer' */

Bool fwritebuffer(Bstruct file,       /**< pointer to restart file */
                  const char *name,   /**< name of object */
                  const Buffer buffer /**< pointer to buffer */
                 )                    /** \return TRUE on error */
{
  bstruct_writestruct(file,name);
  bstruct_writeint(file,"size",buffer->size);
  bstruct_writeint(file,"index",buffer->index);
  bstruct_writereal(file,"sum",buffer->sum);
  bstruct_writerealarray(file,"data",buffer->data,buffer->n);
  return bstruct_writeendstruct(file);
} /* of 'fwritebuffer' */

Buffer freadbuffer(Bstruct file,    /**< pointer to restart file */
                   const char *name /**< name of object */
                  )                 /** \return allocated buffer or NULL */
{
  int i,size;
  Buffer buffer;
  if(bstruct_readstruct(file,name))
    return NULL;
  if(bstruct_readint(file,"size",&size))
    return NULL;
  buffer=new(struct buffer);
  if(buffer==NULL)
  {
    printallocerr("buffer");
    return NULL;
  }
  buffer->size=size;
  buffer->data=newvec(Real,buffer->size);
  if(buffer->data==NULL)
  {
    printallocerr("buffer");
    free(buffer);
    return NULL;
  }
  if(bstruct_readint(file,"index",&buffer->index))
  {
    free(buffer->data);
    free(buffer);
    return NULL;
  }
  if(bstruct_readreal(file,"sum",&buffer->sum))
  {
    free(buffer->data);
    free(buffer);
    return NULL;
  }
  if(bstruct_readarray(file,"data",&buffer->n))
  {
    free(buffer->data);
    free(buffer);
    return NULL;
  }
  if(buffer->n>buffer->size)
  {
    fprintf(stderr,"ERROR250: Size of buffer '%s'=%d is >%d.\n",
            name,buffer->n,buffer->size);
    free(buffer->data);
    free(buffer);
    return NULL;
  }
  for(i=0;i<buffer->n;i++)
    if(bstruct_readreal(file,NULL,buffer->data+i))
    {
      free(buffer->data);
      free(buffer);
      return NULL;
    }
  if(bstruct_readendarray(file))
  {
    /* read error occured */
    free(buffer->data);
    free(buffer);
    return NULL;
  }
  if(bstruct_readendstruct(file))
  {
    /* read error occured */
    free(buffer->data);
    free(buffer);
    return NULL;
  }
  return buffer;
} /* of 'freadbuffer' */

void freebuffer(Buffer buffer)
{
  if(buffer!=NULL)
  {
    free(buffer->data);
    free(buffer);
  }
} /* of 'freebuffer' */

Real getbufferavg(const Buffer buffer)
{
  return buffer->sum/buffer->n;
} /* of 'getbufferavg' */

Bool isemptybuffer(const Buffer buffer)
{
  return buffer->n==0;
} /* of 'isemptybuffer' */

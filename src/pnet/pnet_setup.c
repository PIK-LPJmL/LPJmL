/**************************************************************************************/
/**                                                                                \n**/
/**                     p  n  e  t  _  s  e  t  u  p  .  c                         \n**/
/**                                                                                \n**/
/**     MPI-parallelization of networks                                            \n**/
/**                                                                                \n**/
/**     Function initializes communication pattern necessary for the               \n**/
/**     distributed network. This is the core function for the Pnet                \n**/
/**     library. Data have to be exchanged by incoking the pnet_exchg              \n**/
/**     macro.                                                                     \n**/
/**                                                                                \n**/
/**     Implementation is described in:                                            \n**/
/**     W. von Bloh, 2008. Sequential and Parallel Implementation of               \n**/
/**     Networks. In P. beim Graben, C. Zhou, M. Thiel, and J. Kurths              \n**/
/**     (eds.), Lectures in Supercomputational Neuroscience, Dynamics in           \n**/
/**     Complex Brain Networks, Springer, 279-318.                                 \n**/
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
#include <errno.h>
#ifdef USE_MPI
#include <mpi.h>
#endif
#include "types.h"
#include "pnet.h"

static int compare(const int *a,const int *b)
{
  /* compare function to sort integers in ascending order, used by qsort() */
  return *a-*b;
} /* of 'compare' */

int pnet_setup(Pnet *pnet /**< Pointer to Pnet structure */
              )           /** \return error code        */
{
  int *lo,*hi;
  int i,j,k,*index,*in,size,slice,rem,task,insize;
#ifdef USE_MPI
  MPI_Aint extent;
#endif
  if(pnet==NULL)
    return PNET_NULL_PTR_ERR;
  slice=pnet->n/pnet->ntask;
  rem=pnet->n % pnet->ntask;
  lo=newvec(int,pnet->ntask);
  if(lo==NULL)
    return PNET_ALLOC_ERR;
  hi=newvec(int,pnet->ntask);
  if(hi==NULL)
  {
    free(lo);
    return PNET_ALLOC_ERR;
  }
  /* calculate lower and upper bounds for all tasks */
  for(i=0;i<pnet->ntask;i++)
  {
    lo[i]=i*slice;
    hi[i]=(i+1)*slice-1;
    if(i<rem)
    {
       lo[i]+=i;
       hi[i]+=i+1;
    }
    else
    {
      lo[i]+=rem;
      hi[i]+=rem;
    }
  }
  for(i=0;i<pnet->ntask;i++)
    pnet->outlen[i]=pnet->inlen[i]=0;
  /* determine total length of connection lists */
  size=0;
  pnet_foreach(pnet,i)
    size+=pnet->connect[i].n;
  in=newvec(int,size);
  if(in==NULL)
  {
    /* error allocating memory, exit with error code */
    free(lo);
    free(hi);
    pnet->outindex=NULL;
    pnet->inbuffer=pnet->outbuffer=NULL;
    return PNET_ALLOC_ERR;
  }
  if(size) /* are connection lists not all empty? */
  {
    /* concatenate connection lists to array in */
    k=0;
    for(i=pnet->lo;i<=pnet->hi;i++)
      for(j=0;j<pnet->connect[i].n;j++)
        in[k++]=pnet->connect[i].index[j];
    /* sort in ascending order */
    qsort(in,size,sizeof(int),(int (*)(const void *,const void *))compare);
    /* delete duplicates */
    insize=1;
    for(i=1;i<size;i++)
      if(in[i]!=in[i-1])
      {
        /* let in[i] in array if distinct from in[i-1] */
        in[insize]=in[i];
        insize++;
      }
    task=0;
    /* calculate inlen vector */
    pnet->inlen[task]=0;
    i=0;
    while(i<insize)
      if(in[i]<=hi[task])
      {
        /* in[i] is within lower and upper bound of task */
        pnet->inlen[task]++;
        i++;
      }
      else
      {
        /* move to next task */
        task++;
        pnet->inlen[task]=0;
      }
    /* create index vectors for array in */
    for(i=pnet->lo;i<=pnet->hi;i++)
      for(j=0;j<pnet->connect[i].n;j++)
      {
        /* performing binary search for index in in vector */
        index=(int *)bsearch(pnet->connect[i].index+j,in,insize,sizeof(int),
                             (int (*)(const void *,const void *))compare);
        /* replace entry in connection list by index */
        pnet->connect[i].index[j]=index-in;
      }
  }
  else
    insize=0;
#ifdef USE_MPI
  /* distribute information of inlen vectors to all tasks */
  MPI_Alltoall(pnet->inlen,1,MPI_INT,pnet->outlen,1,MPI_INT,pnet->comm);
#else
  /* simple copy for sequential code */
  pnet->outlen[0]=pnet->inlen[0];
#endif
  /* calculate displacement vectors */
  pnet->indisp[0]=pnet->outdisp[0]=0;
  for(k=1;k<pnet->ntask;k++)
  {
    pnet->indisp[k]=pnet->inlen[k-1]+pnet->indisp[k-1];
    pnet->outdisp[k]=pnet->outlen[k-1]+pnet->outdisp[k-1];
  }
#ifdef DEBUG
  printf("in %d:",pnet->taskid);
  k=0;
  for(i=0;i<insize;i++)
  {
    if(pnet->indisp[k]+pnet->inlen[k]==i)
    {
      printf("|");
      k++;
    }
    printf(" %d",in[i]);
  }
  printf("\n");
#endif
  /* calculate length of output buffer */
  pnet->outsize=0;
  for(i=0;i<pnet->ntask;i++)
    pnet->outsize+=pnet->outlen[i];
  /* allocate output index vector */
  pnet->outindex=newvec(int,pnet->outsize);
  if(pnet->outindex==NULL)
  {
    free(lo);
    free(hi);
    free(in);
    pnet->inbuffer=pnet->outbuffer=NULL;
    return PNET_ALLOC_ERR;
  }
#ifdef USE_MPI
  /* distribute index vectors */
  MPI_Alltoallv(in,pnet->inlen,pnet->indisp,MPI_INT,pnet->outindex,pnet->outlen,
                pnet->outdisp,MPI_INT,pnet->comm);
#else
  for(i=0;i<pnet->inlen[0];i++)
    pnet->outindex[i]=in[i];
#endif
#ifdef DEBUG
  printf("out %d:",pnet->taskid);
  k=0;
  for(i=0;i<pnet->outsize;i++)
  {
    if(pnet->outlen[k]+pnet->outdisp[k]==i)
    {
      printf("|");
      k++;
    }
    printf(" %d",pnet->outindex[i]);
  }
  printf("\n");
#endif
  /* Allocation of input and output buffer */
#ifdef USE_MPI
  MPI_Type_extent(pnet->type,&extent); /* calculate size of datatype */
  pnet->outbuffer=malloc(extent*pnet->outsize);
  pnet->inbuffer=malloc(extent*insize);
#else
  pnet->outbuffer=malloc(pnet->size*pnet->outsize);
  pnet->inbuffer=malloc(pnet->size*insize);
#endif
  free(in);
  free(lo);
  free(hi);
  return (pnet->outbuffer==NULL || pnet->inbuffer==NULL) ? PNET_ALLOC_ERR : PNET_OK;
} /* of 'pnet_setup' */

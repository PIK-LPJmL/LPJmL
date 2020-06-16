/**************************************************************************************/
/**                                                                                \n**/
/**                 p  n  e  t  _  r  e  v  e  r  s  e  .  c                       \n**/
/**                                                                                \n**/
/**     MPI-parallelization of networks                                            \n**/
/**                                                                                \n**/
/**     Function reverses the direction of all connections                         \n**/
/**                                                                                \n**/
/**     Implementation is described in:                                            \n**/
/**     W. von Bloh, 2008. Sequential and Parallel Implementation of               \n**/
/**     Networks. In P. beim Graben, C. Zhou, M. Thiel, and J. Kurths              \n**/
/**     (eds.), Lectures in Supercomputational Neuroscience, Dynamics in           \n**/
/**     Complex Brain Networks, Springer, New York, 279-318.                       \n**/
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

typedef struct
{
  int index,from;
} data_t;

static int compare(const data_t *a,const data_t *b)
{
  /* compare function to sort data_t items by their integer index */
  return a->index-b->index;
} /* of 'compare' */

int pnet_reverse(Pnet *pnet /**< Pointer to Pnet structure */
                )           /** \return error code        */
{
  int *lo,*hi;
  int i,j,size,slice,rem,task,outsize,rc;
  data_t *in,*out;
#ifdef USE_MPI
  MPI_Datatype type;
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
  in=newvec(data_t,size);
  if(in==NULL)
  {
    free(lo);
    free(hi);
    return PNET_ALLOC_ERR;
  }
  /* concatenate connection lists to array in */
  size=0;
  for(i=pnet->lo;i<=pnet->hi;i++)
    for(j=0;j<pnet->connect[i].n;j++)
    {
      in[size].from=i;
      in[size++].index=pnet->connect[i].index[j];
    }
  /* sort by their index in ascending order */
  qsort(in,size,sizeof(data_t),(int (*)(const void *,const void *))compare);
  task=0;
  /* calculate inlen vector */
  pnet->inlen[task]=0;
  i=0;
  while(i<size)
    /* check, whether in[i].index is within task */
    if(in[i].index<=hi[task])
    {
      /* yes, increase inlen */
      pnet->inlen[task]++;
      i++;
    }
    else
    {
      /* no, move to next task */
      task++;
      pnet->inlen[task]=0;
    }
  /* empty connection lists  */
  for(i=pnet->lo;i<=pnet->hi;i++)
    emptyintlist(pnet->connect+i);
#ifdef USE_MPI
  MPI_Alltoall(pnet->inlen,1,MPI_INT,pnet->outlen,1,MPI_INT,pnet->comm);
#else
  pnet->outlen[0]=pnet->inlen[0];
#endif
  /* calculate displacement vectors */
  pnet->indisp[0]=pnet->outdisp[0]=0;
  for(i=1;i<pnet->ntask;i++)
  {
    pnet->indisp[i]=pnet->inlen[i-1]+pnet->indisp[i-1];
    pnet->outdisp[i]=pnet->outlen[i-1]+pnet->outdisp[i-1];
  }
#ifdef DEBUG
  printf("in %d:",pnet->taskid);
  j=0;
  for(i=0;i<size;i++)
  {
    if(pnet->indisp[j]+pnet->inlen[j]==i)
    {
      printf("|");
      j++;
    }
    printf(" %d,%d",in[i].from,in[i].index);
  }
  printf("\n");
#endif
  /* calculate length of out vector */
  outsize=0;
  for(i=0;i<pnet->ntask;i++)
    outsize+=pnet->outlen[i];
  out=newvec(data_t,outsize);
  if(out==NULL)
  {
    free(lo);
    free(hi);
    free(in);
    return PNET_ALLOC_ERR;
  }
#ifdef USE_MPI
  /* create new MPI datatype for data_t */
  MPI_Type_contiguous(2,MPI_INT,&type);
  MPI_Type_commit(&type);
  MPI_Alltoallv(in,pnet->inlen,pnet->indisp,type,out,pnet->outlen,
                pnet->outdisp,type,pnet->comm);
  MPI_Type_free(&type);
#else
  for(i=0;i<outsize;i++)
    out[i]=in[i];
#endif
#ifdef DEBUG
  printf("out %d:",pnet->taskid);
  j=0;
  for(i=0;i<outsize;i++)
  {
    if(pnet->outlen[j]+pnet->outdisp[j]==i)
    {
      printf("|");
      j++;
    }
    printf(" %d,%d",out[i].from,out[i].index);
  }
  printf("\n");
#endif
  /* build new connection lists */
  for(i=0;i<outsize;i++)
  {
    rc=pnet_addconnect(pnet,out[i].index,out[i].from);
    if(rc!=PNET_OK)
    {
      free(in);
      free(lo);
      free(hi);
      free(out);
      return rc;
    }
  }
  /* free memory */
  free(in);
  free(lo);
  free(hi);
  free(out);
  return PNET_OK;
} /* of 'pnet_reverse' */

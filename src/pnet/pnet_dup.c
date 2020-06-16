/**************************************************************************************/
/**                                                                                \n**/
/**                     p  n  e  t  _  d  u  p  .  c                               \n**/
/**                                                                                \n**/
/**     MPI-parallelization of networks                                            \n**/
/**                                                                                \n**/
/**     Function duplicates pnet datatype                                          \n**/
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

#include <stdlib.h>
#include <stdio.h>
#ifdef USE_MPI
#include <mpi.h>
#endif
#include "types.h"
#include "pnet.h"

Pnet *pnet_dup(const Pnet *pnet
              )      /** \returs duplicated pnet structure or NULL */
{
  int i;
  Pnet *ret;
  if(pnet==NULL)
    return NULL;
  ret=new(Pnet);
  if(ret==NULL)
    return NULL;
  ret->n=pnet->n;
#ifdef USE_MPI
  ret->type=pnet->type;
  ret->comm=pnet->comm;
#else
  ret->size=pnet->size;
#endif
  ret->taskid=pnet->taskid;
  ret->ntask=pnet->ntask;
  ret->lo=pnet->lo;
  ret->hi=pnet->hi;
  /* allocate memory for connection list array */
  ret->connect=newvec2(Intlist,pnet->lo,pnet->hi);
  if(ret->connect+pnet->lo==NULL) /* was memory allocation successful? */
  {
    free(ret);
    return NULL; /* no, return NULL */
  }
  /* copy connection lists */
  pnet_foreach(pnet,i)
    if(copyintlist(ret->connect+i,pnet->connect+i))
    { /* error occurred duplicating list */
      free(ret);
      return NULL;
    }
  /* allocate length and displacement vectors for input and output buffers */
  ret->outdisp=newvec(int,pnet->ntask);
  if(ret->outdisp==NULL)
  {
    freevec(ret->connect,pnet->lo,pnet->hi);
    free(ret);
    return NULL;
  }
  ret->indisp=newvec(int,pnet->ntask);
  if(ret->indisp==NULL)
  {
    free(ret->outdisp);
    freevec(ret->connect,pnet->lo,pnet->hi);
    free(ret);
    return NULL;
  }
  ret->outlen=newvec(int,pnet->ntask);
  if(ret->outlen==NULL)
  {
    free(ret->outdisp);
    free(ret->indisp);
    freevec(ret->connect,pnet->lo,pnet->hi);
    free(ret);
    return NULL;
  }
  ret->inlen=newvec(int,pnet->ntask);
  if(ret->inlen==NULL)
  {
    free(ret->outdisp);
    free(ret->indisp);
    free(ret->outlen);
    freevec(ret->connect,pnet->lo,pnet->hi);
    free(ret);
    return NULL;
  }
  return ret;
} /* of 'pnet_dup' */

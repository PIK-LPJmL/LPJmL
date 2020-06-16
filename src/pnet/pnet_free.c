/**************************************************************************************/
/**                                                                                \n**/
/**                     p  n  e  t  _  f  r  e  e  .  c                            \n**/
/**                                                                                \n**/
/**     MPI-parallelization of networks                                            \n**/
/**                                                                                \n**/
/**     Function frees allocated memory of Pnet structure                          \n**/
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

void pnet_free(Pnet *pnet)
{
  int i;
  if(pnet!=NULL)
  {
    free(pnet->outdisp);
    free(pnet->indisp);
    free(pnet->outlen);
    free(pnet->inlen);
    free(pnet->inbuffer);
    free(pnet->outbuffer);
    free(pnet->outindex);
    /* empty connection lists */
    for(i=pnet->lo;i<=pnet->hi;i++)
      emptyintlist(pnet->connect+i);
    freevec(pnet->connect,pnet->lo,pnet->hi);
    free(pnet);
  }
} /* of 'pnet_free' */

/**************************************************************************************/
/**                                                                                \n**/
/**               p  n  e  t  _  a  d  d  c  o  n  n  e  c  t  .  c                \n**/
/**                                                                                \n**/
/**     MPI-parallelization of networks                                            \n**/
/**                                                                                \n**/
/**     Function adds connection to network                                        \n**/
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

#include <stdlib.h>
#include <stdio.h>
#ifdef USE_MPI
#include <mpi.h>
#endif
#include "types.h"
#include "pnet.h"

int pnet_addconnect(Pnet *pnet, /**< Pointer to pnet structure */
                    int to,     /**< target index of connection */
                    int from    /**< source index of connection */
                   )            /** \return error code */
{
  if(pnet==NULL)
    return PNET_NULL_PTR_ERR;
  /* check whether from and to index are valid */
  if(to<pnet->lo || to>pnet->hi)
    return PNET_TO_INDEX_ERR;
  if(from<0 || from>=pnet->n)
    return PNET_FROM_INDEX_ERR;
  return (addintlistitem(pnet->connect+to,from)==0) ? PNET_ALLOC_ERR : PNET_OK;
} /* of 'pnet_addconnect' */

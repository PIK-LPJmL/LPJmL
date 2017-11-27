/**************************************************************************************/
/**                                                                                \n**/
/**                p  n  e  t  _  s  t  r  e  r  r  o  r  .  c                     \n**/
/**                                                                                \n**/
/**     MPI-parallelization of networks                                            \n**/
/**                                                                                \n**/
/**     Function gets error message string                                         \n**/
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
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#ifdef USE_MPI
#include <mpi.h>
#endif
#include "types.h"
#include "pnet.h"

char *pnet_strerror(int err /**< error code */
                   )        /** \return error string */
{
  static char *errstr[]={"No error",
                         "Could not allocate memory",
                         "Invalid to index",
                         "Invalid from index",
                         "Invalid pointer",
                         "unknown error"};
  return errstr[(err<0 || err>PNET_NULL_PTR_ERR) ? 5 : err];
} /* of 'pnet_strerror' */

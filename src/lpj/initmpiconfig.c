/**************************************************************************************/
/**                                                                                \n**/
/**                i  n  i  t  m  p  i  c  o  n  f  i  g  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initialize Config struct for parallel version                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef USE_MPI

void initmpiconfig(Config *config, /**< LPJ configuration */
                   MPI_Comm comm   /**< MPI communicator */
                  )
{
  config->comm=comm;
  MPI_Comm_rank(comm,&config->rank); /* get my rank: 0..ntask-1 */
  MPI_Comm_size(comm,&config->ntask); /* get number of tasks */
} /* of 'initmpiconfig' */

#endif

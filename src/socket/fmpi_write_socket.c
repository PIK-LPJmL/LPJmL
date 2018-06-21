/**************************************************************************************/
/**                                                                                \n**/
/**         f  s  o  c  k  e  t  _  w  r  i  t  e  a  l  l  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
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

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "channel.h"

#ifdef USE_MPI

void mpi_write_socket_(Socket *socket,    /* socket */
                      void *data,        /* data to be written to channel */
                      MPI_Datatype *type, /* MPI datatype of data */
                      int *size,
                      int counts[],
                      int offsets[],
                      int *rank,          /* MPI rank */
                      MPI_Comm *comm,     /* MPI communicator */
                      int *err            /* error code */
                     )
{
  *err=mpi_write_socket(socket,data,*type,*size,counts,offsets,*rank,*comm);
} /* of 'socket_writeall_' */

#endif

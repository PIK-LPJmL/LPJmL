/**************************************************************************************/
/**                                                                                \n**/
/**         m  p  i  _  w  r  i  t  e  _  s  o  c  k  e  t  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes data from all tasks using socket connection                \n**/
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
#include "errmsg.h"

#ifdef USE_MPI

Bool mpi_write_socket(Socket *socket,    /* socket */
                      void *data,        /* data to be written to channel */
                      MPI_Datatype type, /* MPI datatype of data */
                      int size,
                      int counts[],
                      int offsets[],
                      int rank,          /* MPI rank */
                      MPI_Comm comm      /* MPI communicator */
                     )
{
  Bool rc;
  MPI_Aint extent;
  void *vec;
  MPI_Type_extent(type,&extent);

  if(rank==0)
  {
    vec=malloc(size*extent); /* allocate receive buffer */
    check(vec);
  }
  MPI_Gatherv(data,counts[rank],type,vec,counts,offsets,type,0,comm);
  if(rank==0)
  {
    rc=write_socket(socket,vec,size*extent);
    free(vec);
  }
  else
    rc=FALSE;
  MPI_Barrier(comm);
  return rc;
} /* of 'mpi_write_socket' */

#endif

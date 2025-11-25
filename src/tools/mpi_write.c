/**************************************************************************************/
/**                                                                                \n**/
/**                  m  p  i  _  w  r  i  t  e  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes output from all tasks using MPI_Gatherv                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef USE_MPI

Bool mpi_write(FILE *file,        /**< File pointer to binary file */
              void *data,        /**< data to be written to disk */
              MPI_Datatype type, /**< MPI datatype of data */
              int size,
              int counts[],
              int offsets[],
              int rank,          /**< MPI rank */
              MPI_Comm comm      /**< MPI communicator */
             )                   /** \return TRUE on error */
{
  Bool rc=FALSE;
  int n;
  MPI_Aint lb;
  MPI_Aint extent;
  MPI_Type_get_extent(type,&lb,&extent);
  void *vec=NULL;
  if(rank==0)
  {
    vec=malloc(size*extent); /* allocate receive buffer */
    check(vec);
  }
  MPI_Gatherv(data,counts[rank],type,vec,counts,offsets,type,0,comm);
  if(rank==0)
  {
    n=fwrite(vec,extent,size,file); /* write data to file */
    if(n!=size)
    {
      fprintf(stderr,"ERROR204: Cannot write output: %s.\n",strerror(errno));
      rc=TRUE;
    }
    free(vec);
  }
  MPI_Bcast(&rc,1,MPI_INT,0,comm);
#ifdef USE_TIMING
  double tstart;
  timing_start(tstart);
#endif
  MPI_Barrier(comm);
#ifdef USE_TIMING
  timing_stop(MPI_BARRIER_FCN,tstart);
#endif
  return rc;
} /* of 'mpi_write' */
#endif

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

int mpi_write(FILE *file,        /**< File pointer to binary file */
              void *data,        /**< data to be written to disk */
              MPI_Datatype type, /**< MPI datatype of data */
              int size,
              int counts[],
              int offsets[],
              int rank,          /**< MPI rank */
              MPI_Comm comm      /**< MPI communicator */
             )                   /** \return number of items written to disk */
{
  int rc=0;
  MPI_Aint extent;
  MPI_Type_extent(type,&extent);
  void *vec;
  if(rank==0)
  {
    vec=malloc(size*extent); /* allocate receive buffer */
    check(vec);
  }
  MPI_Gatherv(data,counts[rank],type,vec,counts,offsets,type,0,comm);
  if(rank==0)
  {
    rc=fwrite(vec,extent,size,file); /* write data to file */
    if(rc!=size)
      fprintf(stderr,"ERROR204: Error writing output: %s.\n",strerror(errno));
    free(vec);
  }
  MPI_Barrier(comm);
  return rc;
} /* of 'mpi_write' */
#endif

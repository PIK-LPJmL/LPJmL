/**************************************************************************************/
/**                                                                                \n**/
/**                  m p i _ w r i t e _ p f t _ n e t c d f . c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes PFT-specific output from all tasks using                   \n**/
/**     MPI_Gatherv into NetCDF file                                               \n**/
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

Bool mpi_write_pft_netcdf(const Netcdf *cdf, /* pointer to NetCDF */
                          void *data,        /* data to be written to disk */
                          MPI_Datatype type,
                          int size,
                          int year,
                          int pft,
                          int counts[],
                          int offsets[],
                          int rank,          /* MPI rank */
                          MPI_Comm comm      /* MPI communicator */
                         )                   /* returns TRUE on error */
{
  Bool rc;
  MPI_Aint extent;
  void *vec=NULL;
  MPI_Type_extent(type,&extent);
  if(rank==0)
  {
    vec=malloc(size*extent); /* allocate receive buffer */
    if(vec==NULL)
    {
      printallocerr("vec");
      rc=TRUE;
    }
    else
      rc=FALSE;
  }
  MPI_Bcast(&rc,1,MPI_INT,0,comm);
  if(rc)
    return TRUE;
  MPI_Gatherv(data,counts[rank],type,vec,counts,offsets,type,0,comm);
  if(rank==0)
  {
    if(type==MPI_FLOAT)
      rc=write_pft_float_netcdf(cdf,vec,year,pft,size); /* write data to file */
    else if(type==MPI_SHORT)
      rc=write_pft_short_netcdf(cdf,vec,year,pft,size); /* write data to file */
    free(vec);
  }
  MPI_Barrier(comm);
  return rc;
} /* of 'mpi_write_pft_netcdf' */
#endif

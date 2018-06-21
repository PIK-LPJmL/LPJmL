/**************************************************************************************/
/**                                                                                \n**/
/**            m p i _ o p e n c l i m a t e _ n e t c d f . c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function opens climate data file in NetCDF format on one task              \n**/
/**     and broadcasts information to all other tasks                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#if defined(USE_NETCDF) || defined(USE_NETCDF4)
#include <netcdf.h>
#endif

Bool mpi_openclimate_netcdf(Climatefile *file,    /**< climate data file */
                            const char *filename, /**< filename */
                            const char *var,      /**< variable name or NULL */
                            const char *units,    /**< units or NULL */
                            const Config *config  /**< LPJ configuration */
                           )                      /** \return TRUE on error */
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int rc;
  if(isroot(*config))
    rc=openclimate_netcdf(file,filename,var,units,config);
#ifdef USE_MPI
  /* broadcast return code */
  MPI_Bcast(&rc,1,MPI_INT,0,config->comm);
#endif
  if(rc)
    return TRUE;
#ifdef USE_MPI
  /* broadcast necessary data */
  MPI_Bcast(&file->datatype,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->var_len,sizeof(size_t),MPI_BYTE,0,config->comm);
  MPI_Bcast(&file->time_step,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->isleap,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->n,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->firstyear,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->nyear,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->nlon,sizeof(size_t),MPI_BYTE,0,config->comm);
  MPI_Bcast(&file->nlat,sizeof(size_t),MPI_BYTE,0,config->comm);
  MPI_Bcast(&file->is360,1,MPI_INT,0,config->comm);
  MPI_Bcast(&file->lon_min,1,MPI_FLOAT,0,config->comm);
  MPI_Bcast(&file->lat_min,1,MPI_FLOAT,0,config->comm);
  MPI_Bcast(&file->lon_res,1,MPI_FLOAT,0,config->comm);
  MPI_Bcast(&file->lat_res,1,MPI_FLOAT,0,config->comm);
  MPI_Bcast(&file->slope,1,MPI_DOUBLE,0,config->comm);
  MPI_Bcast(&file->intercept,1,MPI_DOUBLE,0,config->comm);
  MPI_Bcast(&file->offset,sizeof(size_t),MPI_BYTE,0,config->comm);
  MPI_Bcast(&file->missing_value,sizeof(file->missing_value),MPI_BYTE,0,
            config->comm);
#endif
  return FALSE;
#else
  if(isroot(*config))
    fputs("ERROR401: NetCDF input is not supported by this version of LPJmL.\n",stderr);
  return TRUE;
#endif

} /* of 'mpi_openclimate_netcdf' */

/**************************************************************************************/
/**                                                                                \n**/
/**               g e t i n t a r r a y _ n e t c d f . c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads int array from NetCDF file                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef USE_NETCDF
#include <netcdf.h>
#endif

int *getintarray_netcdf(int ncid,        /**< id of NetCDF file */
                        int *size,       /**< size of int array */
                        const char *name /**< name of int array in NetCDF file */
                       )                 /** int array read or NULL on error */
{
#ifdef USE_NETCDF
  int *array;
  size_t len;
  int varid,rc,ndims,dimids;
  if(nc_inq_varid(ncid,name,&varid))
    return NULL;
  nc_inq_varndims(ncid,varid,&ndims);
  if(ndims!=1)
  {
    fprintf(stderr,"ERROR408: Invalid number %d of dimensions of %s, must be 1.\n",
            ndims,name);
    return NULL;
  }
  nc_inq_vardimid(ncid,varid,&dimids);
  nc_inq_dimlen(ncid,dimids,&len);
  *size=len;
  array=newvec(int,*size);
  if(array==NULL)
  {
    printallocerr("array");
    return NULL;
  }
  rc=nc_get_var_int(ncid,varid,array);
  if(rc)
  {
    fprintf(stderr,"ERROR411: Cannot read %s: %s\n",name,nc_strerror(rc));
    free(array);
    return NULL;
  }
  return array;
#else
  return NULL;
#endif
} /* of 'getintarray_netcdf' */

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

int *getintarray_netcdf(const char *filename, /**< name of NetCDF file */
                        int *size,            /**< size of int array */
                        const char *name,     /**< name of int array in NetCDF file */
                        Verbosity verb        /**< verbosity level (NO_ERR,ERR,VERB) */
                       )                      /** int array read or NULL on error */
{
#ifdef USE_NETCDF
  int *array;
  size_t len;
  int ncid,varid,rc,ndims,dimids;
  rc=nc_open(filename,NC_NOWRITE,&ncid);
  if(rc)
  {
    if(verb)
      fprintf(stderr,"ERROR401: Cannot open '%s': %s\n",filename,nc_strerror(rc));
    return NULL;
  }
  if(nc_inq_varid(ncid,name,&varid))
  {
    if(verb)
      fprintf(stderr,"ERROR406: Cannot find variable %s in '%s'.\n",name,filename);
    nc_close(ncid);
    return NULL;
  }
  nc_inq_varndims(ncid,varid,&ndims);
  if(ndims!=1)
  {
    if(verb)
      fprintf(stderr,"ERROR408: Invalid number %d of dimensions of %s in '%s', must be 1.\n",
              ndims,name,filename);
    nc_close(ncid);
    return NULL;
  }
  nc_inq_vardimid(ncid,varid,&dimids);
  nc_inq_dimlen(ncid,dimids,&len);
  *size=len;
  array=newvec(int,*size);
  if(array==NULL)
  {
    printallocerr("array");
    nc_close(ncid);
    return NULL;
  }
  rc=nc_get_var_int(ncid,varid,array);
  if(rc)
  {
    if(verb)
      fprintf(stderr,"ERROR411: Cannot read %s in '%s': %s\n",name,filename,nc_strerror(rc));
    free(array);
    nc_close(ncid);
    return NULL;
  }
  nc_close(ncid);
  return array;
#else
  return NULL;
#endif
} /* of 'getintarray_netcdf' */

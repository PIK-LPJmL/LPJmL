/**************************************************************************************/
/**                                                                                \n**/
/**               g e t l i m i t a r r a y _ n e t c d f . c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads limit array from NetCDF file                                \n**/
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

Limit *getlimitarray_netcdf(int ncid,        /**< id of NetCDF file */
                            int *size,       /**< size of limit array */
                            const char *name /**< name of limit array in NetCDF file */
                           )                 /** limit array read or NULL on error */
{
#ifdef USE_NETCDF
  Limit *limits;
  double *data;
  size_t len;
  int varid,rc,ndims,dimids[2],i,count;
  if(nc_inq_varid(ncid,name,&varid))
    return NULL;
  nc_inq_varndims(ncid,varid,&ndims);
  if(ndims!=2)
  {
    fprintf(stderr,"ERROR408: Invalid number %d of dimensions of %s, must be 2.\n",
            ndims,name);
    return NULL;
  }
  nc_inq_vardimid(ncid,varid,dimids);
  nc_inq_dimlen(ncid,dimids[0],&len);
  *size=len;
  nc_inq_dimlen(ncid,dimids[1],&len);
  if(len!=2)
  {
    fprintf(stderr,"ERROR408: Length of second dimension=%zu of %s, is not 2.\n",
            len,name);
    return NULL;
  }
  data=newvec(double,*size*2);
  if(data==NULL)
  {
    printallocerr("data");
    return NULL;
  }
  rc=nc_get_var_double(ncid,varid,data);
  if(rc)
  {
    fprintf(stderr,"ERROR411: Cannot read %s: %s\n",name,nc_strerror(rc));
    free(data);
    return NULL;
  }
  limits=newvec(Limit,*size);
  if(limits==NULL)
  {
    printallocerr("limits");
    free(data);
    return NULL;
  }
  count=0;
  for(i=0;i<*size;i++)
  {
    limits[i].low=data[count++];
    limits[i].high=data[count++];
  }
  free(data);
  return limits;
#else
  return NULL;
#endif
} /* of 'getlimitarray_netcdf' */

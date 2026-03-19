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

Limit *getlimitarray_netcdf(const char *filename, /**< name of NetCDF file */
                            int *size,            /**< size of limit array */
                            const char *name,     /**< name of limit array in NetCDF file */
                            Verbosity verb        /**< verbosity level (NO_ERR,ERR,VERB) */
                           )                      /** limit array read or NULL on error */
{
#ifdef USE_NETCDF
  Limit *limits;
  double *data;
  size_t len;
  int ncid,varid,rc,ndims,dimids[2],i,count;
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
  if(ndims!=2)
  {
    if(verb)
      fprintf(stderr,"ERROR408: Invalid number %d of dimensions of %s in '%s', must be 2.\n",
              ndims,name,filename);
    nc_close(ncid);
    return NULL;
  }
  nc_inq_vardimid(ncid,varid,dimids);
  nc_inq_dimlen(ncid,dimids[0],&len);
  *size=len;
  nc_inq_dimlen(ncid,dimids[1],&len);
  if(len!=2)
  {
    if(verb)
      fprintf(stderr,"ERROR408: Length of second dimension=%zu of %s in '%s' is not 2.\n",
              len,name,filename);
    nc_close(ncid);
    return NULL;
  }
  data=newvec(double,*size*2);
  if(data==NULL)
  {
    printallocerr("data");
    nc_close(ncid);
    return NULL;
  }
  rc=nc_get_var_double(ncid,varid,data);
  if(rc)
  {
    if(verb)
      fprintf(stderr,"ERROR411: Cannot read %s in '%s': %s\n",name,filename,nc_strerror(rc));
    free(data);
    nc_close(ncid);
    return NULL;
  }
  nc_close(ncid);
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

/**************************************************************************************/
/**                                                                                \n**/
/**       g  e  t  l  a  t  l  o  n  _  n  e  t  c  d  f  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function gets longitude/latitude data in NetCDF file                       \n**/
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

#define error(var,rc) if(rc) {if(isroot(*config))fprintf(stderr,"ERROR403: Cannot read '%s' in '%s': %s.\n",var,filename,nc_strerror(rc)); return TRUE;}
#endif

Bool getlatlon_netcdf(Climatefile *file,    /**< Climate data */
                      const char *filename, /**< filename */
                      const Config *config  /**< LPJ configuration */
                     )                      /** \return TRUE on error */
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  int rc,var_id,*dimids,ndims,index;
  char name[NC_MAX_NAME+1];
  float *dim;
  nc_inq_varndims(file->ncid,file->varid,&ndims);
  if(ndims<2)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR408: Invalid number of dimensions %d in '%s'.\n",
              ndims,filename);
    return TRUE;
  }
  index=ndims-1;
  dimids=newvec(int,ndims);
  if(dimids==NULL)
  {
    printallocerr("dimids");
    return TRUE;
  }
  nc_inq_vardimid(file->ncid,file->varid,dimids);
  nc_inq_dimname(file->ncid,dimids[index],name);
  rc=nc_inq_varid(file->ncid,name,&var_id);
  if(rc)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR410: Cannot read %s in '%s': %s.\n",
              name,filename,nc_strerror(rc));
    free(dimids);
    return TRUE;
  }
  nc_inq_dimlen(file->ncid,dimids[index],&file->nlon);
  dim=newvec(float,file->nlon);
  if(dim==NULL)
  {
    free(dimids);
    printallocerr("dim");
    return TRUE;
  }
  rc=nc_get_var_float(file->ncid,var_id,dim);
  file->is360=(dim[file->nlon-1]>180);
  if(isroot(*config) && file->is360)
   fprintf(stderr,"REMARK401: Longitudinal values>180 in '%s', will be transformed.\n",filename);
  if(rc)
  {
    free(dimids);
    free(dim);
    if(isroot(*config))
      fprintf(stderr,"ERROR410: Cannot read longitude in '%s': %s.\n",
              filename,nc_strerror(rc));
    return TRUE;
  }
  file->lon_min=dim[0];
  if(file->nlon==1)
    file->lon_res=config->resolution.lon;
  else
    file->lon_res=(dim[file->nlon-1]-dim[0])/(file->nlon-1);
  if(fabs(file->lon_res-config->resolution.lon)/config->resolution.lon>1e-3)
  {
    if(isroot(*config))
      fprintf(stderr,"WARNING405: Incompatible resolution %g for longitude in '%s',not %g.\n",
              file->lon_res,filename,config->resolution.lon);
    //free(dim);
    //free(dimids);
    //return TRUE;
  }
  free(dim);
  nc_inq_dimname(file->ncid,dimids[index-1],name);
  rc=nc_inq_varid(file->ncid,name,&var_id);
  if(rc)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR410: Cannot read %s in '%s': %s.\n",
              name,filename,nc_strerror(rc));
    free(dimids);
    return TRUE;
  }
  nc_inq_dimlen(file->ncid,dimids[index-1],&file->nlat);
  free(dimids);
  dim=newvec(float,file->nlat);
  if(dim==NULL)
  {
    printallocerr("dim");
    return TRUE;
  }
  rc=nc_get_var_float(file->ncid,var_id,dim);
  if(rc)
  {
    free(dim);
    if(isroot(*config))
      fprintf(stderr,"ERROR411: Cannot read latitude in '%s': %s.\n",
              filename,nc_strerror(rc));
    return TRUE;
  }
  if(file->nlat==1)
  {
    file->lat_min=dim[0];
    file->lat_res=config->resolution.lat;
    file->offset=0;
  }
  else if(dim[1]>dim[0])
  {
    file->lat_min=dim[0];
    file->lat_res=(dim[file->nlat-1]-dim[0])/(file->nlat-1);
    file->offset=0;
  }
  else
  {
    file->lat_min=dim[file->nlat-1];
    file->lat_res=(dim[0]-dim[file->nlat-1])/(file->nlat-1);
    file->offset=file->nlat-1;
  }
  if(fabs(file->lat_res-config->resolution.lat)/config->resolution.lat>1e-3)
  {
    if(isroot(*config))
      fprintf(stderr,"WARNING406: Incompatible resolution %g for latitude in '%s', not %g.\n",
              file->lat_res,filename,config->resolution.lat);
    //free(dim);
    //return TRUE;
  }
  free(dim);
  return FALSE;
#else
  return TRUE;
#endif
} /* of 'getlatlon_netcdf' */

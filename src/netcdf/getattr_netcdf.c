/**************************************************************************************/
/**                                                                                \n**/
/**            g  e  t  a  t  t  r  _  n  e  t  c  d  f  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads attribute of variable in NetCDF file                        \n**/
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

char *getattr_netcdf(const Climatefile *file, /**< climate data file */
                     int varid,               /**< variable id */
                     const char *attr         /**< name of attribute */
                    )                         /** \return attribute string or NULL */
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  char *s;
  size_t len;
  if(nc_inq_attlen(file->ncid, varid, attr, &len))
    s=NULL;
  else
  {
    s=malloc(len+1);
    if(s==NULL)
    {
      printallocerr("s");
      return NULL;
    }
    nc_get_att_text(file->ncid, varid, attr,s);
    s[len]='\0';
  }
  return s;
#else
  fputs("ERROR401: NetCDF input is not supported by this version of LPJmL.\n",stderr);
  return NULL;
#endif
} /* 'getattr_netcdf' */

/**************************************************************************************/
/**                                                                                \n**/
/**            g  e  t  v  a  r  n  a  m  e  _   n  e  t  c  d  f  .  c            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads name of variable in NetCDF file                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#if defined(USE_NETCDF)
#include <netcdf.h>
#endif

char *getvarname_netcdf(const Climatefile *file /**< climate data file */
                       )                        /** \return name or NULL */
{
#if defined(USE_NETCDF)
  char name[NC_MAX_NAME];
  if(nc_inq_varname(file->ncid,file->varid,name))
    return NULL;
  return strdup(name);
#else
  fputs("ERROR401: NetCDF input is not supported by this version of LPJmL.\n",stderr);
  return NULL;
#endif
} /* 'getvarname_netcdf' */

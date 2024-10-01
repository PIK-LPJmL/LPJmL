/**************************************************************************************/
/**                                                                                \n**/
/**                 c  l  o  s  e  _  n  e  t  c  d  f  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function closes NetCDF file                                                \n**/
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

Bool close_netcdf(Netcdf *cdf)
{
#ifdef USE_NETCDF
  if(cdf->state==APPEND || cdf->state==CREATE)
    return FALSE;
  return nc_close(cdf->ncid)!=NC_NOERR;
#else
  return TRUE;
#endif
} /* of 'close_netcdf' */

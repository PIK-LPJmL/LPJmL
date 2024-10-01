/**************************************************************************************/
/**                                                                                \n**/
/**                 f  l  u  s  h  _  n  e  t  c  d  f  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function flushes NetCDF file                                               \n**/
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

void flush_netcdf(Netcdf *cdf)
{
#ifdef USE_NETCDF
  nc_sync(cdf->ncid);
#endif
} /* of 'flush_netcdf' */

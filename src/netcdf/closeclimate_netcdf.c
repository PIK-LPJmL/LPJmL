/**************************************************************************************/
/**                                                                                \n**/
/**    c  l  o  s  e  c  l  i  m  a  t  e  _  n  e  t  c  d  f  .  c               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function closes climate data file in NetCDF format                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void closeclimate_netcdf(Climatefile *file,Bool isroot)
{
#if defined(USE_NETCDF) || defined(USE_NETCDF4)
  if(file->oneyear)
    free(file->filename);
  else if(isroot)
    free_netcdf(file->ncid);
#endif
} /* of 'closeclimate_netcdf' */

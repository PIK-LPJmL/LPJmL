/**************************************************************************************/
/**                                                                                \n**/
/**          o  p  e  n  f  i  l  e  _  n  e  t  c  d  f  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function opens data file in NetCDF format                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool openfile_netcdf(Climatefile *file,    /**< data file */
                     const char *filename, /**< filename */
                     const char *var,      /**< variable name or NULL */
                     const char *units,    /**< units or NULL */
                     const Config *config  /**< LPJ configuration */
                    )                      /** \return TRUE on error */
{
  if(openclimate_netcdf(file,filename,var,units,config))
    return TRUE;
  file->oneyear=FALSE;
  if(file->time_step!=YEAR)
  {
    fprintf(stderr,"ERROR435: No yearly data in file '%s'.\n",filename);
    closeclimate_netcdf(file,TRUE);
    return TRUE;
  }
  return FALSE;
} /* of 'openfile_netcdf' */

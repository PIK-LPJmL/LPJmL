/**************************************************************************************/
/**                                                                                \n**/
/**          o  p  e  n  d  a  t  a  _  n  e  t  c  d  f  .  c                     \n**/
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

Bool opendata_netcdf(Climatefile *file,    /**< data file */
                     const Filename *filename, /**< filename */
                     const char *units,    /**< units or NULL */
                     const Config *config  /**< LPJ configuration */
                    )                      /** \return TRUE on error */
{
  if(mpi_openclimate_netcdf(file,filename,units,config))
    return TRUE;
  file->oneyear=FALSE;
  if(file->time_step!=YEAR)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR435: No yearly data in file '%s'.\n",filename->name);
    closeclimate_netcdf(file,isroot(*config));
    return TRUE;
  }
  else if(file->delta_year!=1)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR435: Time step of %d yrs in file '%s' must be 1.\n",
              file->delta_year,filename->name);
    closeclimate_netcdf(file,isroot(*config));
    return TRUE;
  }
  return FALSE;
} /* of 'opendata_netcdf' */

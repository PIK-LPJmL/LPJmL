/**************************************************************************************/
/**                                                                                \n**/
/**                      o  p  e  n  d  a  t  a  _  s  e  q  .  c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function opens data file without MPI                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool opendata_seq(Climatefile *file,        /**< pointer to file */
                  const Filename *filename, /**< filename */
                  const char *name,         /**< name of data */
                  const char *unit,         /**< unit or NULL */
                  Type datatype,            /**< datatype for version 2 files */
                  Real scalar,              /**< scalar for version 1 files */
                  int nbands,               /**< number of bands */
                  Bool ischeck,             /**< check number of bands (TRUE/FALSE) */
                  const Config *config      /**< LPJ configuration */
                 )                          /** \return TRUE on error */
{
  file->fmt=filename->fmt;
  if(file->fmt==CDF)
  {
    if(openclimate_netcdf(file,filename->name,filename->time,filename->var,filename->unit,unit,config))
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
  }
  else
  {
    if(openclmdata(file,filename,name,datatype,scalar,nbands,config))
      return TRUE;
  }
  if(ischeck && file->var_len!=nbands)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR147: Invalid number of bands=%zu in %s data file '%s', must be %d.\n",
             file->var_len,name,filename->name,nbands);
    closeclimatefile(file,isroot(*config));
    return TRUE;
  }
  return FALSE;
} /* of 'opendata_seq' */

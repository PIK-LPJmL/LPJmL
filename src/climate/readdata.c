/**************************************************************************************/
/**                                                                                \n**/
/**                 r  e  a  d  d  a  t  a  .  c                                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions reads real data from clm or NetCDF file                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real *readdata(Climatefile *file,   /**< Pointer to data file */
               const Cell grid[],   /**< LPJ grid */
               const char *name,    /**< name of data data */
               int year,            /**< year (AD) */
               const Config *config /**< LPJ configuration */
              )                     /** \return data or NULL on error */
{
  Real *data;
  year-=file->firstyear;
  if(year>=file->nyear)
    year=file->nyear-1;
  else if(year<0)
    year=0;
  data=newvec(Real,config->ngridcell*file->var_len);
  if(data==NULL)
  {
    printallocerr("data");
    return NULL;
  }
  if(file->fmt==CDF)
  {
    if(readdata_netcdf(file,data,grid,year,config))
    {
      fprintf(stderr,
              "ERROR149: Cannot read %s of year %d in readdata().\n",
               name,year+file->firstyear);
      fflush(stderr);
      free(data);
      return NULL;
    }
  }
  else
  {
    if(fseek(file->file,(long long)year*file->size+file->offset,SEEK_SET))
    {
      fprintf(stderr,
              "ERROR148: Cannot seek %s to year %d in readdata().\n",
              name,year+file->firstyear);
      fflush(stderr);
      free(data);
      return NULL;
    }
    if(readrealvec(file->file,data,0,file->scalar,file->n,
                   file->swap,file->datatype))
    {
      fprintf(stderr,
              "ERROR149: Cannot read %s of year %d in readdata().\n",
              name,year+file->firstyear);
      fflush(stderr);
      free(data);
      return NULL;
    }
  }
  return data;
} /* of 'readdata' */

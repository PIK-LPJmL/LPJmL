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
               Real *data,          /**< pointer to data vector or NULL */
               const Cell grid[],   /**< LPJ grid */
               const char *name,    /**< name of data */
               int year,            /**< year (AD) */
               const Config *config /**< LPJ configuration */
              )                     /** \return data or NULL on error */
{
  Bool isalloc;
  
  if(file->fmt!=SOCK)
  {
    year-=file->firstyear;
    if(year>=file->nyear)
      year=file->nyear-1;
    else if(year<0)
      year=0;
  }
  if(data==NULL)
  {
    /* data is null, allocate data */
    data=newvec(Real,config->ngridcell*file->var_len);
    if(data==NULL)
    {
      printallocerr("data");
      return NULL;
    }
    isalloc=TRUE;
  }
  else
    isalloc=FALSE; /* allocation is not necessary */
  if(file->fmt==CDF)
  {
    if(readdata_netcdf(file,data,grid,year,config))
    {
      fprintf(stderr,"ERROR149: Cannot read %s of year %d in readdata().\n",
              name,year+file->firstyear);
      fflush(stderr);
      if(isalloc)
        free(data);
      return NULL;
    }
  }
  else if(file->fmt==SOCK)
  {
    if(receive_real_copan(file->id,data,file->var_len,year,config))
    {
      fprintf(stderr,"ERROR149: Cannot receive %s of year %d in readdata().\n",name,year);
      fflush(stderr);
      if(isalloc)
        free(data);
      return NULL;
    }
  }
  else
  {
    if(fseek(file->file,(long long)year*file->size+file->offset,SEEK_SET))
    {
      fprintf(stderr,"ERROR148: Cannot seek %s to year %d in readdata().\n",
              name,year+file->firstyear);
      fflush(stderr);
      if(isalloc)
        free(data);
      return NULL;
    }
    if(readrealvec(file->file,data,0,file->scalar,file->n,
                   file->swap,file->datatype))
    {
      fprintf(stderr,"ERROR149: Cannot read %s of year %d in readdata().\n",
              name,year+file->firstyear);
      fflush(stderr);
      if(isalloc)
        free(data);
      return NULL;
    }
  }
  return data;
} /* of 'readdata' */

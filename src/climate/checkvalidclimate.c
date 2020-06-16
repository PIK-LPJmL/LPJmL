/**************************************************************************************/
/**                                                                                \n**/
/**        c  h  e  c  k  v  a  l  i  d  c  l  i  m  a  t  e  .  c                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function checks NetCDF climate files for missing values and skips          \n**/
/**     corresponding LPJ cells                                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

static Bool checkvalid(Climatefile *climate, /**< climate data file */
                       const char *filename, /**< filename */
                       Cell grid[],          /**< LPJ grid */
                       Config *config        /**< LPJ configuration */
                      )                      /** \return TRUE on error */
{
  Bool iserr;
#ifdef USE_MPI
  int i,*counts;
#endif
  int invalid,invalid_total;
  if(climate->fmt==CDF)
  {
    /**
    * check data for first year
    **/
    invalid=checkvalidclimate_netcdf(climate,grid,0,config);
    iserr=(invalid==-1);
    if(iserror(iserr,config))
      return TRUE;
    config->count-=invalid;
#ifdef USE_MPI
    MPI_Reduce(&invalid,&invalid_total,1,MPI_INT,MPI_SUM,0,config->comm);
    counts=newvec(int,config->ntask);
    if(counts==NULL)
    {
      printallocerr("counts");
      iserr=TRUE;
    }
    else
      iserr=FALSE;
    if(iserror(iserr,config))
      return TRUE;
    MPI_Allgather(&config->count,1,MPI_INT,counts,1,MPI_INT,
                  config->comm);
    config->offset=0;
    for(i=0;i<config->rank;i++)
      config->offset+=counts[i];
    MPI_Allreduce(&config->count,&config->total,1,MPI_INT,MPI_SUM,
                  config->comm);
    free(counts);
#else
    invalid_total=invalid;
    config->total=config->count;
#endif
    if(config->total==0)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR207: No cell with valid climate data found in '%s'.\n",filename);
      return TRUE;
    }
    if(isroot(*config) && invalid_total)
      fprintf(stderr,"WARNING008: No climate data for %d cells in '%s', will be skipped.\n",
              invalid_total,filename);
  }
  return FALSE;
} /* of 'checkvalid' */

Bool checkvalidclimate(Climate *climate, /**< pointer to climate data */
                       Cell grid[],      /**< LPJ grid */
                       Config *config    /**< LPJ configuration */
                      )                  /** \return TRUE on error */
{
  /**
  * check data in temperature file
  **/
  if(checkvalid(&climate->file_temp,config->temp_filename.name,grid,config))
    return TRUE;
  if(checkvalid(&climate->file_prec,config->prec_filename.name,grid,config))
    return TRUE;
  switch(config->with_radiation)
  {
    case RADIATION: case RADIATION_LWDOWN:
      if(checkvalid(&climate->file_lwnet,config->lwnet_filename.name,grid,config))
        return TRUE;
      if(checkvalid(&climate->file_swdown,config->swdown_filename.name,grid,config))
        return TRUE;
      break;
    case CLOUDINESS:
      if(checkvalid(&climate->file_cloud,config->cloud_filename.name,grid,config))
        return TRUE;
      break;
    case RADIATION_SWONLY:
      if(checkvalid(&climate->file_swdown,config->swdown_filename.name,grid,config))
        return TRUE;
      break;
  }
  if(config->wet_filename.name!=NULL)
  {
    if(checkvalid(&climate->file_wet,config->wet_filename.name,grid,config))
      return TRUE;
  }
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(checkvalid(&climate->file_wind,config->wind_filename.name,grid,config))
      return TRUE;
    if(checkvalid(&climate->file_tamp,config->tamp_filename.name,grid,config))
      return TRUE;
    if(config->fire==SPITFIRE_TMAX)
    {
      if(checkvalid(&climate->file_tmax,config->tmax_filename.name,grid,config))
        return TRUE;
    }
    if(checkvalid(&climate->file_lightning,config->lightning_filename.name,grid,config))
      return TRUE;
  }
  return FALSE;
} /* of 'checkvalidclimate' */

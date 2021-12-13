/**************************************************************************************/
/**                                                                                \n**/
/**                      g  e  t  c  l  i  m  a  t  e  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function getclimate reads monthly climate data (temperature,               \n**/
/**     precipitation, cloudiness and wet days) for a specified year.              \n**/
/**     The pointer to the climate data has to be initialized by the               \n**/
/**     function initclimate.                                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool readclimate(int what,            /**< which input data */
                 Climatefile *file,   /**< climate data file */
                 Real data[],         /**< climate data read */
                 Real intercept,      /**< offset for data */
                 Real slope,          /**< scale factor for data*/
                 const Cell grid[],   /**< LPJ grid */
                 int year,            /**< year (AD) */
                 const Config *config /**< LPJ configuration */
                )                     /** \return TRUE on error */
{
  int rc;
  long long index;
  if(file->fmt==FMS)
    return FALSE;
  if(file->fmt==SOCK)
  {
    if(receive_real_copan(what,data,(file->time_step==DAY) ? NDAYYEAR : NMONTH,year,config))
    {
      if(isroot(*config))
      {
        fprintf(stderr,"ERROR149: Cannot receive climate of year %d in readclimate().\n",year);
        fflush(stderr);
      }
      return TRUE;
    }
    return FALSE;
  }
  index=year-file->firstyear;
  if(index<0 || index>=file->nyear)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR130: Invalid year %d in readclimate(), must be in [%d,%d].\n",
              year,file->firstyear,file->firstyear+file->nyear-1);
    return TRUE;
  }
  if(file->fmt==CDF)
    rc=readclimate_netcdf(file,data,grid,index,config);
  else
  {
    if(fseek(file->file,index*file->size+file->offset,SEEK_SET))
      rc=TRUE;
    else
      rc=readrealvec(file->file,data,intercept,slope,file->n,file->swap,
                     file->datatype);
  }
  return iserror(rc,config);
} /* of'readclimate' */

Bool getclimate(Climate *climate,    /**< pointer to climate data */
                const Cell grid[],   /**< LPJ grid */
                int year,            /**< year of climate data to be read */
                const Config *config /**< LPJ configuration */
               )                     /** \return TRUE on error */
{
  char *name;
  Real *wet;
  int i,index,year_climate,year_depos;
  Bool rc;
  if(config->const_climate)
    year_climate=climate->firstyear+(year-config->firstyear) % 30;
  else
    year_climate=year;
  if(config->const_deposition)
    year_depos=climate->firstyear+(year-config->firstyear) % 30;
  else
    year_depos=year;
  if(readclimate(TEMP_DATA,&climate->file_temp,climate->data.temp,0,climate->file_temp.scalar,grid,year_climate,config))
  {
    if(isroot(*config))
    {
      name=getrealfilename(&config->temp_filename);
      fprintf(stderr,"ERROR131: Cannot read temperature of year %d from '%s'.\n",
              year_climate,name);
      free(name);
    }
    return TRUE;
  }
  if(readclimate(PREC_DATA,&climate->file_prec,climate->data.prec,0,climate->file_prec.scalar,grid,year_climate,config))
  {
    if(isroot(*config))
    {
      name=getrealfilename(&config->prec_filename);
      fprintf(stderr,"ERROR131: Cannot read precipitation of year %d from '%s'.\n",
              year_climate,name);
      free(name);
    }
    return TRUE;
  }
  if(climate->data.tmax!=NULL)
  {
    if(readclimate(TMAX_DATA,&climate->file_tmax,climate->data.tmax,0,climate->file_tmax.scalar,grid,year_climate,config))
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->tmax_filename);
        fprintf(stderr,"ERROR131: Cannot read tmax of year %d from '%s'.\n",
                year_climate,name);
        free(name);
      }
      return TRUE;
    }
  }
  if(climate->data.tmin!=NULL)
  {
    if(readclimate(TMIN_DATA,&climate->file_tmin,climate->data.tmin,0,climate->file_tmin.scalar,grid,year_climate,config))
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->tmin_filename);
        fprintf(stderr,"ERROR131: Cannot read tmin of year %d from '%s'.\n",
                year_climate,name);
        free(name);
      }
      return TRUE;
    }
  }
  if(climate->data.sun!=NULL)
  {
    if(readclimate(CLOUD_DATA,&climate->file_cloud,climate->data.sun,100,-climate->file_cloud.scalar,grid,year_climate,config))
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->cloud_filename);
        fprintf(stderr,"ERROR131: Cannot read cloudiness of year %d from '%s'.\n",
                year_climate,name);
        free(name);
      }
      return TRUE;
    }
    if(config->cloud_filename.fmt==CDF)
      for(i=0;i<climate->file_cloud.n;i++)
        climate->data.sun[i]=100-climate->data.sun[i];
  }
  if(climate->data.lwnet!=NULL)
  {
    if(readclimate(LWNET_DATA,&climate->file_lwnet,climate->data.lwnet,0,climate->file_lwnet.scalar,grid,year_climate,config))
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->lwnet_filename);
        fprintf(stderr,"ERROR131: Cannot read lwnet of year %d from '%s'.\n",
                year_climate,name);
        free(name);
      }
      return TRUE;
    }
  }
  if(climate->data.swdown!=NULL)
  {
    if(readclimate(SWDOWN_DATA,&climate->file_swdown,climate->data.swdown,0,climate->file_swdown.scalar,grid,year_climate,config))
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->swdown_filename);
        fprintf(stderr,"ERROR131: Cannot read swdown of year %d from '%s'.\n",
                year_climate,name);
        free(name);
      }
      return TRUE;
    }
  }
  if(climate->data.humid!=NULL)
  {
    if(readclimate(HUMID_DATA,&climate->file_humid,climate->data.humid,0,climate->file_humid.scalar,grid,year,config))
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->humid_filename);
        fprintf(stderr,"ERROR131: Cannot read humidity of year %d from '%s'.\n",
                year_climate,name);
        free(name);
      }
      return TRUE;
    }
  }
  if(climate->data.wind!=NULL)
  {
    if(readclimate(WIND_DATA,&climate->file_wind,climate->data.wind,0,climate->file_wind.scalar,grid,year_climate,config))
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->wind_filename);
        fprintf(stderr,"ERROR131: Cannot read wind of year %d from '%s'.\n",
                year_climate,name);
        free(name);
      }
      return TRUE;
    }
  }
  if(climate->data.tamp!=NULL)
  {
    if(readclimate(TAMP_DATA,&climate->file_tamp,climate->data.tamp,0,climate->file_tamp.scalar,grid,year_climate,config))
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->tamp_filename);
        fprintf(stderr,"ERROR131: Cannot read %s of year %d from '%s'.\n",
                (config->tamp_filename.fmt==CDF) ? "tmin" : "tamp",year_climate,name);
        free(name);
      }
      return TRUE;
    }
  }
  if(climate->data.burntarea!=NULL)
  {
    if(readclimate(BURNTAREA_DATA,&climate->file_burntarea,climate->data.burntarea,0,climate->file_burntarea.scalar,grid,year,config))
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->burntarea_filename);
        fprintf(stderr,"ERROR131: Cannot read burntarea of year %d from '%s'.\n",
                year,name);
        free(name);
      }
      return TRUE;
    }
  }
  if(climate->data.no3deposition!=NULL)
  {
    if(readclimate(NO3_DATA,&climate->file_no3deposition,climate->data.no3deposition,0,climate->file_no3deposition.scalar,grid,year_depos,config))
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->no3deposition_filename);
        fprintf(stderr,"ERROR131: Cannot read no3deposition of year %d from '%s'.\n",
                year_depos,name);
        free(name);
      }
      return TRUE;
    }
  }
  if(climate->data.nh4deposition!=NULL)
  {
    if(readclimate(NH4_DATA,&climate->file_nh4deposition,climate->data.nh4deposition,0,climate->file_nh4deposition.scalar,grid,year_depos,config))
    {
      if(isroot(*config))
      {
        name=getrealfilename(&config->nh4deposition_filename);
        fprintf(stderr,"ERROR131: Cannot read nh4deposition of year %d from '%s'.\n",
                year_depos,name);
        free(name);
      }
      return TRUE;
    }
  }
  if(climate->data.wet!=NULL)
  {
    index=year_climate-climate->file_wet.firstyear;
    if(index<0)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR130: Invalid year %d for wet days in getclimate(), must be >%d.\n",
                year_climate,climate->file_wet.firstyear-1);
      return TRUE;
    }
    if(index<climate->file_wet.nyear)
    {
      if(readclimate(WET_DATA,&climate->file_wet,climate->data.wet,0,climate->file_wet.scalar,grid,year_climate,config))
      {
        if(isroot(*config))
        {
          name=getrealfilename(&config->wet_filename);
          fprintf(stderr,"ERROR131: Cannot read wet days of year %d from '%s'.\n",
                  year_climate,name);
          free(name);
        }
        return TRUE;
      }
      climate->file_wet.ready=FALSE;
    }
    else if(!climate->file_wet.ready)
    {
      climate->file_wet.ready=TRUE;
      wet=newvec(Real,climate->file_wet.n);
      if(wet==NULL)
      {
        printallocerr("wet");
        rc=TRUE;
      }
      else
        rc=FALSE;
      if(iserror(rc,config))
        return TRUE;
      for(i=0;i<climate->file_wet.n;i++)
        climate->data.wet[i]=0;
      /**
      * Average number of wet days from 1960 to 1989
      **/
      for(index=1960;index<1990;index++)
      {
        if(readclimate(WET_DATA,&climate->file_wet,wet,0,climate->file_wet.scalar,grid,index,config))
        {
          if(isroot(*config))
          {
            name=getrealfilename(&config->wet_filename);
            fprintf(stderr,"ERROR131: Cannot read wet days of year %d from '%s'.\n",
                    index,name);
            free(name);
          }
          return TRUE;
        }
        for(i=0;i<climate->file_wet.n;i++)
          climate->data.wet[i]+=wet[i];
      }
      for(i=0;i<climate->file_wet.n;i++)
        climate->data.wet[i]/=30;
      free(wet);
    }
  }
  return FALSE;
} /* of 'getclimate' */

/**************************************************************************************/
/**                                                                                \n**/
/**               i  n  i  t  c  l  i  m  a  t  e  .  c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initclimate allocates and initializes the climate data            \n**/
/**     pointer. The corresponding data files are opened and storage               \n**/
/**     allocated.                                                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Climate *initclimate(const Cell grid[],   /**< LPJ grid */
                     const Config *config /**< pointer to LPJ configuration */
                    )                     /** \return allocated climate data struct or NULL on error */
{
  Climate *climate;
  climate=new(Climate);
  if(climate==NULL)
  {
    printallocerr("climate");
    return NULL;
  }
  if(openclimate(&climate->file_temp,&config->temp_filename,"celsius",LPJ_SHORT,config))
  {
    free(climate);
    return NULL;
  }
  if(config->temp_filename.fmt!=CDF && climate->file_temp.version<=1) 
    climate->file_temp.scalar=0.1;
  climate->firstyear=climate->file_temp.firstyear;
  if(openclimate(&climate->file_prec,&config->prec_filename,"kg/m2/day" /* "mm" */,LPJ_SHORT,config))
  {
    closeclimatefile(&climate->file_temp,isroot(*config));
    free(climate);
    return NULL;
  }
  if(climate->firstyear<climate->file_prec.firstyear)
    climate->firstyear=climate->file_prec.firstyear;
  if(config->with_radiation)
  {
    if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
    {
      if(openclimate(&climate->file_lwnet,&config->lwnet_filename,"W/m2",LPJ_SHORT,config))
      {
        closeclimatefile(&climate->file_temp,isroot(*config));
        closeclimatefile(&climate->file_prec,isroot(*config));
        free(climate);
        return NULL;
      }
      if(config->lwnet_filename.fmt!=CDF && climate->file_lwnet.version<=1)
        climate->file_lwnet.scalar=0.1;
      if(climate->firstyear<climate->file_lwnet.firstyear)
        climate->firstyear=climate->file_lwnet.firstyear;
    }
    if(openclimate(&climate->file_swdown,&config->swdown_filename,"W/m2",LPJ_SHORT,config))
    {
      closeclimatefile(&climate->file_temp,isroot(*config));
      closeclimatefile(&climate->file_prec,isroot(*config));
      if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
        closeclimatefile(&climate->file_lwnet,isroot(*config));
      free(climate);
      return NULL;
    }
    if(config->swdown_filename.fmt!=CDF && climate->file_swdown.version<=1)
      climate->file_swdown.scalar=0.1;
    if(climate->firstyear<climate->file_swdown.firstyear)
      climate->firstyear=climate->file_swdown.firstyear;
    climate->data.sun=NULL;
  }
  else
  {
    if(openclimate(&climate->file_cloud,&config->cloud_filename,"%",LPJ_SHORT,config))
    {
      closeclimatefile(&climate->file_temp,isroot(*config));
      closeclimatefile(&climate->file_prec,isroot(*config));
      free(climate);
      return NULL;
    }
    if(climate->firstyear<climate->file_cloud.firstyear)
      climate->firstyear=climate->file_cloud.firstyear;
    climate->data.lwnet=climate->data.swdown=NULL;
  }
  if(config->wet_filename.name!=NULL)
  {
    if(openclimate(&climate->file_wet,&config->wet_filename,NULL,LPJ_SHORT,config))
    {
      closeclimatefile(&climate->file_temp,isroot(*config));
      closeclimatefile(&climate->file_prec,isroot(*config));
      if(config->with_radiation)
      {
        if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
          closeclimatefile(&climate->file_lwnet,isroot(*config));
        closeclimatefile(&climate->file_swdown,isroot(*config));
      }
      else
        closeclimatefile(&climate->file_cloud,isroot(*config));
      free(climate);
      return NULL;
    }
    if(climate->firstyear<climate->file_wet.firstyear)
      climate->firstyear=climate->file_wet.firstyear;
    climate->file_wet.ready=FALSE;
  }
  else
    climate->data.wet=NULL;

  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(config->fdi==WVPD_INDEX)
    {
       if(openclimate(&climate->file_humid,&config->humid_filename,NULL,LPJ_SHORT,config))
       {
         closeclimatefile(&climate->file_temp,isroot(*config));
         closeclimatefile(&climate->file_prec,isroot(*config));
         if(config->with_radiation)
         {
           if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
             closeclimatefile(&climate->file_lwnet,isroot(*config));
           closeclimatefile(&climate->file_swdown,isroot(*config));
         }
         else
           closeclimatefile(&climate->file_cloud,isroot(*config));
         if(config->wet_filename.name!=NULL)
           closeclimatefile(&climate->file_wet,isroot(*config));
         free(climate);
         return NULL;
       }
    }
    if(openclimate(&climate->file_wind,&config->wind_filename,"m/s",LPJ_SHORT,config))
    {
      if(config->fdi==WVPD_INDEX)
        closeclimatefile(&climate->file_humid,isroot(*config));
      closeclimatefile(&climate->file_temp,isroot(*config));
      closeclimatefile(&climate->file_prec,isroot(*config));
      if(config->with_radiation)
      {
        if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
          closeclimatefile(&climate->file_lwnet,isroot(*config));
        closeclimatefile(&climate->file_swdown,isroot(*config));
      }
      else
        closeclimatefile(&climate->file_cloud,isroot(*config));
      if(config->wet_filename.name!=NULL)
        closeclimatefile(&climate->file_wet,isroot(*config));
      free(climate);
      return NULL;
    }
    if(config->wind_filename.fmt!=CDF && climate->file_wind.version<=1)
      climate->file_wind.scalar=0.001;
    if(openclimate(&climate->file_tamp,&config->tamp_filename,(config->tamp_filename.fmt==CDF) ? "celsius" : NULL,LPJ_SHORT,config))
    {
      closeclimatefile(&climate->file_temp,isroot(*config));
      closeclimatefile(&climate->file_prec,isroot(*config));
      if(config->with_radiation)
      {
        if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
          closeclimatefile(&climate->file_lwnet,isroot(*config));
        closeclimatefile(&climate->file_swdown,isroot(*config));
      }
      else
        closeclimatefile(&climate->file_cloud,isroot(*config));
      if(config->wet_filename.name!=NULL)
        closeclimatefile(&climate->file_wet,isroot(*config));
      free(climate);
      return NULL;
    }
    if(config->tamp_filename.fmt!=CDF && climate->file_tamp.version<=1)
      climate->file_tamp.scalar=0.1;
    if(config->tmax_filename.name!=NULL)
    {
      if(openclimate(&climate->file_tmax,&config->tmax_filename,"celsius",LPJ_SHORT,config))
    {
      if(config->fdi==WVPD_INDEX)
        closeclimatefile(&climate->file_humid,isroot(*config));
      closeclimatefile(&climate->file_tamp,isroot(*config));
      closeclimatefile(&climate->file_temp,isroot(*config));
      closeclimatefile(&climate->file_prec,isroot(*config));
      if(config->with_radiation)
      {
        if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
          closeclimatefile(&climate->file_lwnet,isroot(*config));
        closeclimatefile(&climate->file_swdown,isroot(*config));
      }
      else
        closeclimatefile(&climate->file_cloud,isroot(*config));
      if(config->wet_filename.name!=NULL)
        closeclimatefile(&climate->file_wet,isroot(*config));
      free(climate);
      return NULL;
    }
      if(climate->file_tmax.version<=1)
        climate->file_tmax.scalar=0.1;
    }
    if(openclimate(&climate->file_lightning,&config->lightning_filename,"1/day/hectare",LPJ_INT,config))
    {
      if(config->fdi==WVPD_INDEX)
        closeclimatefile(&climate->file_humid,isroot(*config));
      closeclimatefile(&climate->file_tmax,isroot(*config));
      closeclimatefile(&climate->file_tamp,isroot(*config));
      closeclimatefile(&climate->file_temp,isroot(*config));
      closeclimatefile(&climate->file_prec,isroot(*config));
      if(config->with_radiation)
      {
        if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
          closeclimatefile(&climate->file_lwnet,isroot(*config));
        closeclimatefile(&climate->file_swdown,isroot(*config));
      }
      else
        closeclimatefile(&climate->file_cloud,isroot(*config));
      if(config->wet_filename.name!=NULL)
        closeclimatefile(&climate->file_wet,isroot(*config));
      free(climate);
      return NULL;
    }
    if (config->prescribe_burntarea)
    {
      if(openclimate(&climate->file_burntarea,&config->burntarea_filename,(config->burntarea_filename.fmt==CDF) ? NULL : NULL,LPJ_SHORT,config))
      {
        if(config->fdi==WVPD_INDEX)
          closeclimatefile(&climate->file_humid,isroot(*config));
        closeclimatefile(&climate->file_lightning,isroot(*config));
        closeclimatefile(&climate->file_tmax,isroot(*config));
        closeclimatefile(&climate->file_tamp,isroot(*config));
        closeclimatefile(&climate->file_temp,isroot(*config));
        closeclimatefile(&climate->file_prec,isroot(*config));
        if(config->with_radiation)
        {
          if(config->with_radiation==RADIATION)
            closeclimatefile(&climate->file_lwnet,isroot(*config));
          closeclimatefile(&climate->file_swdown,isroot(*config));
        }
        else
          closeclimatefile(&climate->file_cloud,isroot(*config));
        if(config->wet_filename.name!=NULL)
          closeclimatefile(&climate->file_wet,isroot(*config));
        free(climate);
        return NULL;
      }
      if(config->burntarea_filename.fmt!=CDF && climate->file_burntarea.version==1)
        climate->file_burntarea.scalar=100;
    }
    else
     climate->data.burntarea=NULL;
  }
  else
    climate->data.wind=climate->data.tamp=climate->data.lightning=climate->data.burntarea=NULL;

#ifdef IMAGE
  if(config->sim_id==LPJML_IMAGE)
  {
    if(openclimate(&climate->file_temp_var,&config->temp_var_filename,NULL,LPJ_SHORT,config))
    {
      closeclimatefile(&climate->file_temp,isroot(*config));
      closeclimatefile(&climate->file_prec,isroot(*config));
      if(config->with_radiation)
      {
        if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
          closeclimatefile(&climate->file_lwnet,isroot(*config));
        closeclimatefile(&climate->file_swdown,isroot(*config));
      }
      else
        closeclimatefile(&climate->file_cloud,isroot(*config));
      if(config->wet_filename.name!=NULL)
        closeclimatefile(&climate->file_wet,isroot(*config));
      free(climate);
      return NULL;
    }
    if(config->temp_var_filename.fmt!=CDF && climate->file_temp_var.version<=1)
      climate->file_temp_var.scalar=0.1;
    if(openclimate(&climate->file_prec_var,&config->prec_var_filename,NULL,LPJ_SHORT,config))
    {
      closeclimatefile(&climate->file_temp,isroot(*config));
      closeclimatefile(&climate->file_prec,isroot(*config));
      if(config->with_radiation)
      {
        if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
          closeclimatefile(&climate->file_lwnet,isroot(*config));
        closeclimatefile(&climate->file_swdown,isroot(*config));
      }
      else
        closeclimatefile(&climate->file_cloud,isroot(*config));
      if(config->wet_filename.name!=NULL)
        closeclimatefile(&climate->file_wet,isroot(*config));
      closeclimatefile(&climate->file_temp_var,isroot(*config));
      free(climate);
      return NULL;
    }
    if(config->prec_var_filename.fmt!=CDF && climate->file_prec_var.version<=1)
      climate->file_prec_var.scalar=0.01;
  }
  else
    climate->file_temp_var.file=climate->file_prec_var.file=NULL;
#endif
  if(climate->firstyear>config->firstyear)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR200: Climate data starts at %d, later than first simulation year.\n",
              climate->firstyear);
    closeclimatefile(&climate->file_temp,isroot(*config));
    closeclimatefile(&climate->file_prec,isroot(*config));
    if(config->with_radiation)
    {
      if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
        closeclimatefile(&climate->file_lwnet,isroot(*config));
      closeclimatefile(&climate->file_swdown,isroot(*config));
    }
    else
      closeclimatefile(&climate->file_cloud,isroot(*config));
    if(config->wet_filename.name!=NULL)
      closeclimatefile(&climate->file_wet,isroot(*config));
#ifdef IMAGE
    if(config->sim_id==LPJML_IMAGE)
    {
      closeclimatefile(&climate->file_temp_var,isroot(*config));
      closeclimatefile(&climate->file_prec_var,isroot(*config));
    }
#endif
    free(climate);
    return NULL;
  }
  if(readco2(&climate->co2,&config->co2_filename,isroot(*config)))
  {
    free(climate);
    return NULL;
  }
  if(isroot(*config) && climate->co2.firstyear>climate->file_temp.firstyear)
      fprintf(stderr,"WARNING001: First year in '%s'=%d greater than climate->file_temp.firstyear=%d.\n"
              "            Preindustrial value=%g is used.\n",
              config->co2_filename.name,climate->co2.firstyear,climate->file_temp.firstyear,param.co2_p);
#ifdef DEBUG7
  printf("climate->file_temp.firstyear: %d  co2-year: %d  value: %f\n",
         climate->file_temp.firstyear, climate->co2.firstyear,climate->co2.data[0]);
#endif
  if(config->prec_filename.fmt==FMS)
    climate->data.prec=NULL;
  else
  {
    if((climate->data.prec=newvec(Real,climate->file_prec.n))==NULL)
    {
      printallocerr("prec");
      free(climate->co2.data);
      free(climate);
      return NULL;
    }
  }
  if(config->temp_filename.fmt==FMS)
    climate->data.temp=NULL;
  else
  {
    if((climate->data.temp=newvec(Real,climate->file_temp.n))==NULL)
    {
      printallocerr("temp");
      free(climate->co2.data);
      free(climate->data.prec);
      free(climate);
      return NULL;
    }
  }
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(config->fdi==WVPD_INDEX)
    {
      if((climate->data.humid=newvec(Real,climate->file_humid.n))==NULL)
      {
        printallocerr("humid");
        free(climate->co2.data);
        free(climate->data.prec);
        free(climate->data.temp);
        free(climate);
        return NULL;
      }
    }
    else
      climate->data.humid=NULL;
    if(config->wind_filename.fmt==FMS)
      climate->data.wind=NULL;
    else
    {
      if((climate->data.wind=newvec(Real,climate->file_wind.n))==NULL)
      {
        printallocerr("wind");
        free(climate->co2.data);
        free(climate->data.prec);
        free(climate->data.temp);
        free(climate);
        return NULL;
      }
    }
    if(config->tamp_filename.fmt==FMS)
      climate->data.tamp=NULL;
    else
    {
      if((climate->data.tamp=newvec(Real,climate->file_tamp.n))==NULL)
      {
        printallocerr("tamp");
        free(climate->co2.data);
        free(climate->data.wind);
        free(climate->data.prec);
        free(climate->data.temp);
        free(climate);
        return NULL;
      }
    }
    if(config->tmax_filename.name!=NULL)
    {
      if((climate->data.tmax=newvec(Real,climate->file_tmax.n))==NULL)
      {
        printallocerr("tmax");
        free(climate->co2.data);
        free(climate->data.wind);
        free(climate->data.tamp);
        free(climate->data.prec);
        free(climate->data.temp);
        free(climate);
        return NULL;
      }
    }
    else
      climate->data.tmax=NULL;
    if((climate->data.lightning=newvec(Real,climate->file_lightning.n))==NULL)
    {
      printallocerr("lightning");
      free(climate->co2.data);
      free(climate->data.wind);
      free(climate->data.tamp);
      free(climate->data.prec);
      free(climate->data.temp);
      free(climate);
      return NULL;
    }
    if(climate->file_lightning.fmt!=CDF && climate->file_lightning.version<=1)
      climate->file_lightning.scalar=1e-7;
    if(climate->file_lightning.fmt==CDF)
    {
     if(readclimate_netcdf(&climate->file_lightning,climate->data.lightning,grid,0,config))
       return NULL;
    }
    else
    {
      if(fseek(climate->file_lightning.file,climate->file_lightning.offset,SEEK_SET))
      {
        fputs("ERROR191: Cannot seek lightning in initclimate().\n",stderr);
        closeclimatefile(&climate->file_lightning,isroot(*config));
        return NULL;
      }
      if(readrealvec(climate->file_lightning.file,climate->data.lightning,0,climate->file_lightning.scalar,climate->file_lightning.n,climate->file_lightning.swap,climate->file_lightning.datatype))
      {
        fputs("ERROR192: Cannot read lightning in initclimate().\n",stderr);
        closeclimatefile(&climate->file_lightning,isroot(*config));
        return NULL;
      }
      closeclimatefile(&climate->file_lightning,isroot(*config));
    }
  }
  else
    climate->data.wind=climate->data.tamp=climate->data.lightning=climate->data.tmax=climate->data.humid=NULL;
  if(config->with_radiation)
  {
    if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
    {
      if(config->lwnet_filename.fmt==FMS)
        climate->data.lwnet=NULL;
      else
      {
        if((climate->data.lwnet=newvec(Real,climate->file_lwnet.n))==NULL)
        {
          printallocerr("lwnet");
          free(climate->co2.data);
          free(climate->data.prec);
          free(climate->data.temp);
          if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
          {
            free(climate->data.wind);
            free(climate->data.tamp);
            free(climate->data.lightning);
          }
          free(climate);
          return NULL;
        }
      }
    }
    else
      climate->data.lwnet=NULL;
    if(config->swdown_filename.fmt==FMS)
      climate->data.swdown=NULL;
    else
    {
      if((climate->data.swdown=newvec(Real,climate->file_swdown.n))==NULL)
      {
        printallocerr("swdown");
        free(climate->co2.data);
        free(climate->data.prec);
        free(climate->data.temp);
        free(climate->data.lwnet);
        if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
        {
          free(climate->data.wind);
          free(climate->data.tamp);
          free(climate->data.lightning);
        }
        free(climate);
        return NULL;
      }
    }
  }
  else
  {
    if(config->cloud_filename.fmt==FMS)
      climate->data.sun=NULL;
    else
    {
      if((climate->data.sun=newvec(Real,climate->file_cloud.n))==NULL)
      {
        printallocerr("cloud");
        free(climate->co2.data);
        free(climate->data.prec);
        free(climate->data.temp);
        if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
        {
          free(climate->data.wind);
          free(climate->data.tamp);
          free(climate->data.lightning);
        }
        free(climate);
        return NULL;
      }
    }
  }
  if(config->wet_filename.name!=NULL)
  {
    if(config->wet_filename.fmt==FMS)
      climate->data.wet=NULL;
    else
    {
      if((climate->data.wet=newvec(Real,climate->file_wet.n))==NULL)
      {
        printallocerr("wet");
        free(climate->co2.data);
        free(climate->data.prec);
        free(climate->data.temp);
        if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
        {
          free(climate->data.wind);
          free(climate->data.tamp);
          free(climate->data.lightning);
        }
        if(config->with_radiation)
        {
          free(climate->data.lwnet);
          free(climate->data.swdown);
        }
        else
          free(climate->data.sun);
        free(climate);
        return NULL;
      }
    }
  }
  if(config->prescribe_burntarea && (config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX))
  {
    if((climate->data.burntarea=newvec(Real,climate->file_burntarea.n))==NULL)
    {
      printallocerr("burntarea");
      free(climate->co2.data);
      free(climate->data.prec);
      free(climate->data.temp);
      free(climate->data.wind);
      free(climate->data.tamp);
      free(climate->data.lightning);
      if(config->with_radiation)
      {
        free(climate->data.lwnet);
        free(climate->data.swdown);
      }
      else
        free(climate->data.sun);
      if(config->wet_filename.name!=NULL)
        free(climate->data.wet);
      free(climate);
      return NULL;
    }
  }

  return climate;
} /* of 'initclimate' */

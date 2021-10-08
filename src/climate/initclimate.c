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
  char *name;
  int i, ndata; 
  Climate *climate;
  climate=new(Climate);
  if(climate==NULL)
  {
    printallocerr("climate");
    return NULL;
  }
  if(openclimate(&climate->file_temp,&config->temp_filename,"celsius",LPJ_SHORT,config))
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR236: Cannot open temp data from '%s'.\n",config->temp_filename.name);
    free(climate);
    return NULL;
  }
  if((config->temp_filename.fmt==CLM || config->temp_filename.fmt==RAW)  && climate->file_temp.version<=1)
    climate->file_temp.scalar=0.1;
  climate->firstyear=climate->file_temp.firstyear;
  ndata = (config->delta_year>1) ? 3 : 1;
  if (config->isanomaly)
  {
    ndata++;
    if (openclimate(&climate->file_delta_temp, &config->delta_temp_filename, "celsius", LPJ_SHORT, config))
    {
      closeclimatefile(&climate->file_temp, isroot(*config));
      free(climate);
      return NULL;
    }
    if (config->delta_temp_filename.fmt != CDF && climate->file_delta_temp.version == 1)
      climate->file_delta_temp.scalar = 0.1;
    if (openclimate(&climate->file_delta_prec, &config->delta_prec_filename, "kg/m2/day" /* "mm" */, LPJ_SHORT, config))
    {
      closeclimatefile(&climate->file_temp, isroot(*config));
      closeclimatefile(&climate->file_delta_temp, isroot(*config));
      free(climate);
      return NULL;
    }
    if (openclimate(&climate->file_delta_lwnet, &config->delta_lwnet_filename, "W/m2", LPJ_SHORT, config))
    {
      closeclimatefile(&climate->file_temp, isroot(*config));
      closeclimatefile(&climate->file_prec, isroot(*config));
      free(climate);
      return NULL;
    }
    if (config->delta_lwnet_filename.fmt != CDF && climate->file_delta_lwnet.version == 1)
      climate->file_delta_lwnet.scalar = 0.1;
    if (openclimate(&climate->file_delta_swdown, &config->delta_swdown_filename, "W/m2", LPJ_SHORT, config))
    {
      closeclimatefile(&climate->file_temp, isroot(*config));
      closeclimatefile(&climate->file_prec, isroot(*config));
      closeclimatefile(&climate->file_lwnet, isroot(*config));
      free(climate);
      return NULL;
    }
    if (config->delta_swdown_filename.fmt != CDF && climate->file_delta_swdown.version == 1)
      climate->file_delta_swdown.scalar = 0.1;
  }
  if(openclimate(&climate->file_prec,&config->prec_filename,"kg/m2/day" /* "mm" */,LPJ_SHORT,config))
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR236: Cannot open prec data from '%s'.\n",config->prec_filename.name);
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
        if(isroot(*config))
          fprintf(stderr,"ERROR236: Cannot open %s data from '%s'.\n",(config->with_radiation==RADIATION) ? "lwnet" : "lwdown",config->lwnet_filename.name);
        closeclimatefile(&climate->file_temp,isroot(*config));
        closeclimatefile(&climate->file_prec,isroot(*config));
        free(climate);
        return NULL;
      }
      if((config->lwnet_filename.fmt==CLM || config->lwnet_filename.fmt==RAW) && climate->file_lwnet.version<=1)
        climate->file_lwnet.scalar=0.1;
      if(climate->firstyear<climate->file_lwnet.firstyear)
        climate->firstyear=climate->file_lwnet.firstyear;
    }
    if(openclimate(&climate->file_swdown,&config->swdown_filename,"W/m2",LPJ_SHORT,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open swdown data from '%s'.\n",config->swdown_filename.name);
      closeclimatefile(&climate->file_temp,isroot(*config));
      closeclimatefile(&climate->file_prec,isroot(*config));
      if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
        closeclimatefile(&climate->file_lwnet,isroot(*config));
      free(climate);
      return NULL;
    }
    if((config->swdown_filename.fmt==CLM || config->swdown_filename.fmt==RAW) && climate->file_swdown.version<=1)
      climate->file_swdown.scalar=0.1;
    if(climate->firstyear<climate->file_swdown.firstyear)
      climate->firstyear=climate->file_swdown.firstyear;
    for (i = 0; i<ndata; i++)
      climate->data[i].sun=NULL;
  }
  else
  {
    if(openclimate(&climate->file_cloud,&config->cloud_filename,"%",LPJ_SHORT,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open cloud data from '%s'.\n",config->cloud_filename.name);
      closeclimatefile(&climate->file_temp,isroot(*config));
      closeclimatefile(&climate->file_prec,isroot(*config));
      free(climate);
      return NULL;
    }
    if(climate->firstyear<climate->file_cloud.firstyear)
      climate->firstyear=climate->file_cloud.firstyear;
    for (i = 0; i<ndata; i++)
      climate->data[i].lwnet=climate->data[i].swdown=NULL;
  }
  if(config->wet_filename.name!=NULL)
  {
    if(openclimate(&climate->file_wet,&config->wet_filename,NULL,LPJ_SHORT,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open wet data from '%s'.\n",config->wet_filename.name);
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
    for (i = 0; i<ndata; i++)
      climate->data[i].wet=NULL;

  if(config->with_nitrogen && config->with_nitrogen!=UNLIM_NITROGEN && !config->no_ndeposition)
  {
    if(openclimate(&climate->file_no3deposition,&config->no3deposition_filename,"g/m2/day",LPJ_FLOAT,config))
    {
      closeclimatefile(&climate->file_temp,isroot(*config));
      closeclimatefile(&climate->file_prec,isroot(*config));
      if(config->with_radiation)
      {
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
    if(climate->firstyear<climate->file_no3deposition.firstyear)
      climate->firstyear=climate->file_no3deposition.firstyear;
    if(openclimate(&climate->file_nh4deposition,&config->nh4deposition_filename,"g/m2/day",LPJ_FLOAT,config))
    {
      closeclimatefile(&climate->file_temp,isroot(*config));
      closeclimatefile(&climate->file_prec,isroot(*config));
      if(config->with_radiation)
      {
        closeclimatefile(&climate->file_lwnet,isroot(*config));
        closeclimatefile(&climate->file_swdown,isroot(*config));
      }
      else
        closeclimatefile(&climate->file_cloud,isroot(*config));
      if(config->wet_filename.name!=NULL)
        closeclimatefile(&climate->file_wet,isroot(*config));
      closeclimatefile(&climate->file_no3deposition,isroot(*config));
      free(climate);
      return NULL;
    }
    if(climate->firstyear<climate->file_nh4deposition.firstyear)
      climate->firstyear=climate->file_nh4deposition.firstyear;
  }
  else
    for (i = 0; i<ndata; i++)
      climate->data[i].no3deposition=climate->data[i].nh4deposition=NULL;

  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX || config->with_nitrogen)
  {
    if(openclimate(&climate->file_wind,&config->wind_filename,"m/s",LPJ_SHORT,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open wind data from '%s'.\n",config->wind_filename.name);
      if((config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX) && config->fdi==WVPD_INDEX)
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
    if((config->wind_filename.fmt==CLM || config->wind_filename.fmt==RAW)&& climate->file_wind.version<=1)
      climate->file_wind.scalar=0.001;
    if(climate->firstyear<climate->file_wind.firstyear)
      climate->firstyear=climate->file_wind.firstyear;
  }
  else
    for (i = 0; i<ndata; i++)
      climate->data[i].wind=NULL;

  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(config->fdi==WVPD_INDEX)
    {
      if(openclimate(&climate->file_humid,&config->humid_filename,NULL,LPJ_SHORT,config))
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR236: Cannot open humid data from '%s'.\n",config->humid_filename.name);
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
  }
  else
    for (i = 0; i<ndata; i++)
      climate->data[i].humid=NULL;

  if(config->cropsheatfrost || config->fire==SPITFIRE_TMAX)
  {
    if(openclimate(&climate->file_tmin,&config->tmin_filename,"celsius",LPJ_SHORT,config))
    {
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
    if((config->tmin_filename.fmt==CLM || config->tmin_filename.fmt==RAW) &&climate->file_tmin.version<=1)
      climate->file_tmin.scalar=0.1;
    if(openclimate(&climate->file_tmax,&config->tmax_filename,"celsius",LPJ_SHORT,config))
    {
      closeclimatefile(&climate->file_temp,isroot(*config));
      closeclimatefile(&climate->file_prec,isroot(*config));
      closeclimatefile(&climate->file_tmin,isroot(*config));
      if(config->with_radiation)
      {
        if(config->with_radiation==RADIATION||config->with_radiation==RADIATION_LWDOWN)
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
    if((config->tmax_filename.fmt==CLM || config->tmax_filename.fmt==RAW) && climate->file_tmax.version<=1)
      climate->file_tmax.scalar=0.1;
  }
  else
    for (i = 0; i<ndata; i++)
      climate->data[i].tmin=climate->data[i].tmax=NULL;

  if(config->fire==SPITFIRE)
  {
    if(openclimate(&climate->file_tamp,&config->tamp_filename,NULL,LPJ_SHORT,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open 'tamp' data from '%s'.\n",config->tamp_filename.name);
      closeclimatefile(&climate->file_temp,isroot(*config));
      closeclimatefile(&climate->file_prec,isroot(*config));
      if(config->with_radiation)
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
      else
        closeclimatefile(&climate->file_cloud,isroot(*config));
      if(config->wet_filename.name!=NULL)
        closeclimatefile(&climate->file_wet,isroot(*config));
      free(climate);
      return NULL;
    }
    if(config->tamp_filename.fmt!=CDF && climate->file_tamp.version<=1)
      climate->file_tamp.scalar=0.1;
  }
  else
    for (i = 0; i<ndata; i++)
      climate->data[i].tamp=NULL;


  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(openclimate(&climate->file_lightning,&config->lightning_filename,"1/day/hectare",LPJ_INT,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open lightning data from '%s'.\n",config->lightning_filename.name);
      if(config->fdi==WVPD_INDEX)
        closeclimatefile(&climate->file_humid,isroot(*config));
      if(config->cropsheatfrost || config->fire==SPITFIRE_TMAX)
      {
        closeclimatefile(&climate->file_tmin,isroot(*config));
        closeclimatefile(&climate->file_tmax,isroot(*config));
      }
      else
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
      if(openclimate(&climate->file_burntarea,&config->burntarea_filename,(config->burntarea_filename.fmt==CDF) ?  (char *)NULL : (char *)NULL,LPJ_SHORT,config))
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR236: Cannot open burntarea data from '%s'.\n",config->burntarea_filename.name);
        if(config->fdi==WVPD_INDEX)
          closeclimatefile(&climate->file_humid,isroot(*config));
        closeclimatefile(&climate->file_lightning,isroot(*config));
        if(config->cropsheatfrost || config->fire==SPITFIRE_TMAX)
        {
          closeclimatefile(&climate->file_tmin,isroot(*config));
          closeclimatefile(&climate->file_tmax,isroot(*config));
        }
        else
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
      if((config->burntarea_filename.fmt==CLM  || config->burntarea_filename.fmt==RAW) && climate->file_burntarea.version<=1)
        climate->file_burntarea.scalar=100;
    }
    else
      for (i = 0; i<ndata; i++)
        climate->data[i].burntarea=NULL;
  }
  else
    for (i = 0; i<ndata; i++)
      climate->data[i].lightning=climate->data[i].burntarea=NULL;

#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
  {
    if(openclimate(&climate->file_temp_var,&config->temp_var_filename,NULL,LPJ_SHORT,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open tempvar data from '%s'.\n",config->temp_var_filename.name);
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
    if((config->temp_var_filename.fmt==CLM || config->temp_var_filename.fmt==RAW)&& climate->file_temp_var.version<=1)
      climate->file_temp_var.scalar=0.1;
    if(openclimate(&climate->file_prec_var,&config->prec_var_filename,NULL,LPJ_SHORT,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open precvar data from '%s'.\n",config->prec_var_filename.name);
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
    if((config->prec_var_filename.fmt==CLM || config->prec_var_filename.fmt==RAW) && climate->file_prec_var.version<=1)
      climate->file_prec_var.scalar=0.01;
  }
  else
    climate->file_temp_var.file=climate->file_prec_var.file=NULL;
#endif
  if(!config->isanomaly && (config->sim_id == LPJML || config->sim_id == LPJ) && climate->firstyear>config->firstyear)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR200: Climate data starts at %d, later than first simulation year %d.\n",
              climate->firstyear,config->firstyear);
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
#if defined IMAGE && defined COUPLED
    if(config->sim_id==LPJML_IMAGE)
    {
      closeclimatefile(&climate->file_temp_var,isroot(*config));
      closeclimatefile(&climate->file_prec_var,isroot(*config));
    }
#endif
    free(climate);
    return NULL;
  }
  if(readtracegas(&climate->co2,&config->co2_filename,isroot(*config)))
  {
    free(climate);
    return NULL;
  }
  if(isroot(*config) && climate->co2.firstyear>climate->file_temp.firstyear)
      fprintf(stderr,"WARNING001: First year in '%s'=%d greater than climate->file_temp.firstyear=%d.\n"
              "            Preindustrial value=%g for CO2 is used.\n",
              config->co2_filename.name,climate->co2.firstyear,climate->file_temp.firstyear,param.co2_p);
  if (!config->with_dynamic_ch4)
  {
    if (readtracegas(&climate->ch4, &config->ch4_filename, isroot(*config)))
    {
      free(climate);
      return NULL;
    }
    if (isroot(*config) && climate->ch4.firstyear>climate->file_temp.firstyear)
      fprintf(stderr, "WARNING001: First year in '%s'=%d greater than climate->file_temp.firstyear=%d.\n"
        "            value=%g for CH4 is used.\n",
        config->ch4_filename.name, climate->ch4.firstyear, climate->file_temp.firstyear,climate->ch4.data[0]);
  }
#ifdef DEBUG7
  printf("climate->file_temp.firstyear: %d  co2-year: %d  value: %f\n",
         climate->file_temp.firstyear, climate->co2.firstyear,climate->co2.data[0]);
#endif
  if(config->prec_filename.fmt==FMS)
    for (i = 0; i<ndata; i++)
      climate->data[i].prec=NULL;
  else
  {
    for (i = 0; i<ndata; i++)
      if((climate->data[i].prec=newvec(Real,climate->file_prec.n))==NULL)
      {
        printallocerr("prec");
        free(climate->co2.data);
        free(climate);
        return NULL;
      }
  }
  if(config->temp_filename.fmt==FMS)
    for (i = 0; i<ndata; i++)
      climate->data[i].temp=NULL;
  else
  {
    for (i = 0; i<ndata; i++)
      if((climate->data[i].temp=newvec(Real,climate->file_temp.n))==NULL)
      {
        printallocerr("temp");
        free(climate->co2.data);
        for (i = 0; i<ndata; i++)
          free(climate->data[i].prec);
        free(climate);
        return NULL;
    }
  }
  if(config->with_nitrogen && config->with_nitrogen!=UNLIM_NITROGEN && !config->no_ndeposition)
  {
    for (i = 0; i<ndata; i++)
      if((climate->data[i].no3deposition=newvec(Real,climate->file_no3deposition.n))==NULL)
      {
        printallocerr("no3deposition");
        free(climate->co2.data);
        for (i = 0; i<ndata; i++)
        {
          free(climate->data[i].prec);
          free(climate->data[i].temp);
        }
        free(climate);
        return NULL;
      }
    for (i = 0; i<ndata; i++)
      if((climate->data[i].nh4deposition=newvec(Real,climate->file_nh4deposition.n))==NULL)
      {
        printallocerr("nh4deposition");
        free(climate->co2.data);
        free(climate->ch4.data);
        for (i = 0; i<ndata; i++)
        {
          free(climate->data[i].prec);
          free(climate->data[i].temp);
          free(climate->data[i].no3deposition);
        }
        free(climate);
        return NULL;
      }
  }
  if(config->with_nitrogen || config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(config->wind_filename.fmt==FMS)
      for (i = 0; i<ndata; i++)
        climate->data[i].wind=NULL;
    else
    {
      for (i = 0; i<ndata; i++)
        if((climate->data[i].wind=newvec(Real,climate->file_wind.n))==NULL)
        {
          printallocerr("wind");
          free(climate->co2.data);
          free(climate->ch4.data);
          for (i = 0; i<ndata; i++)
          {
            free(climate->data[i].prec);
            free(climate->data[i].temp);
          }
        }
    }
  }
  else
    for (i = 0; i<ndata; i++)
      climate->data[i].wind=NULL;

  if(config->cropsheatfrost || config->fire==SPITFIRE_TMAX)
  {
    for (i = 0; i<ndata; i++)
    {
      if((climate->data[i].tmax=newvec(Real,climate->file_tmax.n))==NULL)
      {
        printallocerr("tmax");
        free(climate->co2.data);
        free(climate->ch4.data);
        for (i = 0; i<ndata; i++)
        {
          free(climate->data[i].wind);
          free(climate->data[i].prec);
          free(climate->data[i].temp);
        }
        free(climate);
        return NULL;
      }
      if((climate->data[i].tmin=newvec(Real,climate->file_tmin.n))==NULL)
      {
        printallocerr("tmin");
        free(climate->co2.data);
        free(climate->ch4.data);
        for (i = 0; i<ndata; i++)
        {
          free(climate->data[i].tmax);
          free(climate->data[i].wind);
          free(climate->data[i].prec);
          free(climate->data[i].temp);
        }
        free(climate);
        return NULL;
      }
    }
  }
  else
    for (i=0;i<ndata; i++)
      climate->data[i].tmax=climate->data[i].tmin=NULL;

  if(config->fire==SPITFIRE)
  {
    if(config->tamp_filename.fmt==FMS)
      for (i = 0; i<ndata; i++)
        climate->data[i].tamp=NULL;
    else
    {
      for (i = 0; i<ndata; i++)
      if((climate->data[i].tamp=newvec(Real,climate->file_tamp.n))==NULL)
      {
        printallocerr("tamp");
        free(climate->co2.data);
        free(climate->ch4.data);
        for (i = 0; i<ndata; i++)
        {
          free(climate->data[i].wind);
          free(climate->data[i].prec);
          free(climate->data[i].temp);
        }
        free(climate);
        return NULL;
      }
    }
  }
  else
    for (i = 0; i<ndata; i++)
      climate->data[i].tamp=NULL;
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(config->fdi==WVPD_INDEX)
    {
      for (i = 0; i<ndata; i++)
        if((climate->data[i].humid=newvec(Real,climate->file_humid.n))==NULL)
        {
          printallocerr("humid");
          free(climate->co2.data);
          for (i = 0; i<ndata; i++)
          {
            free(climate->data[i].tamp);
            free(climate->data[i].tmin);
            free(climate->data[i].wind);
            free(climate->data[i].prec);
            free(climate->data[i].tmax);
            free(climate->data[i].temp);
          }
          return NULL;
        }
    }
    else
      for (i = 0; i<ndata; i++)
        climate->data[i].humid=NULL;
    for (i = 0; i<ndata; i++)
      if((climate->data[i].lightning=newvec(Real,climate->file_lightning.n))==NULL)
      {
        printallocerr("lightning");
        free(climate->co2.data);
        free(climate->ch4.data);
        for (i = 0; i<ndata; i++)
        {
          free(climate->data[i].humid);
          free(climate->data[i].tamp);
          free(climate->data[i].tmin);
          free(climate->data[i].wind);
          free(climate->data[i].prec);
          free(climate->data[i].tmax);
          free(climate->data[i].temp);
        }
        free(climate);
        return NULL;
      }
    if((climate->file_lightning.fmt==CLM || climate->file_lightning.fmt==RAW) && climate->file_lightning.version<=1)
       climate->file_lightning.scalar=1e-7;
    if(climate->file_lightning.fmt==CDF)
    {
      for (i = 0; i<ndata; i++)
        if(readclimate_netcdf(&climate->file_lightning,climate->data[i].lightning,grid,0,config))
          return NULL;
    }
    else
    {
      if(fseek(climate->file_lightning.file,climate->file_lightning.offset,SEEK_SET))
      {
        name=getrealfilename(&config->lightning_filename);
        fprintf(stderr,"ERROR191: Cannot seek lightning in '%s'.\n",name);
        free(name);
        closeclimatefile(&climate->file_lightning,isroot(*config));
        return NULL;
      }
      for (i = 0; i<ndata; i++)
        if(readrealvec(climate->file_lightning.file,climate->data[i].lightning,0,climate->file_lightning.scalar,climate->file_lightning.n,climate->file_lightning.swap,climate->file_lightning.datatype))
        {
          name=getrealfilename(&config->lightning_filename);
          fprintf(stderr,"ERROR192: Cannot read lightning from '%s'.\n",name);
          free(name);
          fputs("ERROR192: Cannot read lightning in initclimate().\n",stderr);
          closeclimatefile(&climate->file_lightning,isroot(*config));
          return NULL;
        }
      closeclimatefile(&climate->file_lightning,isroot(*config));
    }
  } /* of if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX) */
  else
    for (i = 0; i<ndata; i++)
      climate->data[i].tamp=climate->data[i].lightning=climate->data[i].tmax=climate->data[i].humid=NULL;
  if(config->with_radiation)
  {
    if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
    {
      if(config->lwnet_filename.fmt==FMS)
        for (i = 0; i<ndata; i++)
          climate->data[i].lwnet=NULL;
      else
      {
        for (i = 0; i<ndata; i++)
        if((climate->data[i].lwnet=newvec(Real,climate->file_lwnet.n))==NULL)
        {
          printallocerr("lwnet");
          free(climate->co2.data);
          free(climate->ch4.data);
          for (i = 0; i<ndata; i++)
          {
            free(climate->data[i].prec);
            free(climate->data[i].temp);
          }
          if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
          {
            for (i = 0; i < ndata; i++)
            {
              free(climate->data[i].wind);
              free(climate->data[i].tamp);
              free(climate->data[i].lightning);
            }
          }
          free(climate);
          return NULL;
        }
      }
    }
    else
      for (i = 0; i < ndata; i++)
        climate->data[i].lwnet=NULL;
    if(config->swdown_filename.fmt==FMS)
      for (i = 0; i < ndata; i++)
        climate->data[i].swdown=NULL;
    else
    {
      for (i = 0; i < ndata; i++)
        if((climate->data[i].swdown=newvec(Real,climate->file_swdown.n))==NULL)
        {
          printallocerr("swdown");
          free(climate->ch4.data);
          free(climate->co2.data);
          for (i = 0; i < ndata; i++)
          {
            free(climate->data[i].prec);
            free(climate->data[i].temp);
            free(climate->data[i].lwnet);
          }
          if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
          {
            for (i = 0; i < ndata; i++)
            {
              free(climate->data[i].wind);
              free(climate->data[i].tamp);
              free(climate->data[i].lightning);
            }
          }
          free(climate);
          return NULL;
        }
    }
  }
  else
  {
    if(config->cloud_filename.fmt==FMS)
      for (i = 0; i < ndata; i++)
        climate->data[i].sun=NULL;
    else
    {
      for (i = 0; i < ndata; i++)
        if((climate->data[i].sun=newvec(Real,climate->file_cloud.n))==NULL)
        {
          printallocerr("cloud");
          free(climate->co2.data);
          free(climate->ch4.data);
          for (i = 0; i < ndata; i++)
          {
            free(climate->data[i].prec);
            free(climate->data[i].temp);
          }
          if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
          {
            for (i = 0; i < ndata; i++)
            {
              free(climate->data[i].wind);
              free(climate->data[i].tamp);
              free(climate->data[i].lightning);
            }
          }
          free(climate);
          return NULL;
        }
    }
  }
  if(config->wet_filename.name!=NULL)
  {
    if(config->wet_filename.fmt==FMS)
      for (i = 0; i < ndata; i++)
        climate->data[i].wet=NULL;
    else
    {
      for (i = 0; i < ndata; i++)
        if((climate->data[i].wet=newvec(Real,climate->file_wet.n))==NULL)
        {
          printallocerr("wet");
          free(climate->co2.data);
          free(climate->ch4.data);
          for (i = 0; i < ndata; i++)
          {
            free(climate->data[i].prec);
            free(climate->data[i].temp);
          }
          if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
          {
            for (i = 0; i < ndata; i++)
            {
              free(climate->data[i].wind);
              free(climate->data[i].tamp);
              free(climate->data[i].lightning);
            }
          }
          if(config->with_radiation)
          {
            for (i = 0; i < ndata; i++)
            {
              free(climate->data[i].lwnet);
              free(climate->data[i].swdown);
            }
          }
          else
            for (i = 0; i < ndata; i++)
              free(climate->data[i].sun);
          free(climate);
          return NULL;
        }
    }
  }
  else
    for (i = 0; i < ndata; i++)
      climate->data[i].wet=NULL;
  if((config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX) && config->prescribe_burntarea)
  {
    for (i = 0; i < ndata; i++)
      if((climate->data[i].burntarea=newvec(Real,climate->file_burntarea.n))==NULL)
      {
        printallocerr("burntarea");
        free(climate->co2.data);
        free(climate->ch4.data);
        for (i = 0; i < ndata; i++)
        {
          free(climate->data[i].prec);
          free(climate->data[i].temp);
          free(climate->data[i].wind);
          free(climate->data[i].tamp);
          free(climate->data[i].lightning);
        }
        if(config->with_radiation)
        {
          for (i = 0; i < ndata; i++)
          {
            free(climate->data[i].lwnet);
            free(climate->data[i].swdown);
          }
        }
        else
          for (i = 0; i < ndata; i++)
            free(climate->data[i].sun);
        if(config->wet_filename.name!=NULL)
          for (i = 0; i < ndata; i++)
            free(climate->data[i].wet);
        free(climate);
        return NULL;
      }
  }

  return climate;
} /* of 'initclimate' */

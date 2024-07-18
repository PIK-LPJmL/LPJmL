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


static void initdata(Climate *climate)
{
  climate->file_temp.isopen=FALSE;
  climate->file_prec.isopen=FALSE;
  climate->file_lwnet.isopen=FALSE;
  climate->file_swdown.isopen=FALSE;
  climate->file_cloud.isopen=FALSE;
  climate->file_wet.isopen=FALSE;
  climate->file_no3deposition.isopen=FALSE;
  climate->file_nh4deposition.isopen=FALSE;
  climate->file_wind.isopen=FALSE;
  climate->file_humid.isopen=FALSE;
  climate->file_tmin.isopen=FALSE;
  climate->file_tmax.isopen=FALSE;
  climate->file_tamp.isopen=FALSE;
  climate->file_lightning.isopen=FALSE;
  climate->file_burntarea.isopen=FALSE;
#if defined IMAGE && defined COUPLED
  climate->file_temp_var.isopen=FALSE;
  climate->file_prec_var.isopen=FALSE;
#endif
  climate->co2.data=NULL;
  climate->data.prec=NULL;
  climate->data.temp=NULL;
  climate->data.wind=NULL;
  climate->data.tamp=NULL;
  climate->data.lightning=NULL;
  climate->data.lwnet=NULL;
  climate->data.swdown=NULL;
  climate->data.sun=NULL;
  climate->data.wet=NULL;
  climate->data.tmin=NULL;
  climate->data.tmax=NULL;
  climate->data.humid=NULL;
  climate->data.burntarea=NULL;
  climate->data.no3deposition=NULL;
  climate->data.nh4deposition=NULL;
} /* of 'initdata' */

Climate *initclimate(const Cell grid[], /**< LPJ grid */
                     Config *config     /**< pointer to LPJ configuration */
                    )                     /** \return allocated climate data struct or NULL on error */
{
  Climate *climate;
  int lastyear;
  climate=new(Climate);
  if(climate==NULL)
  {
    printallocerr("climate");
    return NULL;
  }
  initdata(climate);
  if(openclimate(&climate->file_temp,&config->temp_filename,"celsius",LPJ_SHORT,0.1,config))
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR236: Cannot open temp data file.\n");
    freeclimate(climate,isroot(*config));
    return NULL;
  }
  climate->firstyear=climate->file_temp.firstyear;
  if(openclimate(&climate->file_prec,&config->prec_filename,"kg/m2/day" /* "mm" */,LPJ_SHORT,1.0,config))
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR236: Cannot open prec data file.\n");
    freeclimate(climate,isroot(*config));
    return NULL;
  }
  if(climate->firstyear<climate->file_prec.firstyear)
    climate->firstyear=climate->file_prec.firstyear;
  if(config->with_radiation)
  {
    if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
    {
      if(openclimate(&climate->file_lwnet,&config->lwnet_filename,"W/m2",LPJ_SHORT,0.1,config))
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR236: Cannot open %s data file.\n",(config->with_radiation==RADIATION) ? "lwnet" : "lwdown");
        freeclimate(climate,isroot(*config));
        return NULL;
      }
      if(climate->firstyear<climate->file_lwnet.firstyear)
        climate->firstyear=climate->file_lwnet.firstyear;
    }
    if(openclimate(&climate->file_swdown,&config->swdown_filename,"W/m2",LPJ_SHORT,0.1,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open swdown data file.\n");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
    if(climate->firstyear<climate->file_swdown.firstyear)
      climate->firstyear=climate->file_swdown.firstyear;
  }
  else
  {
    if(openclimate(&climate->file_cloud,&config->cloud_filename,"%",LPJ_SHORT,1.0,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open cloud data file.\n");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
    if(climate->firstyear<climate->file_cloud.firstyear)
      climate->firstyear=climate->file_cloud.firstyear;
  }
  if(config->wet_filename.name!=NULL)
  {
    if(isdaily(climate->file_prec))
    {
      if(isroot(*config))
        fprintf(stderr,"WARNING039: Number of wet days not used for daily precipitation input, set \"random_prec\" to false.\n");
    }
    else
    {
      if(openclimate(&climate->file_wet,&config->wet_filename,"day",LPJ_SHORT,1.0,config))
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR236: Cannot open wet data file.\n");
        freeclimate(climate,isroot(*config));
        return NULL;
      }
      if(climate->firstyear<climate->file_wet.firstyear)
        climate->firstyear=climate->file_wet.firstyear;
      climate->file_wet.ready=FALSE;
    }
  }
  if(config->with_nitrogen && config->with_nitrogen!=UNLIM_NITROGEN && !config->no_ndeposition)
  {
    if(openclimate(&climate->file_no3deposition,&config->no3deposition_filename,"g/m2/day",LPJ_FLOAT,1.0,config))
    {
      freeclimate(climate,isroot(*config));
      return NULL;
    }
    if(openclimate(&climate->file_nh4deposition,&config->nh4deposition_filename,"g/m2/day",LPJ_FLOAT,1.0,config))
    {
      freeclimate(climate,isroot(*config));
      return NULL;
    }
    if(isroot(*config))
    {
      lastyear=(config->fix_deposition) ? max(config->fix_deposition_year,config->fix_deposition_interval[1]) : config->lastyear;
      if(climate->file_no3deposition.firstyear+climate->file_no3deposition.nyear-1<lastyear)
        fprintf(stderr,"WARNING024: Last year in '%s'=%d is less than last simulation year %d, data from last year used.\n",
                config->no3deposition_filename.name,climate->file_no3deposition.firstyear+climate->file_nh4deposition.nyear-1,lastyear);
      if(climate->file_nh4deposition.firstyear+climate->file_nh4deposition.nyear-1<lastyear)
        fprintf(stderr,"WARNING024: Last year in '%s'=%d is less than last simulation year %d, data from last year used.\n",
                config->nh4deposition_filename.name,climate->file_nh4deposition.firstyear+climate->file_nh4deposition.nyear-1,lastyear);
    }
  }
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX || config->with_nitrogen)
  {
    if(openclimate(&climate->file_wind,&config->wind_filename,"m/s",LPJ_SHORT,0.001,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open wind data file.\n");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
    if(climate->firstyear<climate->file_wind.firstyear)
      climate->firstyear=climate->file_wind.firstyear;
  }
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(config->fdi==WVPD_INDEX)
    {
      if(openclimate(&climate->file_humid,&config->humid_filename,NULL,LPJ_SHORT,1.0,config))
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR236: Cannot open humid data file.\n");
        freeclimate(climate,isroot(*config));
        return NULL;
      }
    }
  }
  if(config->fire==SPITFIRE_TMAX)
  {
    if(openclimate(&climate->file_tmin,&config->tmin_filename,"celsius",LPJ_SHORT,0.1,config))
    {
      freeclimate(climate,isroot(*config));
      return NULL;
    }
    if(openclimate(&climate->file_tmax,&config->tmax_filename,"celsius",LPJ_SHORT,0.1,config))
    {
      freeclimate(climate,isroot(*config));
      return NULL;
    }
  }
  if(config->fire==SPITFIRE)
  {
    if(openclimate(&climate->file_tamp,&config->tamp_filename,NULL,LPJ_SHORT,0.1,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open tamp data file.\n");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
  }
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(openclimate(&climate->file_lightning,&config->lightning_filename,"1/day/hectare",LPJ_INT,1.0e-7,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open lightning data file.\n");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
    if (config->prescribe_burntarea)
    {
      if(openclimate(&climate->file_burntarea,&config->burntarea_filename,(config->burntarea_filename.fmt==CDF) ?  (char *)NULL : (char *)NULL,LPJ_SHORT,100.0,config))
      {
        if(isroot(*config))
          fprintf(stderr,"ERROR236: Cannot open burntarea data file.\n");
        freeclimate(climate,isroot(*config));
        return NULL;
      }
    }
  }

#if defined IMAGE && defined COUPLED
  if(config->sim_id==LPJML_IMAGE)
  {
    if(openclimate(&climate->file_temp_var,&config->temp_var_filename,NULL,LPJ_SHORT,0.1,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open tempvar data file.\n");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
    if(openclimate(&climate->file_prec_var,&config->prec_var_filename,NULL,LPJ_SHORT,0.01,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR236: Cannot open precvar data file.\n");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
  }
#endif
  if(climate->firstyear>config->firstyear)
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR200: Climate data starts at %d, later than first simulation year %d.\n",
              climate->firstyear,config->firstyear);
    freeclimate(climate,isroot(*config));
    return NULL;
  }
  if(readco2(&climate->co2,&config->co2_filename,config))
  {
    if(isroot(*config))
      fprintf(stderr,"ERROR236: Cannot read CO2 data file.\n");
    freeclimate(climate,isroot(*config));
    return NULL;
  }
  if(isroot(*config) && climate->co2.firstyear>config->firstyear-config->nspinup)
      fprintf(stderr,"WARNING001: First year in CO2 data file '%s'=%d greater than first simulation year %d,\n"
              "            value=%g ppm of year %d is used.\n",
              config->co2_filename.name,climate->co2.firstyear,config->firstyear-config->nspinup,
              climate->co2.data[0],climate->co2.firstyear);
#ifdef DEBUG7
  printf("climate->file_temp.firstyear: %d  co2-year: %d  value: %f\n",
         climate->file_temp.firstyear, climate->co2.firstyear,climate->co2.data[0]);
#endif
  if(config->prec_filename.fmt!=FMS)
  {
    if((climate->data.prec=newvec(Real,climate->file_prec.n))==NULL)
    {
      printallocerr("prec");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
  }
  if(config->temp_filename.fmt!=FMS)
  {
    if((climate->data.temp=newvec(Real,climate->file_temp.n))==NULL)
    {
      printallocerr("temp");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
  }
  if(config->with_nitrogen && config->with_nitrogen!=UNLIM_NITROGEN && !config->no_ndeposition)
  {
    if((climate->data.no3deposition=newvec(Real,climate->file_no3deposition.n))==NULL)
    {
      printallocerr("no3deposition");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
    if((climate->data.nh4deposition=newvec(Real,climate->file_nh4deposition.n))==NULL)
    {
      printallocerr("nh4deposition");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
  }
  if(config->with_nitrogen || config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(config->wind_filename.fmt!=FMS)
    {
      if((climate->data.wind=newvec(Real,climate->file_wind.n))==NULL)
      {
        printallocerr("wind");
        freeclimate(climate,isroot(*config));
        return NULL;
      }
    }
  }
  if(config->fire==SPITFIRE_TMAX)
  {
    if((climate->data.tmax=newvec(Real,climate->file_tmax.n))==NULL)
    {
      printallocerr("tmax");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
    if((climate->data.tmin=newvec(Real,climate->file_tmin.n))==NULL)
    {
      printallocerr("tmin");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
  }
  if(config->fire==SPITFIRE)
  {
    if(config->tamp_filename.fmt!=FMS)
    {
      if((climate->data.tamp=newvec(Real,climate->file_tamp.n))==NULL)
      {
        printallocerr("tamp");
        freeclimate(climate,isroot(*config));
        return NULL;
      }
    }
  }
  if(config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX)
  {
    if(config->fdi==WVPD_INDEX)
    {
      if((climate->data.humid=newvec(Real,climate->file_humid.n))==NULL)
      {
        printallocerr("humid");
        freeclimate(climate,isroot(*config));
        return NULL;
      }
    }
    if((climate->data.lightning=newvec(Real,climate->file_lightning.n))==NULL)
    {
      printallocerr("lightning");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
    if(readclimate(&climate->file_lightning,climate->data.lightning,0,climate->file_lightning.scalar,grid,climate->file_lightning.firstyear,config))
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR192: Cannot read lightning.\n");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
    closeclimatefile(&climate->file_lightning,isroot(*config));
  }
  if(config->with_radiation)
  {
    if(config->with_radiation==RADIATION || config->with_radiation==RADIATION_LWDOWN)
    {
      if(config->lwnet_filename.fmt!=FMS)
      {
        if((climate->data.lwnet=newvec(Real,climate->file_lwnet.n))==NULL)
        {
          printallocerr("lwnet");
          freeclimate(climate,isroot(*config));
          return NULL;
        }
      }
    }
    if(config->swdown_filename.fmt!=FMS)
    {
      if((climate->data.swdown=newvec(Real,climate->file_swdown.n))==NULL)
      {
        printallocerr("swdown");
        freeclimate(climate,isroot(*config));
        return NULL;
      }
    }
  }
  else
  {
    if(config->cloud_filename.fmt!=FMS)
    {
      if((climate->data.sun=newvec(Real,climate->file_cloud.n))==NULL)
      {
        printallocerr("cloud");
        freeclimate(climate,isroot(*config));
        return NULL;
      }
    }
  }
  if(config->wet_filename.name!=NULL && !isdaily(climate->file_prec) && config->wet_filename.fmt!=FMS)
  {
    if((climate->data.wet=newvec(Real,climate->file_wet.n))==NULL)
    {
      printallocerr("wet");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
  }
  if((config->fire==SPITFIRE || config->fire==SPITFIRE_TMAX) && config->prescribe_burntarea)
  {
    if((climate->data.burntarea=newvec(Real,climate->file_burntarea.n))==NULL)
    {
      printallocerr("burntarea");
      freeclimate(climate,isroot(*config));
      return NULL;
    }
  }
  return climate;
} /* of 'initclimate' */

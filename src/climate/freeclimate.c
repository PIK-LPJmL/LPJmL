/**************************************************************************************/
/**                                                                                \n**/
/**                   f  r  e  e  c  l  i  m  a  t  e  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function closes open files and frees allocated memory                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void freeclimatedata2(Climate *climate)
{
  int i;
  free(climate->co2.data);
  free(climate->ch4.data);
  for (i = 0; i<4; i++)
  {
    free(climate->data[i].tmax);
    free(climate->data[i].tmin);
    free(climate->data[i].prec);
    free(climate->data[i].temp);
    free(climate->data[i].sun);
    free(climate->data[i].lwnet);
    free(climate->data[i].swdown);
    free(climate->data[i].wet);
    free(climate->data[i].wind);
    free(climate->data[i].humid);
    free(climate->data[i].burntarea);
    free(climate->data[i].tamp);
    free(climate->data[i].no3deposition);
    free(climate->data[i].nh4deposition);
    free(climate->data[i].lightning);
  }
} /* of 'freeclimatedata2' */

void closeclimateanomalies(Climate *climate,    /**< pointer to climate data */
                           const Config *config /**< pointer to LPJ configuration */
                          )
{
  if(config->isanomaly)
  {
    closeclimatefile(&climate->file_delta_temp,isroot(*config));
    closeclimatefile(&climate->file_delta_prec,isroot(*config));
    switch(config->with_radiation)
    {
      //case CLOUDINESS:
      //  closeclimatefile(&climate->file_delta_cloud,isroot(*config));
     //   break;
      case RADIATION: case RADIATION_LWDOWN:
        closeclimatefile(&climate->file_delta_swdown,isroot(*config));
        closeclimatefile(&climate->file_delta_lwnet,isroot(*config));
        break;
      case RADIATION_SWONLY:
        closeclimatefile(&climate->file_delta_swdown,isroot(*config));
        break;
    }
  }
} /* of 'closeclimateanomalies' */

void closeclimatefiles(Climate *climate,    /**< pointer to climate data */
                       const Config *config /**< pointer to LPJ configuration */
                      )
{
  closeclimatefile(&climate->file_temp,isroot(*config));
  closeclimatefile(&climate->file_prec,isroot(*config));
  if(climate->data[0].tmax!=NULL)
    closeclimatefile(&climate->file_tmax,isroot(*config));
  if(climate->data[0].tmin!=NULL)
    closeclimatefile(&climate->file_tmin,isroot(*config));
  if(climate->data[0].wind!=NULL)
    closeclimatefile(&climate->file_wind,isroot(*config));
  if(climate->data[0].sun!=NULL)
    closeclimatefile(&climate->file_cloud,isroot(*config));
  if(climate->data[0].lwnet!=NULL)
    closeclimatefile(&climate->file_lwnet,isroot(*config));
  if(climate->data[0].swdown!=NULL)
    closeclimatefile(&climate->file_swdown,isroot(*config));
  if(climate->data[0].humid!=NULL)
    closeclimatefile(&climate->file_humid,isroot(*config));
  if(climate->data[0].tamp!=NULL)
    closeclimatefile(&climate->file_tamp,isroot(*config));
  if(climate->data[0].wet!=NULL)
    closeclimatefile(&climate->file_wet,isroot(*config));
  if(climate->data[0].burntarea!=NULL)
    closeclimatefile(&climate->file_burntarea,isroot(*config));
  if(climate->data[0].no3deposition!=NULL)
    closeclimatefile(&climate->file_no3deposition,isroot(*config));
  if(climate->data[0].nh4deposition!=NULL)
    closeclimatefile(&climate->file_nh4deposition,isroot(*config));
  closeclimateanomalies(climate,config);
#if defined IMAGE && defined COUPLED
  if(climate->file_temp_var.file!=NULL)
    closeclimatefile(&climate->file_temp_var,isroot(*config));
  if(climate->file_prec_var.file!=NULL)
    closeclimatefile(&climate->file_prec_var,isroot(*config));
#endif
} /* of 'closeclimatefiles' */

void freeclimatedata(Climatedata *data /**< pointer to climate data */
                    )                  /** \return void */
{
  free(data->tmax);
  free(data->tmin);
  free(data->prec);
  free(data->temp);
  free(data->sun);
  free(data->lwnet);
  free(data->swdown);
  free(data->wet);
  free(data->humid);
  free(data->wind);
  free(data->tamp);
  free(data->burntarea);
  free(data->lightning);
  free(data->no3deposition);
  free(data->nh4deposition);
} /* of 'freeclimatedata' */

void freeclimate(Climate *climate,    /**< pointer to climate data */
                 const Config *config /**< LPJ configuration */
                )                     /** \return void */
{
  if(climate!=NULL)
  {
    closeclimatefiles(climate,config);
    freeclimatedata2(climate);
  }
  free(climate);
} /* of 'freeclimate' */

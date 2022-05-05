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

static void freeclimatedata2(Climate *climate)
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

void freeclimate(Climate *climate, /**< pointer to climate data */
                 Bool isroot       /**< task is root task */
                )                  /** \return void */
{
  if(climate!=NULL)
  {
    closeclimatefile(&climate->file_temp,isroot);
    closeclimatefile(&climate->file_prec,isroot);
    closeclimatefile(&climate->file_tmax,isroot);
    closeclimatefile(&climate->file_tmin,isroot);
    closeclimatefile(&climate->file_cloud,isroot);
    closeclimatefile(&climate->file_lwnet,isroot);
    closeclimatefile(&climate->file_swdown,isroot);
    closeclimatefile(&climate->file_humid,isroot);
    closeclimatefile(&climate->file_wind,isroot);
    closeclimatefile(&climate->file_tamp,isroot);
    closeclimatefile(&climate->file_wet,isroot);
    closeclimatefile(&climate->file_burntarea,isroot);
    closeclimatefile(&climate->file_no3deposition,isroot);
    closeclimatefile(&climate->file_nh4deposition,isroot);
    closeclimatefile(&climate->file_delta_temp,isroot(*config));
    closeclimatefile(&climate->file_delta_prec,isroot(*config));
    closeclimatefile(&climate->file_delta_swdown,isroot(*config));
    closeclimatefile(&climate->file_delta_lwnet,isroot(*config));
    closeclimatefile(&climate->file_delta_swdown,isroot(*config));
#if defined IMAGE && defined COUPLED
    closeclimatefile(&climate->file_temp_var,isroot);
    closeclimatefile(&climate->file_prec_var,isroot);
#endif
    freeclimatedata2(climate);
    free(climate);
  }
} /* of 'freeclimate' */

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

void freeclimatedata(Climatedata *data /**< pointer to climate data */
                    )                  /** \return void */
{
  free(data->tmax);
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
} /* of 'freeclimatedata' */

void freeclimate(Climate *climate, /**< pointer to climate data */
                 Bool isroot       /**< task is root task */
                 )                 /** \return void */
{
  if(climate!=NULL)
  {
    closeclimatefile(&climate->file_temp,isroot);
    closeclimatefile(&climate->file_prec,isroot);
    if(climate->data.tmax!=NULL)
      closeclimatefile(&climate->file_tmax,isroot);
    if(climate->data.sun!=NULL)
      closeclimatefile(&climate->file_cloud,isroot);
    if(climate->data.lwnet!=NULL)
      closeclimatefile(&climate->file_lwnet,isroot);
    if(climate->data.swdown!=NULL)
      closeclimatefile(&climate->file_swdown,isroot);
    if(climate->data.humid!=NULL)
      closeclimatefile(&climate->file_humid,isroot);
    if(climate->data.wind!=NULL)
      closeclimatefile(&climate->file_wind,isroot);
    if(climate->data.tamp!=NULL)
      closeclimatefile(&climate->file_tamp,isroot);
    if(climate->data.wet!=NULL)
      closeclimatefile(&climate->file_wet,isroot);
   if(climate->data.burntarea!=NULL)
       closeclimatefile(&climate->file_burntarea,isroot);
#ifdef IMAGE
    if(climate->file_temp_var.file!=NULL)
      closeclimatefile(&climate->file_temp_var,isroot);
    if(climate->file_prec_var.file!=NULL)
      closeclimatefile(&climate->file_prec_var,isroot);
#endif
    free(climate->co2.data);
    freeclimatedata(&climate->data);
  }
  free(climate);
} /* of 'freeclimate' */

/**************************************************************************************/
/**                                                                                \n**/
/**                d  a  i  l  y  c  l  i  m  a  t  e  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function dailyclimate gets daily value for temperature,                    \n**/
/**     precipitation, cloudiness and wet days.                                    \n**/
/**     The pointer to the climate data has to be initialized by the               \n**/
/**     function getclimate for the specified year.                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void dailyclimate(Dailyclimate *daily,    /**< */
                  const Climate *climate, /**< climate data pointer */
                  Climbuf *climbuf,       /**< climate buffer pointer*/
                  int cell,               /**< cell index */
                  int day,                /**< day (1..365) */
                  int month,              /**< month (0..11) */
                  int dayofmonth          /**< day of the month */
                  )                       /** \return void */
{
  Real tamp;
  if(climate->file_temp.fmt!=FMS)
  {
    daily->isdailytemp=isdaily(climate->file_temp);
    if(isdaily(climate->file_temp))
    {
      daily->temp=climate->data.temp[cell*NDAYYEAR+day-1];
      climbuf->mtemp+=daily->temp;
    }
    else
      daily->temp=interpolate(getcelltemp(climate,cell),month,dayofmonth);
  }
  else
  {
    daily->isdailytemp=TRUE;
    climbuf->mtemp+=daily->temp;
  }
  if(climate->data.sun!=NULL)
    daily->sun=isdaily(climate->file_cloud) ? climate->data.sun[cell*NDAYYEAR+day-1]
                      : interpolate(getcellsun(climate,cell),month,dayofmonth);
  if(climate->data.lwnet!=NULL)
    daily->lwnet=isdaily(climate->file_lwnet) ? climate->data.lwnet[cell*NDAYYEAR+day-1]
                      : interpolate(getcelllwnet(climate,cell),month,dayofmonth);
  if(climate->data.swdown!=NULL)
    daily->swdown=isdaily(climate->file_swdown) ? climate->data.swdown[cell*NDAYYEAR+day-1]
                      : interpolate(getcellswdown(climate,cell),month,dayofmonth);
  if(climate->data.humid!=NULL)
    daily->humid=isdaily(climate->file_humid) ? climate->data.humid[cell*NDAYYEAR+day-1]
                      : interpolate(getcellhumid(climate,cell),month,dayofmonth);
  if(climate->data.wind!=NULL)
  {
    if(isdaily(climate->file_wind))
      daily->windspeed=climate->data.wind[cell*NDAYYEAR+day-1];
    else
      daily->windspeed=interpolate(getcellwind(climate,cell),month,dayofmonth);
  }
  if(climate->data.tamp!=NULL)
  {
    if(climate->data.tmax!=NULL)
    {
      daily->tmin=isdaily(climate->file_tamp) ?  climate->data.tamp[cell*NDAYYEAR+day-1] : interpolate(getcelltamp(climate,cell),month,dayofmonth);
      daily->tmax=isdaily(climate->file_tmax) ?  climate->data.tmax[cell*NDAYYEAR+day-1] : interpolate(getcelltmax(climate,cell),month,dayofmonth);
    }
    else
    {
      tamp=isdaily(climate->file_tamp) ?  climate->data.tamp[cell*NDAYYEAR+day-1] : interpolate(getcelltamp(climate,cell),month,dayofmonth);
      daily->tmin=daily->temp-tamp*0.5;
      daily->tmax=daily->temp+tamp*0.5;
    }
  }
  if(climate->data.lightning!=NULL)
  {
    if(isdaily(climate->file_lightning))
      daily->lightning=climate->data.lightning[cell*NDAYYEAR+day-1];
    else
      daily->lightning=interpolate(getcelllightning(climate,cell),month,dayofmonth)*ndaymonth1[month];
  }
  if(climate->file_prec.fmt!=FMS)
  {
    if(isdaily(climate->file_prec))
    {
      daily->prec=climate->data.prec[cell*NDAYYEAR+day-1];
      climbuf->mprec+=daily->prec;
    }
    else
    {
      daily->prec=(israndomprec(climate)) ? climbuf->dval_prec[dayofmonth+1] :
          interpolate(getcellprec(climate,cell),month,dayofmonth)*
                        ndaymonth1[month];
      daily->prec=(daily->prec>0.000001) ? daily->prec : 0.0;
    }
  }
  else
    climbuf->mprec+=daily->prec;
 
  if(climate->data.burntarea!=NULL)
   {
    daily->burntarea=isdaily(climate->file_burntarea) ? climate->data.burntarea[cell*NDAYYEAR+day-1]
                     : climate->data.burntarea[cell*12+month] * ndaymonth1[month]; 
    daily->burntarea=(daily->burntarea > 0.000001) ? daily->burntarea : 0.0;
  }
} /* of 'dailyclimate' */

/**************************************************************************************/
/**                                                                                \n**/
/**                    s  o  i  l  t  e  m  p  _  l  a  g  .  c                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real soiltemp_lag(const Soil *soil,      /**< Soil data */
                  const Climbuf *climbuf /**< Climate buffer */
                 )                       /** \return soil temperature (deg C) */
{
  Real a,b,temp_lag;
  if(soil->w[0]<epsilon)
    return climbuf->temp[NDAYS-1];
  linreg(&a,&b,climbuf->temp,NDAYS);
  temp_lag=a+b*(NDAYS-1-soil->alag*LAG_CONV);
  return climbuf->atemp_mean+soil->amp*(temp_lag-climbuf->atemp_mean);
} /* of 'soiltemp_lag' */

/**************************************************************************************/
/**                                                                                \n**/
/**                      g  e  t  c  o  2  .  c                                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function get atmospheric CO2 concentration for specified year.             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"


Real getco2(const Climate *climate, /**< Pointer to climate data */
            int year                /**< year (AD) */
           )                        /** \return atmospheric CO2 (ppm) */
{
  year-=climate->co2.firstyear;
  if(year>=climate->co2.nyear)
    fail(INVALID_YEAR_ERR,FALSE,"Invalid yr=%d in getco2()",
         year+climate->co2.firstyear);
  return (year<0) ? param.co2_p : climate->co2.data[year];
} /* of 'getco2' */

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

Bool getco2(const Climate *climate, /**< Pointer to climate data */
            Real *pco2,             /** atmospheric CO2 (ppm) */
            int year,                /**< year (AD) */
            const Config *config    /**< LPJmL configuration */
           )                        /** \return TRUE on error */
{
  if(config->co2_filename.fmt==SOCK)
    return receive_real_scalar_copan(pco2,1,config);
  else
  {
    year-=climate->co2.firstyear;
    if(year>=climate->co2.nyear)
      return TRUE;
    *pco2=(year<0) ? param.co2_p : climate->co2.data[year];
  }
  return FALSE;
} /* of 'getco2' */

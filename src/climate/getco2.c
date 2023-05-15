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
            Real *pco2,             /**< atmospheric CO2 (ppm) */
            int year,               /**< year (AD) */
            const Config *config    /**< LPJmL configuration */
           )                        /** \return TRUE on error */
{
  if(iscoupled(*config) && config->co2_filename.issocket && year>=config->start_coupling)
  {
    if(receive_real_scalar_coupler(climate->co2.id,pco2,1,year,config))
      return TRUE;
    if(*pco2<0)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR246: Atmospheric CO2=%g less than zero in year %d.\n",*pco2,year);
      return TRUE;
    }
  }
  else
  {
    year-=climate->co2.firstyear;
    if(year>=climate->co2.nyear)
    {
      if(isroot(*config))
        fprintf(stderr,"ERROR015: Invalid year %d in getco2(), must be <%d.\n",
                year+climate->co2.firstyear,climate->co2.firstyear+climate->co2.nyear);

      return TRUE;
    }
    *pco2=(year<0) ? param.co2_p : climate->co2.data[year];
  }
  return FALSE;
} /* of 'getco2' */

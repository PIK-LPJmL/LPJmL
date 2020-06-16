/**************************************************************************************/
/**                                                                                \n**/
/**                   r  a  d  i  a  t  i  o  n  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates daily photosynthetically active                        \n**/
/**     radiation flux, daylength and equilibrium evapotranspiration               \n**/
/**     from daily climate data, latidude, day of year and albedo                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void radiation(Real *daylength,       /**< daylength (h) */
               Real *par,             /**< photosynthetic active radiation flux (J/m2/day) */
               Real *eeq,             /**< equilibrium evapotranspiration (mm) */
               Real lat,              /**< latitude (deg) */
               int day,	              /**< day (1..365) */
               Dailyclimate *climate, /**< daily climate data */
               Real beta,             /**< albedo */
               int with_radiation     /**< CLOUDINESS/RADIATION/RADIATION_LWDOWN/RADIATION_SWONLY */
              )
{
  switch(with_radiation)
  {
    case CLOUDINESS:
      petpar(daylength,par,eeq,&climate->swdown,lat,day,climate->temp,
             climate->sun,beta);
      break;
    case RADIATION:
      petpar2(daylength,par,eeq,lat,day,climate->temp,
              climate->lwnet,climate->swdown,FALSE,beta);
      break;
    case RADIATION_LWDOWN:
      petpar2(daylength,par,eeq,lat,day,climate->temp,
              climate->lwnet,climate->swdown,TRUE,beta);
      break;
    case RADIATION_SWONLY:
      petpar3(daylength,par,eeq,lat,day,climate->temp,climate->swdown,beta);
      break;
#ifdef SAFE
    default:
      fail(INVALID_RADIATION_ERR,FALSE,
           "Invalid radiation model %d in radiation()",with_radiation);
#endif
  }
} /* of 'radiation' */ 

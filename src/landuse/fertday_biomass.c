/**************************************************************************************/
/**                                                                                \n**/
/**                  f  e  r  t  d  a  y  _  b  i  o  m  a  s  s  .  c             \n**/
/**                                                                                \n**/
/**     Function calculates day of fertilizer application for biomass              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int fertday_biomass(const Cell *cell,    /**< pointer to cell */
                    const Config *config /**< LPJmL configuration */
                   )                     /** \return day of application (1..365) */
{
  int fday;
  if (config->sdate_option>=PRESCRIBED_SDATE)
  {
    if(fabs(cell->coord.lat) > 30)
      fday=cell->ml.sdate_fixed[param.cft_fertday_temp];  /* wheat sowing date */
    else
      fday=cell->ml.sdate_fixed[param.cft_fertday_tropic];  /* maize sowing date */
  }
  else
  {
    if(fabs(cell->coord.lat) > 30)
      fday=cell->ml.cropdates[param.cft_fertday_temp].spring_sdate20;  /* wheat sowing date */
    else
      fday=cell->ml.cropdates[param.cft_fertday_tropic].spring_sdate20;  /* maize sowing date */
  }
  if (fday==0)
    fday=(cell->coord.lat >= 0.0) ? 91 /* 1st of April */ : 274; /* 1st of October */
  return fday;
} /* of 'fertday_biomass' */

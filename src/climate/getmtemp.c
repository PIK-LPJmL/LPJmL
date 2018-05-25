/**************************************************************************************/
/**                                                                                \n**/
/**                      g  e  t  m  t  e  m  p  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function getmtemp gets monthly value of temperature                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real getmtemp(const Climate *climate, /**< pointer to climate data */
              const Climbuf *climbuf, /**< pointer to climate buffer */
              int cell,               /**< cell index */
              int month               /**< month (0..11) */
             )                        /** \return monthly averaged temperature (deg C) */
{
  return isdaily(climate->file_temp) ? climbuf->mtemp/ndaymonth[month] : (getcelltemp(climate,cell))[month];
} /* of 'getmtemp' */

/**************************************************************************************/
/**                                                                                \n**/
/**                      g  e  t  m p r e c  .  c                                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function getmprec gets monthly value of precipitation                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real getmprec(const Climate *climate, /**< pointer to climate data */
              const Climbuf *climbuf, /**< pointer to climate buffer */
              int cell,               /**< cell index */
              int month               /**< month (0..11) */
             )                        /** \return monthly averaged precipitation (mm) */
{
  return isdaily(climate->file_prec) ? climbuf->mprec : (getcellprec(climate,cell))[month];
} /* of 'getmprec' */

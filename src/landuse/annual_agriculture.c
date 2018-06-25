/**************************************************************************************/
/**                                                                                \n**/
/**        a  n  n  u  a  l  _  a  g  r  i  c  u  l  t  u  r  e  .  c              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function performs necessary updates after iteration over one               \n**/
/**     year for agriculture stand                                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "agriculture.h"

Bool annual_agriculture(Stand *stand,         /**< Pointer to stand */
                        int UNUSED(npft),     /**< number of natural pfts */
                        int UNUSED(ncft),     /**< number of crop PFTs */
                        Real UNUSED(popdens), /**< population density (capita/km2) */
                        int UNUSED(year),     /**< simulation year */
                        Bool UNUSED(isdaily),
                        Bool UNUSED(intercrop), /**< enable intercropping (TRUE/FALSE) */
                        const Config * UNUSED(config)
                       )                      /** \return stand has to be killed (TRUE/FALSE) */
{
  Irrigation *data;
  data=stand->data;
  stand->cell->output.soil_storage+=(data->irrig_stor+data->irrig_amount)*stand->frac*stand->cell->coord.area;
  return FALSE;
} /* of 'annual_agriculture' */

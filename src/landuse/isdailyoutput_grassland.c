/**************************************************************************************/
/**                                                                                \n**/
/**                i s d a i l y o u t p u t _ g r a s s l a n d . c               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grassland.h"

Bool isdailyoutput_grassland(const Output *output, /**< Output data */
                             const Stand *stand    /**< stand pointer */
                           )
{
  const Grassland *data;
  data = stand->data;
  return (output->daily.cft==TEMPERATE_HERBACEOUS && 
     output->daily.irrigation==data->irrigation.irrigation);
} /* of 'isdailyoutput_grassland' */

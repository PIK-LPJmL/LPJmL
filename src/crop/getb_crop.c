/**************************************************************************************/
/**                                                                                \n**/
/**                        g  e  t  b  _  c  r  o  p  .  c                         \n**/
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
#include "crop.h"

void getb_crop(Pft *pft, /**< pointer to PFT variables */
               Real gdd  /**< growing degree days above 0 degC */
              )
{
  pft->b=pft->par->b;
} /* of 'getb_crop' */

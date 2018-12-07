/**************************************************************************************/
/**                                                                                \n**/
/**             v  e  g  c  _  s  u  m  _  c  r  o  p  .  c                        \n**/
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

Real vegc_sum_crop(const Pft *pft /**< pointer to PFT data */
                  )               /** \return crop carbon (gC/m2) */
{
  const Pftcrop *crop;
  crop=pft->data;
  return phys_sum_crop(crop->ind)*pft->nind;
} /* of 'vegc_sum_crop' */

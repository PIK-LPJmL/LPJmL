/**************************************************************************************/
/**                                                                                \n**/
/**             l  e  a  f  c   _  c  r  o  p  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates leaf carbon of crop PFTs                               \n**/
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

Real leafc_crop(const Pft *pft /**< pointer to PFT data */
               )               /** \return leaf carbon (gC/m2) */
{
  const Pftcrop *crop;
  crop=pft->data;
  return crop->ind.leaf.carbon*pft->nind;
} /* of 'leafc_crop' */

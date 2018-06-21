/**************************************************************************************/
/**                                                                                \n**/
/**                     n  e  w  _  i  m  a  g  e  .  c                            \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with IMAGE                           \n**/
/**     Allocates and initializes memory for IMAGE data                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef IMAGE

Image_data *new_image(const Product *productpool)
{
  Image_data *image_data;
  image_data=new(Image_data);
  check(image_data);
  /* data received from IMAGE */
  image_data->fburnt=image_data->timber.fast=
  image_data->timber.slow=image_data->timber_frac=
  image_data->timber_f.fast=image_data->timber_f.slow=
  image_data->timber_turnover=image_data->deforest_emissions=
  image_data->store_bmtree_yield=image_data->biomass_yield_annual=0.0
;
  image_data->timber.fast=productpool->fast;
  image_data->timber.slow=productpool->slow;
  /* data sent to image */
  image_data->anpp=image_data->arh=0.0;
  return image_data;
} /* of 'new_image' */

#endif

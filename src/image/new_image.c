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

#if defined IMAGE && defined COUPLED

#define CN_ratio 7.0 /* C:N ratio of product pools */

Bool new_image(Cell *cell,const Product *productpool)
{
  Image_data *image_data;
  image_data=new(Image_data);
  if(image_data==NULL)
    return TRUE;
  cell->ml.image_data=image_data;
  /* data received from IMAGE */
  image_data->fburnt=image_data->timber_frac=
  image_data->timber_f.fast=image_data->timber_f.slow=
  image_data->timber_turnover=image_data->deforest_emissions=
  image_data->store_bmtree_yield=image_data->biomass_yield_annual=
  image_data->totwatcons=image_data->totwatdem=0.0
;
  cell->ml.product.fast.carbon=productpool->fast;
  cell->ml.product.slow.carbon=productpool->slow;
  cell->ml.product.fast.nitrogen=productpool->fast/CN_ratio;
  cell->ml.product.slow.nitrogen=productpool->slow/CN_ratio;
  /* data sent to image */
  return FALSE;
} /* of 'new_image' */

#endif

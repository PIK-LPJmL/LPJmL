/**************************************************************************************/
/**                                                                                \n**/
/**    p  r  o  d  u  c  t  _  t  u  r  n  o  v  e  r  .  c                        \n**/
/**                                                                                \n**/
/**     extension of LPJ to compute turnover of product pools                      \n**/
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

Stocks product_turnover(Image_data *data)
{
  Stocks stocks;
  data->prod_turn.fast.carbon=(data->timber.fast.carbon)*PRODUCT_FAST;
  data->prod_turn.fast.nitrogen=(data->timber.fast.nitrogen)*PRODUCT_FAST;
  data->prod_turn.slow.carbon=(data->timber.slow.carbon)*PRODUCT_SLOW;
  data->prod_turn.slow.nitrogen=(data->timber.slow.carbon)*PRODUCT_SLOW;
  
  data->timber.fast.carbon*=(1.0-PRODUCT_FAST);
  data->timber.slow.carbon*=(1.0-PRODUCT_SLOW);
  stocks.carbon=data->prod_turn.fast.carbon+data->prod_turn.slow.carbon;
  stocks.nitrogen=data->prod_turn.fast.nitrogen+data->prod_turn.slow.nitrogen;
  return stocks;
} /* of 'product_turnover' */

#else
Real product_turnover(Cell *cell)
{
  cell->output.prod_turnover.carbon+=cell->ml.product.fast.carbon*PRODUCT_FAST;
  cell->output.prod_turnover.carbon+=cell->ml.product.slow.carbon*PRODUCT_SLOW;
  cell->ml.product.fast.carbon*=(1.0-PRODUCT_FAST);
  cell->ml.product.slow.carbon*=(1.0-PRODUCT_SLOW);
  cell->output.prod_turnover.nitrogen+=cell->ml.product.fast.nitrogen*PRODUCT_FAST;
  cell->output.prod_turnover.nitrogen+=cell->ml.product.slow.nitrogen*PRODUCT_SLOW;
  cell->ml.product.fast.nitrogen*=(1.0-PRODUCT_FAST);
  cell->ml.product.slow.nitrogen*=(1.0-PRODUCT_SLOW);
}
#endif

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
  data->prod_turn.fast.carbon=(data->timber.fast.carbon)*param.product_turnover.fast;
  data->prod_turn.fast.nitrogen=(data->timber.fast.nitrogen)*param.product_turnover.fast;
  data->prod_turn.slow.carbon=(data->timber.slow.carbon)*param.product_turnover.slow;
  data->prod_turn.slow.nitrogen=(data->timber.slow.carbon)*param.product_turnover.slow;
  
  data->timber.fast.carbon*=(1.0-param.product_turnover.fast);
  data->timber.slow.carbon*=(1.0-param.product_turnover.slow);
  stocks.carbon=data->prod_turn.fast.carbon+data->prod_turn.slow.carbon;
  stocks.nitrogen=data->prod_turn.fast.nitrogen+data->prod_turn.slow.nitrogen;
  return stocks;
} /* of 'product_turnover' */

#else

void product_turnover(Cell *cell)
{
  cell->output.prod_turnover.carbon+=cell->ml.product.fast.carbon*param.product_turnover.fast;
  cell->output.prod_turnover.carbon+=cell->ml.product.slow.carbon*param.product_turnover.slow;
  cell->ml.product.fast.carbon*=(1.0-param.product_turnover.fast);
  cell->ml.product.slow.carbon*=(1.0-param.product_turnover.slow);
  cell->output.prod_turnover.nitrogen+=cell->ml.product.fast.nitrogen*param.product_turnover.fast;
  cell->output.prod_turnover.nitrogen+=cell->ml.product.slow.nitrogen*param.product_turnover.slow;
  cell->ml.product.fast.nitrogen*=(1.0-param.product_turnover.fast);
  cell->ml.product.slow.nitrogen*=(1.0-param.product_turnover.slow);
  cell->output.product_pool.fast.carbon=cell->ml.product.fast.carbon;
  cell->output.product_pool.slow.carbon=cell->ml.product.slow.carbon;
  cell->output.product_pool.fast.nitrogen=cell->ml.product.fast.nitrogen;
  cell->output.product_pool.slow.nitrogen=cell->ml.product.slow.nitrogen;
} /* of 'product_turnover' */

#endif

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

void product_turnover(Cell *cell)
{
#if 0
  cell->output.prod_turnover+=cell->ml.product.fast.carbon*param.product_turnover.fast;
  cell->balance.prod_turnover.fast.carbon+=cell->ml.product.fast.carbon*param.product_turnover.fast;
  cell->output.prod_turnover+=cell->ml.product.slow.carbon*param.product_turnover.slow;
  cell->balance.prod_turnover.slow.carbon+=cell->ml.product.slow.carbon*param.product_turnover.slow;
  cell->ml.product.fast.carbon*=(1.0-param.product_turnover.fast);
  cell->ml.product.slow.carbon*=(1.0-param.product_turnover.slow);
  cell->balance.prod_turnover.fast.nitrogen+=cell->ml.product.fast.nitrogen*param.product_turnover.fast;
  cell->balance.prod_turnover.slow.nitrogen+=cell->ml.product.slow.nitrogen*param.product_turnover.slow;
  cell->ml.product.fast.nitrogen*=(1.0-param.product_turnover.fast);
  cell->ml.product.slow.nitrogen*=(1.0-param.product_turnover.slow);
  cell->output.product_pool.fast=cell->ml.product.fast.carbon;
  cell->output.product_pool.slow=cell->ml.product.slow.carbon;
#endif
} /* of 'product_turnover' */

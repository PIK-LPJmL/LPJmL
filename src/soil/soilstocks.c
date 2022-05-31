/**************************************************************************************/
/**                                                                                \n**/
/**                s  o  i  l  s  t  o  c  k  s  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function computes total carbon/nitrogen in soil                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Stocks soilstocks(const Soil *soil)
{
  Stocks stocks;
  int l;
  stocks=litterstocks(&soil->litter);
  forrootsoillayer(l)
  {
    stocks.carbon+=soil->pool[l].slow.carbon+soil->pool[l].fast.carbon;
    stocks.nitrogen+=soil->pool[l].slow.nitrogen+soil->pool[l].fast.nitrogen
                     +soil->NO3[l]+soil->NH4[l];
  }
  stocks.carbon+=soil->YEDOMA;
  return stocks;
} /* of 'soilstocks' */

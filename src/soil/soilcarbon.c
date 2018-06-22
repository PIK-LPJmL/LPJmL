/**************************************************************************************/
/**                                                                                \n**/
/**                s  o  i  l  c  a  r  b  o  n  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function computes total carbon in soil                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real soilcarbon(const Soil *soil /**< pointer to soil data */
               )                 /** \return soil carbon (gC/m2) */ 
{
  Real carbon=0;
  int l;
  forrootsoillayer(l)
    carbon+=soil->cpool[l].slow+soil->cpool[l].fast;
  carbon+=soil->YEDOMA;
  return carbon+littersum(&soil->litter);
} /* of 'soilcarbon' */

Real soilcarbon_slow(const Soil *soil /**< pointer to soil data */
                    )                 /** \return slow carbon pool (gC/m2) */
{
  Real carbon=0;
  int l;
  forrootsoillayer(l)
     carbon+=soil->cpool[l].slow;
  return carbon;
} /* of 'soilcarbon_slow' */

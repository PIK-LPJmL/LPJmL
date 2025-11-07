/**************************************************************************************/
/**                                                                                \n**/
/**                  s  o  i  l  m  e  t  h  a  n  e  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function computes total methane in soil                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real soilmethane(const Soil *soil /**< pointer to soil */
                )                 /** \return methane sum (g/m2) */
{
  int l;
  Real methane = 0;
  forrootsoillayer(l)
    methane += soil->CH4[l];
  return methane;
} /* of 'soilmethane' */

/**************************************************************************************/
/**                                                                                \n**/
/**                s  o  i  l  w  a  t  e  r   .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function computes total water in soil                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real soilwater(const Soil *soil /**< pointer to soil data */
              )                 /** \return soil water (mm) */
{
  Real totw;
  int l;
  totw=soil->snowpack+soil->rw_buffer;
  foreachsoillayer(l)
    totw+=soil->w[l]*soil->par->whcs[l]+soil->ice_depth[l]+soil->w_fw[l]+soil->ice_fw[l];
  return totw;
} /* of 'soilwater' */

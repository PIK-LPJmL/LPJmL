/**************************************************************************************/
/**                                                                                \n**/
/**          g  e  t  o  u  t  p  u  t  t  y  p  e  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function determines output data type                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Type getoutputtype(int index /**< index of output */
                  )          /** \return type of output data */
{
  switch(index)
  {
    case SEASONALITY: case SDATE: case HDATE: case GRID: case COUNTRY:
    case REGION: case SDATE2: case HDATE2: case SYEAR: case SYEAR2:
      return LPJ_SHORT;
    default:
      return LPJ_FLOAT;
  }
} /* of 'getoutputtype' */

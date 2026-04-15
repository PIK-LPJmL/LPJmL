/**************************************************************************************/
/**                                                                                \n**/
/**               g  e  t  r  h  .  c                                              \n**/
/**                                                                                \n**/
/**     Function calculates relative humidity from specific humidity               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <math.h>
#include "types.h"
#include "date.h"
#include "hash.h"
#include "bstruct.h"
#include "numeric.h"
#include "units.h"

Real getrh(Real temp,    /**< air temperature (deg C) */
           Real humidity /**< specific humidity (kg/kg) */
          )              /** \return relative humidity (0..1) */
{
  Real temperature, rh;
  temperature = temp + 273.16;

  /*conversion of specific humidity to relative humidity*/
  rh= 0.263 * 1013.25 * humidity *1/(exp(17.67*temp/(temperature-29.65)));

  if (rh > 1)
    rh = 1;
  return rh;
} /* of 'getrh' */

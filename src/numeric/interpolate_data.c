/**************************************************************************************/
/**                                                                                \n**/
/**       i  n  t  e  r  p  o  l  a  t  e  _  d  a  t  a  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Linear interpolation function of arrays                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include "types.h"
#include "bstruct.h"
#include "date.h"
#include "numeric.h"

void interpolate_data(Real *res, Real *a, Real *b, int n, Real x)
{
  int i;
  for (i = 0; i<n; i++)
    res[i] = (1 - x)*a[i] + x*b[i];
} /* of 'interpolate_data' */

/**************************************************************************************/
/**                                                                                \n**/
/**                      l  i  n  r  e  g  .  c                                    \n**/
/**                                                                                \n**/
/**     Calculate coefficients of linear regression                                \n**/
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
#include "hash.h"
#include "bstruct.h"
#include "numeric.h"

void linreg(Real *a,        /**< intercept of line */
            Real *b,        /**< slope of line */
            const Real y[], /**< array */
            int n           /**< size of array */
           )
{
  Real sx,sy,sxx,sxy,ri,delta;
  int i;
  sx=(n*(n-1))/2; /* sum of i from 0 .. n-1 */
  sxx=(n*(n-1)*(2*n-1))/6; /* sum of i^2 from 0 .. n-1 */
  sy=sxy=ri=0;
  for(i=0;i<n;i++)
  {
    sy+=y[i];
    sxy+=y[i]*ri;
    ri++;
  }
  delta=1/(ri*sxx-sx*sx);
  *a=(sxx*sy-sx*sxy)*delta;
  *b=(ri*sxy-sx*sy)*delta;
} /* of 'linreg' */

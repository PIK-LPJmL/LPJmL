/***************************************************************************/
/**                                                                       **/
/**       i  n  t  e  r  p  o  l  a  t  e  _  d  a  t  a  .  c            **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     Linear interpolation function of arrays                           **/
/**                                                                       **/
/**     written by Werner von Bloh, Sibyll Schaphoff                      **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change: 21.10.2004                                           **/
/**                                                                       **/
/***************************************************************************/

#include <stdio.h>
#include "types.h"
#include "date.h"
#include "numeric.h"

void interpolate_data(Real *res, Real *a, Real *b, int n, Real x)
{
  int i;
  for (i = 0; i<n; i++)
    res[i] = (1 - x)*a[i] + x*b[i];
} /* of 'interpolate_data' */

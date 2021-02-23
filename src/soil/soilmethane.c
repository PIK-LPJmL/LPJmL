/***************************************************************************/
/**                                                                       **/
/**                s  t  a  n  d  m  e  t  h  a  n  e  .  c               **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     Function computes total carbon in stand                           **/
/**                                                                       **/
/**     written by Sibyll Schaphoff                                       **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change: 27.02.2009                                           **/
/**                                                                       **/
/***************************************************************************/

#include "lpj.h"

Real soilmethane(const Soil *soil /* pointer to stand */
)                 /* returns carbon sum (g/m2) */
{
  int l;
  Real methane = 0;
  forrootsoillayer(l)
    methane += soil->CH4[l];
  return methane;
} /* of 'standmethane' */

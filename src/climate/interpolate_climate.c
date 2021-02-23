/***************************************************************************/
/**                                                                       **/
/**    i  n  t  e  r  p  o  l  a  t  e  _  c  l  i  m  a  t  e  .  c      **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     Function interpolate_climate interpolates climate data            **/
/**                                                                       **/
/**     written by Werner von Bloh, Sibyll Schaphoff                      **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change: $Date:: 2019-01-24 15:59:54 +0100 (Thu, 24 Jan 201#$ **/
/**     By         : $Author:: bloh                            $          **/
/**                                                                       **/
/***************************************************************************/

#include "lpj.h"

void interpolate_climate(Climate *climate, int index, Real x)
{
  interpolate_data(climate->data[0].temp, climate->data[index + 1].temp, climate->data[(index + 1) % 2 + 1].temp, climate->file_temp.n, x);
#ifdef DEBUG
  printf("in=%g %g %g %g\n", climate->data[0].temp[0], climate->data[index + 1].temp[0], climate->data[(index + 1) % 2 + 1].temp[0], x);
#endif
  interpolate_data(climate->data[0].prec, climate->data[index + 1].prec, climate->data[(index + 1) % 2 + 1].prec, climate->file_prec.n, x);
  if (climate->data[0].wet != NULL)
    interpolate_data(climate->data[0].wet, climate->data[index + 1].wet, climate->data[(index + 1) % 2 + 1].wet, climate->file_wet.n, x);
  if (climate->data[0].sun != NULL)
    interpolate_data(climate->data[0].sun, climate->data[index + 1].sun, climate->data[(index + 1) % 2 + 1].sun, climate->file_cloud.n, x);
  if (climate->data[0].lwnet != NULL)
    interpolate_data(climate->data[0].lwnet, climate->data[index + 1].lwnet, climate->data[(index + 1) % 2 + 1].lwnet, climate->file_lwnet.n, x);
  if (climate->data[0].swdown != NULL)
    interpolate_data(climate->data[0].swdown, climate->data[index + 1].swdown, climate->data[(index + 1) % 2 + 1].swdown, climate->file_swdown.n, x);
} /* of 'interpolate_climate' */

/***************************************************************************/
/**                                                                       **/
/**    a  d  d  a  n  o  m  a  l  y  _  c  l  i  m  a  t  e  .  c         **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     Function adds anomalies to climate data                           **/
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

static void add_data(Real *res, const Real *a, int n)
{
  int i;
  for (i = 0; i<n; i++)
    res[i] += a[i];
} /* of 'add_data' */

void addanomaly_climate(Climate *climate, int index)
{
  int i;
#ifdef DEBUG
  printf("addanomalie %g+%g\n", climate->data[0].temp[0], climate->data[index].temp[1]);
  printf("addanomalie %g+%g\n", climate->data[0].prec[0], climate->data[index].prec[1]);
  printf("addanomalie %g+%g\n", climate->data[0].lwnet[0], climate->data[index].lwnet[1]);
  printf("addanomalie %g+%g\n", climate->data[0].swdown[0], climate->data[index].swdown[1]);
#endif
  add_data(climate->data[0].temp, climate->data[index].temp, climate->file_temp.n);
  add_data(climate->data[0].prec, climate->data[index].prec, climate->file_prec.n);
  add_data(climate->data[0].lwnet, climate->data[index].lwnet, climate->file_lwnet.n);
  add_data(climate->data[0].swdown, climate->data[index].swdown, climate->file_swdown.n);
  for (i = 0; i<climate->file_wet.n; i++)
    climate->data[0].wet[i] = climate->data[index].wet[i];
} /* of 'addanomaly_climate' */

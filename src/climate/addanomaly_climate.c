/**************************************************************************************/
/**                                                                                \n**/
/**    a  d  d  a  n  o  m  a  l  y  _  c  l  i  m  a  t  e  .  c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function adds anomalies to climate data                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

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

/**************************************************************************************/
/**                                                                                \n**/
/**    i  n  t  e  r  p  o  l  a  t  e  _  c  l  i  m  a  t  e  .  c               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function interpolate_climate interpolates climate data                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
void interpolate_climate(Climate *climate, int index, Real x)
{
  interpolate_data(climate->data[0].temp, climate->data[index + 1].temp, climate->data[(index + 1) % 2 + 1].temp, climate->file_temp.n, x);
#ifdef DEBUG
  printf("in=%g %g %g %g\n", climate->data[0].temp[0], climate->data[index + 1].temp[0], climate->data[(index + 1) % 2 + 1].temp[0], x);
  printf("in=%g %g %g %g\n", climate->data[0].wet[0], climate->data[index + 1].wet[0], climate->data[(index + 1) % 2 + 1].wet[0], x);
#endif
  interpolate_data(climate->data[0].prec, climate->data[index + 1].prec, climate->data[(index + 1) % 2 + 1].prec, climate->file_prec.n, x);
  if (climate->data[0].sun != NULL)
    interpolate_data(climate->data[0].sun, climate->data[index + 1].sun, climate->data[(index + 1) % 2 + 1].sun, climate->file_cloud.n, x);
  if (climate->data[0].lwnet != NULL)
    interpolate_data(climate->data[0].lwnet, climate->data[index + 1].lwnet, climate->data[(index + 1) % 2 + 1].lwnet, climate->file_lwnet.n, x);
  if (climate->data[0].swdown != NULL)
    interpolate_data(climate->data[0].swdown, climate->data[index + 1].swdown, climate->data[(index + 1) % 2 + 1].swdown, climate->file_swdown.n, x);
} /* of 'interpolate_climate' */

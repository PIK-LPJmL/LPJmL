/**************************************************************************************/
/**                                                                                \n**/
/**               f  r  e  a  d  h  y  d  r  o  t  o  p  e  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads hydrotope variables from restart file                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool freadhydrotope(FILE *file, Hydrotope *hydrotopes, Bool swap)
{

  freadreal(&hydrotopes->wtable_mean, 1, swap, file);
  freadreal(&hydrotopes->wtable_min, 1, swap, file);
  freadreal(&hydrotopes->wtable_max, 1, swap, file);

  freadreal(&hydrotopes->meanwater, 1, swap, file);
  freadreal(&hydrotopes->wetland_area, 1, swap, file);
  freadreal(&hydrotopes->wetland_area_runmean, 1, swap, file);
  freadreal(&hydrotopes->wetland_cti, 1, swap, file);
  freadreal(&hydrotopes->wetland_cti_runmean, 1, swap, file);

  freadreal(&hydrotopes->wetland_wtable_current, 1, swap, file);
  freadreal(&hydrotopes->wetland_wtable_max, 1, swap, file);
  if (freadreal(&hydrotopes->wetland_wtable_mean, 1, swap, file) != 1)
    return TRUE;
  return FALSE;
} /* of 'freadhydrotope' */


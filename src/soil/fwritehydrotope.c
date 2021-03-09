/**************************************************************************************/
/**                                                                                \n**/
/**               f  w  r  i  t  e  h  y  d  r  o  t  o  p  e  .  c                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function write hydrotope variables to restart file                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fwritehydrotope(FILE *file,const  Hydrotope *hydrotopes)
{
  fwrite(&hydrotopes->cti_mean, sizeof(Real), 1, file);
  fwrite(&hydrotopes->cti_chi, sizeof(Real), 1, file);
  fwrite(&hydrotopes->cti_phi, sizeof(Real), 1, file);
  fwrite(&hydrotopes->cti_mu, sizeof(Real), 1, file);

  fwrite(&hydrotopes->skip_cell, sizeof(Bool), 1, file);

  fwrite(&hydrotopes->wtable_mean, sizeof(Real), 1, file);
  fwrite(&hydrotopes->wtable_min, sizeof(Real), 1, file);
  fwrite(&hydrotopes->wtable_max, sizeof(Real), 1, file);

  fwrite(&hydrotopes->meanwater, sizeof(Real), 1, file);
  fwrite(&hydrotopes->wetland_area, sizeof(Real), 1, file);
  fwrite(&hydrotopes->wetland_area_runmean, sizeof(Real), 1, file);
  fwrite(&hydrotopes->wetland_cti, sizeof(Real), 1, file);
  fwrite(&hydrotopes->wetland_cti_runmean, sizeof(Real), 1, file);

  fwrite(&hydrotopes->wetland_wtable_current, sizeof(Real), 1, file);
  fwrite(&hydrotopes->wetland_wtable_max, sizeof(Real), 1, file);
  return  (fwrite(&hydrotopes->wetland_wtable_mean, sizeof(Real), 1, file) != 1);
} /* of 'fwritehydrotope' */


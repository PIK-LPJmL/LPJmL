/**************************************************************************************/
/**                                                                                \n**/
/**               f  p  r  i  n  t  h  y  d  r  o  t  o  p  e  .  c                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints hydrotope variables                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprinthydrotope(FILE *file,const  Hydrotope *hydrotopes)
{
  fprintf(file, "wtable_mean:\t%g\n", hydrotopes->wtable_mean);
  fprintf(file, "wtable_min:\t%g\n", hydrotopes->wtable_min);
  fprintf(file, "wtable_max:\t%g\n", hydrotopes->wtable_max);

  fprintf(file, "meanwater:\t%g\n", hydrotopes->meanwater);
  fprintf(file, "wetland_area:\t%g\n", hydrotopes->wetland_area);
  fprintf(file, "wetland_area_runmean:\t%g\n", hydrotopes->wetland_area_runmean);
  fprintf(file, "wetland_cti:\t%g\n", hydrotopes->wetland_cti);
  fprintf(file, "wetland_cti_runmean:\t%g\n", hydrotopes->wetland_cti_runmean);

  fprintf(file, "wetland wtable_current:\t%g\n", hydrotopes->wetland_wtable_current);
  fprintf(file, "wetland wtable_max:\t%g\n", hydrotopes->wetland_wtable_max);
  fprintf(file, "wetland wetland wtable_mean:\t%g\n", hydrotopes->wetland_wtable_mean);
} /* of 'fprinthydrotope' */

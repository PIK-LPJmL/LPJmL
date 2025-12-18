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

Bool fwritehydrotope(Bstruct file,
                     const char *name,
                     const  Hydrotope *hydrotopes)
{
  bstruct_writebeginstruct(file,name);
  bstruct_writereal(file,"wtable_mean",hydrotopes->wtable_mean);
  bstruct_writereal(file,"wtable_min",hydrotopes->wtable_min);
  bstruct_writereal(file,"wtable_max",hydrotopes->wtable_max);

  bstruct_writereal(file,"meanwater",hydrotopes->meanwater);
  bstruct_writereal(file,"wetland_area",hydrotopes->wetland_area);
  bstruct_writereal(file,"wetland_area_runmean",hydrotopes->wetland_area_runmean);
  bstruct_writereal(file,"wetland_cti",hydrotopes->wetland_cti);
  bstruct_writereal(file,"wetland_cti_mean",hydrotopes->wetland_cti_runmean);

  bstruct_writereal(file,"wetland_wtable_current",hydrotopes->wetland_wtable_current);
  bstruct_writereal(file,"wetland_wtable_max",hydrotopes->wetland_wtable_max);
  bstruct_writereal(file,"wetland_wtable_mean",hydrotopes->wetland_wtable_mean);
  return bstruct_writeendstruct(file);
} /* of 'fwritehydrotope' */


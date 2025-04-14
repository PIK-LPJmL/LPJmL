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

#define readreal(file,name,var) if(bstruct_readreal(file,name,var)) return TRUE

Bool freadhydrotope(Bstruct file,const char *name, Hydrotope *hydrotopes)
{
  if(bstruct_readstruct(file,name))
    return TRUE;
  readreal(file,"wtable_mean",&hydrotopes->wtable_mean);
  readreal(file,"wtable_min",&hydrotopes->wtable_min);
  readreal(file,"wtable_max",&hydrotopes->wtable_max);

  readreal(file,"meanwater",&hydrotopes->meanwater);
  readreal(file,"wetland_area",&hydrotopes->wetland_area);
  readreal(file,"wetland_area_runmean",&hydrotopes->wetland_area_runmean);
  readreal(file,"wetland_cti",&hydrotopes->wetland_cti);
  readreal(file,"wetland_cti_mean",&hydrotopes->wetland_cti_runmean);

  readreal(file,"wetland_wtable_current",&hydrotopes->wetland_wtable_current);
  readreal(file,"wetland_wtable_max",&hydrotopes->wetland_wtable_max);
  readreal(file,"wetland_wtable_mean",&hydrotopes->wetland_wtable_mean);
  return bstruct_readendstruct(file,name);
} /* of 'freadhydrotope' */


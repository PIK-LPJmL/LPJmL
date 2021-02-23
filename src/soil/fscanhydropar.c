/**************************************************************************************/
/**                                                                                \n**/
/**            f  s  c  a  n  h  y  d  r  o  p  a  r  .  c                         \n**/
/**                                                                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints LPJ configuration                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define fscanhydroreal(isout,file,var,fcn,name) \
  if(fscanreal(file,var,name,FALSE,isout)) return TRUE;

Hydropar hydropar;

Bool fscanhydropar(LPJfile *file,  /**< file  pointer */
                   Verbosity verb  /**< generate error output */
                  )                /**< returns TRUE on error */
{
  LPJfile f;
  if(fscanstruct(file,&f,"hydropar",verb))
    return TRUE;
  fscanhydroreal(verb,&f,&hydropar.wland_min,"fscanhydropar","wland_min");
  fscanhydroreal(verb,&f,&hydropar.cti_max,"fscanhydropar","cti_max");
  fscanhydroreal(verb,&f,&hydropar.lat_min,"fscanhydropar","lat_min");
  fscanhydroreal(verb,&f,&hydropar.wtab_thres,"fscanhydropar","wtab_thres");
  fscanhydroreal(verb,&f,&hydropar.cti_thres,"fscanhydropar","cti_thres");
  return FALSE;
} /* of 'fscanhydropar' */

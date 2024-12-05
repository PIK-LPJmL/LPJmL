/**************************************************************************************/
/**                                                                                \n**/
/**            f  s  c  a  n  h  y  d  r  o  p  a  r  .  c                         \n**/
/**                                                                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function read hydrologic parameter from config file                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define fscanhydroreal(isout,file,var,name) \
  if(fscanreal(file,var,name,FALSE,isout)) return TRUE;

Bool fscanhydropar(LPJfile *file,      /**< file pointer */
                   Hydropar *hydropar, /**< hydrologic parameter read */
                   Verbosity verb      /**< generate error output */
                  )                    /**< returns TRUE on error */
{
  LPJfile *f;
  f=fscanstruct(file,"hydropar",verb);
  if(f==NULL)
    return TRUE;
  fscanhydroreal(verb,f,&hydropar->wland_min,"wland_min");
  fscanhydroreal(verb,f,&hydropar->cti_max,"cti_max");
  fscanhydroreal(verb,f,&hydropar->wtab_thres,"wtab_thres");
  fscanhydroreal(verb,f,&hydropar->cti_thres,"cti_thres");
  return FALSE;
} /* of 'fscanhydropar' */

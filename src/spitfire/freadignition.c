/**************************************************************************************/
/**                                                                                \n**/
/**            f  r  e  a  d  i  g  n  i  t  i  o  n  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function read ignition data from binary file                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool freadignition(FILE *file,const char *name,Ignition *ignition,Bool swap)
{
  if(readstruct(file,name,swap))
    return TRUE;
  if(readreal(file,"nesterov_accum",&ignition->nesterov_accum,swap))
    return TRUE;
  if(readreal(file,"nesterov_max",&ignition->nesterov_max,swap))
    return TRUE;
  if(readint(file,"nesterov_day",&ignition->nesterov_day,swap))
    return TRUE;
  return readendstruct(file);
} /* of 'freadignition' */

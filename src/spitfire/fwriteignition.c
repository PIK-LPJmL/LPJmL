/**************************************************************************************/
/**                                                                                \n**/
/**            f  w  r  i  t  e  i  g  n  i  t  i  o  n  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes ignition data to binary file                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fwriteignition(Bstruct file,const char *name,const Ignition *ignition)
{
  bstruct_writestruct(file,name);
  bstruct_writereal(file,"nesterov_accum",ignition->nesterov_accum);
  bstruct_writereal(file,"nesterov_max",ignition->nesterov_max);
  bstruct_writeint(file,"nesterov_day",ignition->nesterov_day);
  return bstruct_writeendstruct(file);
} /* of 'fwriteignition' */

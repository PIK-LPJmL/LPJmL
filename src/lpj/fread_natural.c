/**************************************************************************************/
/**                                                                                \n**/
/**           f  r  e  a  d  _  n  a  t  u  r  a  l  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "natural.h"

Bool fread_natural(FILE * UNUSED(file),Stand *stand,
                   Bool UNUSED(swap))
{
  stand->landcover=NULL;
  return FALSE;
} /* of 'fread_natural' */

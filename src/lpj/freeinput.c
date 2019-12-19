/**************************************************************************************/
/**                                                                                \n**/
/**                   f  r  e  e  i  n  p  u  t  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function deallocates input data                                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void freeinput(Input input, /**< Input struct */
               Bool isroot  /**< task is root task (TRUE/FALSE) */
              )
{
  freeclimate(input.climate,isroot);
  freewateruse(input.wateruse,isroot);
  freelanduse(input.landuse,isroot);
  freepopdens(input.popdens,isroot);
  freelandcover(input.landcover,isroot);
} /* of 'freeinput' */

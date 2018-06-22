/**************************************************************************************/
/**                                                                                \n**/
/**                   f  r  e  e  s  o  i  l  .  c                                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function deallocates memory for soil                                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void freesoil(Soil *soil /**< pointer to soil data */
             )
{
  int l;
  freelitter(&soil->litter);
  for (l=0;l<LASTLAYER;l++)
  {
    free(soil->c_shift_fast[l]);
    free(soil->c_shift_slow[l]);
  }
} /* of 'freesoil' */

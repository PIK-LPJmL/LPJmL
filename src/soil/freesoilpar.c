/**************************************************************************************/
/**                                                                                \n**/
/**               f  r  e  e  s  o  i  l  p  a  r  .  c                            \n**/
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

void freesoilpar(Soilpar soilpar[], /**< soil parameter array */
                 int nsoil          /**< size of soil parameter array */
                )
{
  int i;
  for(i=0;i<nsoil;i++)
    free(soilpar[i].name);
  free(soilpar);
} /* of 'freesoilpar' */

/**************************************************************************************/
/**                                                                                \n**/
/**                     f  r  e  e  m  a  t  .  c                                  \n**/
/**                                                                                \n**/
/**     Function deallocates 2-dimensional array allocated by newmat()             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "types.h"

void freemat(void **mat)
{
  if(mat!=NULL)
  {
    free(mat[0]);
    free(mat);
  }
} /* of 'freemat' */

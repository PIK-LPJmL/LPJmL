/**************************************************************************************/
/**                                                                                \n**/
/**                 f  r  e  e  c  o  o  r  d  a  r  r  a  y  .  c                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function deallocates index array                                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void freecoordarray(Coord_array *array)
{
  if(array!=NULL)
  {
    free(array->index);
    free(array);
  }
} /* of 'freecoordarray' */

/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  _  f  r  e  e  d  a  t  a  .  c         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions for reading/writing JSON-like objects from binary file           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "bstruct_intern.h"

void bstruct_freedata(Bstruct_data *data)
{
  switch(data->token)
  {
    case BSTRUCT_STRING: case BSTRUCT_STRING1:
      free(data->data.string);
      break;
    case BSTRUCT_INDEXARRAY:
      free(data->data.index);
      break;
    default:
      break;
  }
} /* of 'bstruct_freedata' */

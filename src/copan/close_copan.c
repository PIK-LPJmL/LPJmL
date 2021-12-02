/**************************************************************************************/
/**                                                                                \n**/
/**                  c  l  o  s  e  _  c  o  p  a  n  .  c                         \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with COPAN                           \n**/
/**     Closes connection to COPAN model                                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void close_copan(const Config *config /**< LPJmL configuration */
                )
{
  if(isroot(*config))
  {
    if(config->socket!=NULL)
      close_socket(config->socket);
  }
} /* of 'close_copan' */

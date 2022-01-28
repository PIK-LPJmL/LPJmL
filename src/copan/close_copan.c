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

void close_copan(Bool iserror,        /**< closing on error? */
                 const Config *config /**< LPJmL configuration */
                )
{
  int token;
  if(isroot(*config))
  {
    if(config->socket!=NULL)
    {
      token=(iserror) ? FAIL_DATA : END_DATA;
      writeint_socket(config->socket,&token,1);
      close_socket(config->socket);
    }
  }
} /* of 'close_copan' */

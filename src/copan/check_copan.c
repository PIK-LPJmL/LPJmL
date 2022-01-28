/**************************************************************************************/
/**                                                                                \n**/
/**                  c  h  e  c  k  _  c  o  p  a  n  .  c                         \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with COPAN                           \n**/
/**     Check status of COPAN model                                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int check_copan(Config *config /**< LPJmL configuration */
               )               /** \return error code from COPAN */
{
  int token=GET_STATUS;
  int status;
  if(isroot(*config))
  {
    writeint_socket(config->socket,&token,1);
    readint_socket(config->socket,&status,1);
    if(status)
    {
      close_socket(config->socket);
      config->socket=NULL;
    }
    return status;
  }
  return COPAN_OK;
} /* of 'check_copan' */

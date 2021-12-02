/**************************************************************************************/
/**                                                                                \n**/
/**                  o  p  e  n  _  c  o  p  a  n  .  c                            \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with COPAN                           \n**/
/**     Opens connection to COPAN model using TDT compliant socket                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <signal.h>
#include "lpj.h"

static void handler(int UNUSED(num))
{
  fail(SOCKET_ERR,FALSE,"Output channel is broken");
} /* of 'handler' */

Bool open_copan(Config *config /**< LPJmL configuration */
               )               /** \return TRUE on error */
{
  int version=1;
  if(isroot(*config))
  {

    /* Establish the TDT connection */
    printf("Connecting to COPAN model...\n");
    fflush(stdout);
    config->socket=connecttdt_socket(config->copan_host,config->copan_port);
    if(config->socket==NULL)
    {
      close_socket(config->socket);
      config->socket=NULL;
      return TRUE;
    }
#ifndef _WIN32
    signal(SIGPIPE,handler);
#endif
    writeint_socket(config->socket,&version,1);
    writeint_socket(config->socket,&config->total,1);
  }
  return FALSE;
} /* of 'open_copan' */

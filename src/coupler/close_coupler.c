/**************************************************************************************/
/**                                                                                \n**/
/**                  c  l  o  s  e  _  c  o  u  p  l  e  r  .  c                   \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online                                      \n**/
/**     Closes connection to coupled model                                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifndef _WIN32
#include <signal.h>
#endif

void close_coupler(Bool errorcode,      /**< error code (0= no error) */
                   const Config *config /**< LPJmL configuration */
                  )
{
  if(isroot(*config))
  {
    if(config->socket!=NULL) /* already closed? */
    {
      send_token_coupler((errorcode) ? FAIL_DATA : END_DATA,errorcode,config);
      close_socket(config->socket);
#ifndef _WIN32
      signal(SIGPIPE,SIG_DFL);
#endif
    }
  }
} /* of 'close_coupler' */

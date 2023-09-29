/**************************************************************************************/
/**                                                                                \n**/
/**                  o  p  e  n  _  c  o  u  p  l  e  r  .  c                      \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online                                      \n**/
/**     Opens connection to coupled model using TDT compliant socket               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <signal.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include "lpj.h"


static char *coupled_model;

static void handler(int UNUSED(num))
{
  fail(SOCKET_ERR,TRUE,FALSE,"Channel to %s is broken",coupled_model);
} /* of 'handler' */

static void alarmhandler(int UNUSED(num))
{
  fail(OPEN_COUPLER_ERR,TRUE,FALSE,"Timeout in connection to %s",coupled_model);
} /* of 'alarmhandler' */

Bool open_coupler(Config *config /**< LPJmL configuration */
                 )               /** \return TRUE on error */
{
  int version=COUPLER_VERSION;
  Type type=LPJ_INT;
  if(isroot(*config))
  {

    /* Establish the TDT connection */
    printf("Connecting to %s model...\n",config->coupled_model);
    coupled_model=config->coupled_model;
    fflush(stdout);
    if(config->wait)
    {
#ifndef _WIN32
      /* set alarm timer */
      signal(SIGALRM,alarmhandler);
      alarm(config->wait);
#endif
    }
    config->socket=connecttdt_socket(config->coupled_host,config->coupler_port);
#ifndef _WIN32
    if(config->wait)
    {
      /* disable alarm handler */
      alarm(0);
      signal(SIGALRM,SIG_DFL);
    }
#endif
    if(config->socket==NULL)
      return TRUE;
#ifndef _WIN32
    signal(SIGPIPE,handler);
#endif
    /* send coupler version */
    writeint_socket(config->socket,&version,1);
    if(version>3)
    {
      /* receive version from coupled model */
      readint_socket(config->socket,&version,1);
      if(version!=COUPLER_VERSION)
      {
        fprintf(stderr,"ERROR312: Invalid coupler version %d received from %s, must be %d.\n",
                version,config->coupled_model,COUPLER_VERSION);
        return TRUE;
      }
      /* send 5 integer values */
      send_token_coupler(PUT_INIT_DATA,5,config);
      writeint_socket(config->socket,&type,1);
      /* send first index of cell */
      writeint_socket(config->socket,&config->firstgrid,1);
      /* send total number of cells */
      writeint_socket(config->socket,&config->nall,1);
    }
    /* send number of cells with valid soil code */
    writeint_socket(config->socket,&config->total,1);
    /* send number of input and output streams */
    writeint_socket(config->socket,&config->coupler_in,1);
    writeint_socket(config->socket,&config->coupler_out,1);
  }
  return FALSE;
} /* of 'open_coupler' */

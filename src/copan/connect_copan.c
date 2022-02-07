/**************************************************************************************/
/**                                                                                \n**/
/**               c  o  n  n  e  c  t   _  c  o  p  a  n  .  c                     \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with COPAN                           \n**/
/**     Connects COPAN model to LPJML                                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Socket *connect_copan(int port, /**< socket port */
                      int wait  /**< time to wait (sec) */
                     )          /** \return pointer to open socket or NULL */
{
  Socket *socket;
  int version,my_version=COPAN_COUPLER_VERSION;
  socket=opentdt_socket(port,wait);
  if(socket==NULL)
  {
    fprintf(stderr,"Error opening communication channel at port %d.\n",
            port);
    return NULL;
  }
  /* Get protocol version */
  readint_socket(socket,&version,1);
  if(version>3)
    writeint_socket(socket,&my_version,1);
  if(version!=COPAN_COUPLER_VERSION)
  {
    fprintf(stderr,"Unsupported coupler version %d, must be %d.\n",
            version,COPAN_COUPLER_VERSION);
    close_socket(socket);
    return NULL;
  }
  return socket;
} /* of 'connect_copan' */

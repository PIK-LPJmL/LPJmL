/**************************************************************************************/
/**                                                                                \n**/
/**                  c  l  o  s  e  _  s  o  c  k  e  t  .  c                      \n**/
/**                                                                                \n**/
/**     Function closes socket                                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include "types.h"
#include "channel.h"

void close_socket(Socket *socket)
{
#ifdef _WIN32
  closesocket(socket->channel);
  WSACleanup();
#else
  close(socket->channel);
#endif
  free(socket);
} /* of 'close_socket' */

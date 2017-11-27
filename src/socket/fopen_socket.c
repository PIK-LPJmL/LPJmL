/**************************************************************************************/
/**                                                                                \n**/
/**               f  o  p  e  n  _  s  o  c  k  e  t  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "channel.h"

void open_socket_(Socket *socket,
                  int *port, /* port of TCP/IP connection */
                  int *wait,
                  int *err
                 )
{
  Socket *s;
  s=open_socket(*port,*wait);
  if(s==NULL)
    *err=1;
  else
  {
    *socket=*s;
    free(s);
    *err=0;
  }
} /* of 'open_socket' */

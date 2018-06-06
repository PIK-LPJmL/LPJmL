/**************************************************************************************/
/**                                                                                \n**/
/**     f  r  e  a  d  l  o  n  g  _  s  o  c  k  e  t  .  c                       \n**/
/**                                                                                \n**/
/**     FORTARN interface to readlong_socket()                                     \n**/
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
#include "types.h"
#include "channel.h"

void readlong_socket_(Socket *socket,long long *data,int *n,int *err)
{
  *err=readlong_socket(socket,data,*n);
} /* of 'readlong_socket' */

/**************************************************************************************/
/**                                                                                \n**/
/**     f  r  e  a  d  i  n  t  _  s  o  c  k  e  t  .  c                          \n**/
/**                                                                                \n**/
/**     FORTARN interface to readint_socket()                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "channel.h"

void readint_socket_(Socket *socket,int *data,int *n,int *err)
{
  *err=readint_socket(socket,data,*n);
} /* of 'readint_socket' */

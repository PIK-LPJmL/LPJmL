/**************************************************************************************/
/**                                                                                \n**/
/**           f  w  r  i  t  e  l  o  n  g  _  s  o  c  k  e  t  .  c              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
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

void writelong_socket_(Socket *socket,long long *buffer,int *n,int *err)
{
  *err=writelong_socket(socket,buffer,*n);
} /* of 'writelong_socket' */

/**************************************************************************************/
/**                                                                                \n**/
/**       f  w  r  i  t  e  s  h  o  r  t  _  s  o  c  k  e  t  .  c               \n**/
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

#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "channel.h"

void writeshort_socket_(Socket *socket,short *buffer,int *n,int *err)
{
  *err=writeshort_socket(socket,buffer,*n);
} /* of 'writeshort_socket' */

/**************************************************************************************/
/**                                                                                \n**/
/**             b  i  g  e  n  d  i  a  n  .  c                                    \n**/
/**                                                                                \n**/
/**     Determine whether byte order is big or little endian                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include "types.h"
#include "swap.h"

typedef struct
{
  char hi,lo;
} Int16;

static Bool checkendian(Int16 *data)
{
  return data->hi==0;
} /* of 'checkendian' */

Bool bigendian(void)  /** \return TRUE if byte order is big-endian */
{
  short data=0xff;
  return checkendian((Int16 *)&data);
} /* of 'bigendian' */

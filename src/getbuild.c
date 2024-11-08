/**************************************************************************************/
/**                                                                                \n**/
/**                     g  e  t  b  u  i  l  d  .  c                               \n**/
/**                                                                                \n**/
/**     Get build number, build date and SVN revision number                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "types.h"

char *getbuilddate(void)
{
  static char *s=__DATE__;
  return s;
} /* of 'getbuilddate' */

char *getrepo(void)
{
  static char *s=(strlen(GIT_REPO)==0) ?  "https://github.com/PIK-LPJmL/LPJmL" : GIT_REPO;
  return s;
} /* of 'getrepo' */

char *gethash(void)
{
  static char *s=GIT_HASH;
  return s;
} /* of 'gethash' */

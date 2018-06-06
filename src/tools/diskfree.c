/**************************************************************************************/
/**                                                                                \n**/
/**                   d  i  s  k  f  r  e  e  .  c                                 \n**/
/**                                                                                \n**/
/**     Function gets free space on file system in bytes                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#ifdef _WIN32
#include "windows.h"
#else
#include <sys/statvfs.h>
#endif
#include "types.h"

long long diskfree(const char *disk /**< path to disk */
                  )                 /** \return number of free bytes */
{
#ifdef _WIN32
  unsigned long long freebytes;
  if(GetDiskFreeSpaceEx(disk,(PULARGE_INTEGER)&freebytes,NULL,NULL))
    return (long long)freebytes;
  else
    return -1;
#else
  struct statvfs fiData;
  if((statvfs(disk,&fiData))<0)
    return -1;
  else
    return fiData.f_bsize*fiData.f_bfree;
#endif
} /* of 'diskfree' */

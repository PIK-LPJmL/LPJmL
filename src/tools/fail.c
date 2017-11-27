/**************************************************************************************/
/**                                                                                \n**/
/**                      f  a  i  l  .  c                                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes out error message and terminates program                   \n**/
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
#include <stdarg.h>
#include <string.h>
#include "types.h"

void fail(int errcode,     /**< Error code (0...999) */
          Bool core,       /**< generate core file (TRUE/FALSE) */
          const char *msg, /**< error format string */
          ...              /**< optional parameter for output */
         )
{
  char *s;
  va_list ap;
  /*
   * Output is put in one printf statement. This has to be done because output
   * in multiple printf's is mixed up in the parallel version using MPI
   */
  s=alloca(strlen(msg)+strlen("ERROR000: ")+strlen(", program terminated unsuccessfully.\n")+1);
  if(errcode>999)
    errcode=999;
  if(errcode<0)
    errcode=1;
  sprintf(s,"ERROR%03d: ",errcode);
  strcat(s,msg);
  strcat(s,", program terminated unsuccessfully.\n");
  va_start(ap,msg);
  vfprintf(stderr,s,ap);
  fflush(stderr);
  va_end(ap);
  if(core)
    abort(); /* generate core file for post-mortem analysis */
  exit(errcode);
} /* of 'fail' */

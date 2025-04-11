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
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "types.h"

static void printfailerr2(int errcode,Bool stop,const char *msg,va_list ap)
{
  char *s;
  int len;
  if(stop)
    len=strlen(msg)+strlen("ERROR000: ")+strlen(", program terminated unsuccessfully.\n")+1;
  else
    len=strlen(msg)+strlen("ERROR000: ")+strlen(".\n")+1;
  /*
   * Output is put in one printf statement. This has to be done because output
   * in multiple printf's is mixed up in the parallel version using MPI
   */
  s=alloca(len);
  if(errcode>999)
    errcode=999;
  if(errcode<0)
    errcode=1;
  snprintf(s,len,"ERROR%03d: ",errcode);
  strcat(s,msg);
  strcat(s,(stop) ? ", program terminated unsuccessfully.\n" : ".\n");
  vfprintf(stderr,s,ap);
  fflush(stderr);
}  /* of 'printfailerr2' */

void printfailerr(int errcode,     /**< Error code (0...999) */
                  Bool stop,       /**< terminate program */
                  const char *msg, /**< error format string */
                  ...              /**< optional parameter for output */
                 )
{
  va_list ap;
  va_start(ap,msg);
  printfailerr2(errcode,stop,msg,ap);
  va_end(ap);
}  /* of 'printfailerr' */

void fail(int errcode,     /**< Error code (0...999) */
          Bool stop,       /**< terminate program */
          Bool core,       /**< generate core file (TRUE/FALSE) */
          const char *msg, /**< error format string */
          ...              /**< optional parameter for output */
         )
{
  va_list ap;
  va_start(ap,msg);
  printfailerr2(errcode,stop,msg,ap);
  va_end(ap);
  if(stop && core)
    abort(); /* generate core file for post-mortem analysis */
  if(stop)
    exit(errcode);
} /* of 'fail' */

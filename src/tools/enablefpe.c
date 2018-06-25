/**************************************************************************************/
/**                                                                                \n**/
/**                        e  n  a  b  l  e  f  p  e  .  c                         \n**/
/**                                                                                \n**/
/**     Function enables floating point exceptions. In case of an error            \n**/
/**     core file is generated. -DWITH_FPE flag must be set in                     \n**/
/**     Makefile.inc.                                                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifdef WITH_FPE
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "types.h"
#include "errmsg.h"

static void fpehandler(int UNUSED(sig_num))
{
  fail(FPE_ERR,TRUE,"Floating point exception occurred");
} /* of 'fpehandler' */

#ifdef _WIN32
#include <float.h>
#else
#include <fenv.h>
#ifdef  __xlc__
#include <fptrap.h>
#endif
#endif
void enablefpe(void)
{
#ifdef _WIN32
  _control87(EM_UNDERFLOW|EM_INEXACT,MCW_EM); /* Enable FPEs for Windows */
#else
#ifdef __xlc__        /* for IBM AIX and xlc compiler */
  fenv_t fenv;
  fegetenv(&fenv);
  fenv.trapstate=1;
  fesetenv(&fenv);
  fp_enable(TRP_INVALID|TRP_OVERFLOW|TRP_DIV_BY_ZERO);
#else
  /* for GNU C and Intel C compiler */
  feenableexcept(FE_DIVBYZERO|FE_OVERFLOW|FE_INVALID);
#endif
#endif
  signal(SIGFPE,fpehandler);
}
#endif

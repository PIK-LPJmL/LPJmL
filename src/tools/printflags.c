/**************************************************************************************/
/**                                                                                \n**/
/**                 p  r  i  n  t  f  l  a  g  s  .  c                             \n**/
/**                                                                                \n**/
/**     Function  prints information about OS, compiler used and compile           \n**/
/**     flags set. Intel C, GNU C, IBM XL C, and Microsoft Compiler are            \n**/
/**     recognized.                                                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define xstr(s) mkstr(s)
#define mkstr(s) #s

void printflags(const char *progname /**< program name */
               )
{
  printf("%s C Version " LPJ_VERSION " (%s)\n",progname,
               getbuilddate());
  printf("Operating system: %s, %s endian\n",sysname(),
         (bigendian()) ? "big" : "little");
  printf("Compiler:         ");
#if defined(__INTEL_COMPILER)
  printf("Intel C version %.2f",__INTEL_COMPILER*0.01);
#elif defined( __GNUC__)
  printf("GNU C version %d.%d",__GNUC__,__GNUC_MINOR__);
#elif defined( __xlc__)
  printf("IBM XL C");
#elif defined(_MSC_VER)
  printf("Microsoft Visual C++ version %.2f",_MSC_VER*0.01);
#else
  printf("Unknown");
#endif
  printf(", %d bit\n",(int)sizeof(void *)*8);
  printf("Compile flags:    %s\n",xstr(CFLAGS));
} /* of 'printflags' */

/**************************************************************************************/
/**                                                                                \n**/
/**                      c  o  p  y  r  i  g  h  t  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints copyright notice                                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void copyright(const char *progname /**< program name */
              )
{
  String os,title;
  const char *t[17];
  /* large letters created by figlet */

  t[0]=" _     ____     _           _       _  _          ___  ";
  t[1]="| |   |  _ \\   | |_ __ ___ | |     | || |        / _ \\ ";
  t[2]="| |   | |_) |  | | '_ ` _ \\| |     | || |_      | | | |";
  t[3]="| |___|  __/ |_| | | | | | | |___  |__   _|  _  | |_| |";
  t[4]="|_____|_|   \\___/|_| |_| |_|_____|    |_|   (_)  \\___/ ";
  t[5]="";
  snprintf(title,78-10,"%s C Version " LPJ_VERSION " (%s)",progname,
           getbuilddate());
  t[6]=title;
#ifdef USE_MPI
  snprintf(os,78-10,"Compiled for %s with MPI"
#ifdef IMAGE
           " and IMAGE coupler"
#endif
           ,sysname());
#else
  snprintf(os,78-10,"Compiled for %s"
#ifdef IMAGE
           " with IMAGE coupler"
#endif
           ,sysname());
#endif
  t[7]="Dynamic global vegetation model with natural and managed land";
  t[8]=os;
  t[9]="(C) Potsdam Institute for Climate Impact Research (PIK),";
  t[10]="see COPYRIGHT file";
  t[11]="Authors, and contributors see AUTHORS file";
  t[12]="This version of LPJmL is licensed under GNU AGPL Version 3 or later";
  t[13]="See LICENSE file or go to http://www.gnu.org/licenses/";
  t[14]="or invoke lpjml -l to print license";
  t[15]="Contact: https://github.com/PIK-LPJmL/LPJmL /lpjml";
  t[16]="";
  banner(t,17,78);
} /* of 'copyright' */

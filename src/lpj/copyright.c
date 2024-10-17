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
  String os,title,hash;
  const char *t[18];
  /* large letters created by figlet */
  t[0]=" _     ____     _           _       ____         ___  ";
  t[1]="| |   |  _ \\   | |_ __ ___ | |     | ___|       / _ \\";
  t[2]="| |   | |_) |  | | '_ ` _ \\| |     |___ \\      | (_) |";
  t[3]="| |___|  __/ |_| | | | | | | |___   ___) |  _   \\__, |";
  t[4]="|_____|_|   \\___/|_| |_| |_|_____| |____/  (_)    /_/";
  t[5]="";
  snprintf(title,78-10,"%s C Version %s (%s)",progname,
           getversion(),getbuilddate());
  t[6]=title;
#ifdef USE_MPI
  snprintf(os,78-10,"Compiled for %s with MPI"
#if defined IMAGE && defined COUPLED
           " and IMAGE coupler"
#endif
           ,sysname());
#else
  snprintf(os,78-10,"Compiled for %s"
#if defined IMAGE && defined COUPLED
           " with IMAGE coupler"
#endif
           ,sysname());
#endif
  t[7]="Dynamic global vegetation model with natural and managed land";
  t[8]=os;
  snprintf(hash,78-10,"GIT hash: %s",gethash());
  t[9]=hash;
  t[10]="(C) Potsdam Institute for Climate Impact Research (PIK),";
  t[11]="see COPYRIGHT file";
  t[12]="Authors, and contributors see AUTHORS file";
  t[13]="This version of LPJmL is licensed under GNU AGPL Version 3 or later";
  t[14]="See LICENSE file or go to http://www.gnu.org/licenses/";
  t[15]="or invoke lpjml -l to print license";
  t[16]="Contact: https://github.com/PIK-LPJmL/LPJmL";
  t[17]="";
  banner(t,18,78);
} /* of 'copyright' */

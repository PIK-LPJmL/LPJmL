/**************************************************************************************/
/**                                                                                \n**/
/**           f  p  r  i  n  t  s  o  i  l  p  a  r  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints soil parameter                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprintsoilpar(FILE *file,              /**< pointer to ASCII file */
                   const Soilpar soilpar[], /**< soil parameter array */
                   int nsoil                /**< number of soil types */
                  )
{
  int i;
  fprintf(file,"soildepth:\t");
  for(i=0;i<NSOILLAYER;i++)
    fprintf(file," %g",soildepth[i]);
  fprintf(file," (mm)\nfbd factors:\t");
  for(i=0;i<NFUELCLASS;i++)
    fprintf(file," %g",fbd_fac[i]);
  fprintf(file,"\nName               w_pwp  w_fc  w_sat hsg tdiff0 tdiff15 tdiff100 condpwp cond100 cond100ice\n"
               "------------------ ------ ----- ----- --- ------ ------- -------- ------- ------- ----------\n");
  for(i=0;i<nsoil;i++)
    fprintf(file,"%18s %6.2f %5.2f %5.2f %3d %6.3f %7.3f %8.3f %7.3f %7.3f %10.3f\n",
            soilpar[i].name,soilpar[i].wpwp,soilpar[i].wfc,soilpar[i].wsat,
            soilpar[i].hsg+1,
            soilpar[i].tdiff_0,soilpar[i].tdiff_15,soilpar[i].tdiff_100,
            soilpar[i].tcond_pwp,soilpar[i].tcond_100,soilpar[i].tcond_100_ice);
  fprintf(file,"------------------ ------ ----- ----- --- ------ ------- -------- ------- ------- ----------\n");
} /* of 'fprintsoilpar' */

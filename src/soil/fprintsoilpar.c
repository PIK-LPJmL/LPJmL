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
                   int nsoil,               /**< number of soil types */
                   int with_nitrogen        /**< with nitrogen cycle */
                  )
{
  int i;
  fputs("soildepth:\t",file);
  for(i=0;i<NSOILLAYER;i++)
    fprintf(file," %g",soildepth[i]);
  fputs(" (mm)\nfbd factors:\t",file);
  for(i=0;i<NFUELCLASS;i++)
    fprintf(file," %g",fbd_fac[i]);
  fputs("\nName               %sand  %silt %clay hsg tdiff0 tdiff15 tdiff100 condpwp cond100 cond100ice",file);
  if(with_nitrogen)
    fputs(" a_nit   b_nit   c_nit   d_nit   anion_excl cn_ratio",file);
  fputs("\n------------------ ------ ----- ----- --- ------ ------- -------- ------- ------- ----------",file);
  if(with_nitrogen)
    fputs(" ------- ------- ------- ------- ---------- --------\n",file);
  else
    fputc('\n',file);
  for(i=0;i<nsoil;i++)
  {
    fprintf(file,"%18s %6.2f %5.2f %5.2f %3d %6.3f %7.3f %8.3f %7.3f %7.3f %10.3f",
            soilpar[i].name,soilpar[i].sand,soilpar[i].silt,soilpar[i].clay,
            soilpar[i].hsg+1,
            soilpar[i].tdiff_0,soilpar[i].tdiff_15,soilpar[i].tdiff_100,
            soilpar[i].tcond_pwp,soilpar[i].tcond_100,soilpar[i].tcond_100_ice);
    if(with_nitrogen)
      fprintf(file," %7.3f %7.3f %7.4f %7.3f %10.3f %8.1f",
              soilpar[i].a_nit,soilpar[i].b_nit,soilpar[i].c_nit,soilpar[i].d_nit,soilpar[i].anion_excl,soilpar[i].cn_ratio);
    fputc('\n',file);
  }
  fputs("------------------ ------ ----- ----- --- ------ ------- -------- ------- ------- ----------",file);
  if(with_nitrogen)
    fputs(" ------- ------- ------- ------- ---------- --------",file);
  fputc('\n',file);
} /* of 'fprintsoilpar' */

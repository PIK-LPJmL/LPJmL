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

void fprintsoilpar(FILE *file,          /**< pointer to text file */
                   const Config *config /**< LPJmL configuration */
                  )
{
  int i;
  fputs("soildepth:\t",file);
  for(i=0;i<NSOILLAYER;i++)
    fprintf(file," %g",soildepth[i]);
  fputs(" (mm)\nfbd factors:\t",file);
  for(i=0;i<NFUELCLASS;i++)
    fprintf(file," %g",fbd_fac[i]);
  fputs("\nName               %sand  %silt %clay ",file);
  if(config->soilpar_option==PRESCRIBED_SOILPAR)
    fputs("Ks     w_pwp  w_fc  w_sat ",file);
  fputs("hsg tdiff0 tdiff15 tdiff100 condpwp cond100 cond100ice",file);
  if(config->with_nitrogen)
    fputs(" a_nit   b_nit   c_nit   d_nit   anion_excl cn_ratio",file);
  fputs("\n------------------ ------ ----- -----",file);
  if(config->soilpar_option==PRESCRIBED_SOILPAR)
    fputs(" ------ ------ ----- -----",file);
  fputs(" --- ------ ------- -------- ------- ------- ----------",file);
  if(config->with_nitrogen)
    fputs(" ------- ------- ------- ------- ---------- --------\n",file);
  else
    fputc('\n',file);
  for(i=0;i<config->nsoil;i++)
  {
    fprintf(file,"%-18s %6.2f %5.2f %5.2f",
            config->soilpar[i].name,config->soilpar[i].sand,config->soilpar[i].silt,config->soilpar[i].clay);
    if(config->soilpar_option==PRESCRIBED_SOILPAR)
      fprintf(file," %6.2f %6.3f %5.3f %5.3f",
              config->soilpar[i].Ks,config->soilpar[i].wpwp,config->soilpar[i].wfc,config->soilpar[i].wsat);
    fprintf(file," %3d %6.3f %7.3f %8.3f %7.3f %7.3f %10.3f",
            config->soilpar[i].hsg+1,
            config->soilpar[i].tdiff_0,config->soilpar[i].tdiff_15,config->soilpar[i].tdiff_100,
            config->soilpar[i].tcond_pwp,config->soilpar[i].tcond_100,config->soilpar[i].tcond_100_ice);
    if(config->with_nitrogen)
      fprintf(file," %7.3f %7.3f %7.4f %7.3f %10.3f %8.1f",
              config->soilpar[i].a_nit,config->soilpar[i].b_nit,config->soilpar[i].c_nit,
              config->soilpar[i].d_nit,config->soilpar[i].anion_excl,config->soilpar[i].cn_ratio);
    fputc('\n',file);
  }
  fputs("------------------ ------ ----- -----",file);
  if(config->soilpar_option==PRESCRIBED_SOILPAR)
    fputs(" ------ ------ ----- -----",file);
  fputs(" --- ------ ------- -------- ------- ------- ----------",file);
  if(config->with_nitrogen)
    fputs(" ------- ------- ------- ------- ---------- --------",file);
  fputc('\n',file);
} /* of 'fprintsoilpar' */

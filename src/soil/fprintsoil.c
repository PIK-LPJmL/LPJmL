/**************************************************************************************/
/**                                                                                \n**/
/**               f  p  r  i  n  t  s  o  i  l  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints soil variables                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprintsoil(FILE *file,      /**< pointer to text file */
                const Soil *soil /**< pointer to soil variables to print */
               )
{
  int l;
  Pool sum={0,0};
  char *soilstates[]={"NOSTATE","BELOW_T_ZERO","AT_T_ZERO","ABOVE_T_ZERO",
                      "FREEZING","THAWING"};
  fprintf(file,"Soil type:\t%s\n",soil->par->name);
  fputs("Carbon pools:\n"
        "\tlayer slow (gC/m2) fast (gC/m2)\n"
        "\t----- ------------ ------------\n",file);
  forrootsoillayer(l)
  {
    fprintf(file,"\t%5d %12.2f %12.2f\n",l,soil->cpool[l].slow,soil->cpool[l].fast);
    sum.slow+=soil->cpool[l].slow;
    sum.fast+=soil->cpool[l].fast;
  }
  fputs("\t----- ------------ ------------\n",file);
  fprintf(file,"\tTOTAL %12.2f %12.2f\n",sum.slow,sum.fast);
  fprintf(file,"Rootlayer:\t%d",l);
  fprintf(file,"\ndecomp_litter_mean:\t%.2f (gC/m2)\n",soil->decomp_litter_mean);
  fprintf(file,"Litter:");
  fprintlitter(file,&soil->litter);
  fprintf(file,"\nmean maxthaw:\t%.2f (mm)\n",soil->mean_maxthaw);
  fputs("Layer       ",file);
  for(l=0;l<NSOILLAYER+1;l++)
    fprintf(file," %12d",l);
  fputs("\n------------",file);
  for(l=0;l<NSOILLAYER+1;l++)
    fputs(" ------------",file);
  fputs("\nsoil-state  ",file);
  foreachsoillayer(l)
    fprintf(file," %12s",soilstates[soil->state[l]]);    
  fputs("\nWater       ",file);
  foreachsoillayer(l)
    fprintf(file," %12.2f",soil->w[l]);
  fputs("\nfree Water  ",file);
  foreachsoillayer(l)
    fprintf(file," %12.2f",soil->w_fw[l]);
  fputs("\nfree ice    ",file);
  foreachsoillayer(l)
    fprintf(file," %12.2f",soil->ice_fw[l]);
  fputs("\nTemp        ",file);
  for(l=0;l<NSOILLAYER+1;l++)
    fprintf(file," %12.2f",soil->temp[l]);
  fputs("\nIce depth   ",file);
  foreachsoillayer(l)
    fprintf(file," %12.2f",soil->ice_depth[l]);
  fputs("\nFreeze depth",file);
  foreachsoillayer(l)
    fprintf(file," %12.1f",soil->freeze_depth[l]);
  fputs("\nIce pwph    ",file);
  foreachsoillayer(l)
    fprintf(file," %12.2f",soil->ice_pwp[l]);
  fputs("\n------------",file);
  for(l=0;l<NSOILLAYER+1;l++)
    fputs(" ------------",file);
  fprintf(file,"\nSnowpack:\t%g (mm)\n",soil->snowpack);
  fprintf(file,"rw_buffer:\t%g (mm)\n",soil->rw_buffer);
} /* of 'fprintsoil' */

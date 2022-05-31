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

void fprintsoil(FILE *file,           /**< pointer to text file */
                const Soil *soil,     /**< pointer to soil variables to print */
                const Pftpar *pftpar, /**< PFT parameter array */
                int ntotpft,          /**< total number of PFTs */
                int with_nitrogen     /**< nitrogen cycle enabled */
               )
{
  int l,p;
  Pool sum={{0,0},{0,0}};
  Real sum_NO3=0, sum_NH4=0;
  char *soilstates[]={"NOSTATE","BELOW_T_ZERO","AT_T_ZERO","ABOVE_T_ZERO",
                      "FREEZING","THAWING"};
  fprintf(file,"Soil type:\t%s\n",soil->par->name);
  fputs("C shift fast:\n"
        "PFT                                     ",file); 
  forrootsoillayer(l)
    fprintf(file," %5d",l);
  fputs("\n----------------------------------------",file);
  forrootsoillayer(l)
    fputs(" -----",file);
  fputc('\n',file);
  for(p=0;p<ntotpft;p++)
  {
    fprintf(file,"%-40s",pftpar[p].name);
    forrootsoillayer(l)
      fprintf(file," %5.2f",soil->c_shift[l][p].fast);
    fputc('\n',file);
  }
  fputs("----------------------------------------",file);
  forrootsoillayer(l)
    fputs(" -----",file);
  fputc('\n',file);
  fputs("C shift slow:\n"
        "PFT                                     ",file); 
  forrootsoillayer(l)
    fprintf(file," %5d",l);
  fputs("\n----------------------------------------",file);
  forrootsoillayer(l)
    fputs(" -----",file);
  fputc('\n',file);
  for(p=0;p<ntotpft;p++)
  {
    fprintf(file,"%-40s",pftpar[p].name);
    forrootsoillayer(l)
      fprintf(file," %5.2f",soil->c_shift[l][p].slow);
    fputc('\n',file);
  }
  fputs("----------------------------------------",file);
  forrootsoillayer(l)
    fputs(" -----",file);
  fputc('\n',file);
  if(with_nitrogen)
  {
    fputs("Stock pools:\n"
          "\tlayer slow (gC/m2) fast (gC/m2) slow (gN/m2) fast (gN/m2) k_mean_slow k_mean_fast\n"
          "\t----- ------------ ------------ ------------ ------------ ----------- -----------\n",file);
    forrootsoillayer(l)
    {
      fprintf(file,"\t%5d %12.2f %12.2f %12.2f %12.2f %1.9f %1.9f\n",
              l,soil->pool[l].slow.carbon,soil->pool[l].fast.carbon,
              soil->pool[l].slow.nitrogen,soil->pool[l].fast.nitrogen,
              soil->k_mean[l].slow/soil->count,soil->k_mean[l].fast/soil->count);
      sum.slow.carbon+=soil->pool[l].slow.carbon;
      sum.fast.carbon+=soil->pool[l].fast.carbon;
      sum.fast.nitrogen+=soil->pool[l].fast.nitrogen;
      sum.slow.nitrogen+=soil->pool[l].slow.nitrogen;
    }
    fputs("\t----- ------------ ------------ ------------ ------------ ----------- -----------\n",file);
    fprintf(file,"\tTOTAL %12.2f %12.2f %12.2f %12.2f\n",
            sum.slow.carbon,sum.fast.carbon,sum.slow.nitrogen,sum.fast.nitrogen);

    fputs("\n\tlayer NO3 (gN/m2) NH4 (gN/m2)\n"
          "\t----- ----------- -----------\n",file);
    forrootsoillayer(l)
    {
      fprintf(file,"\t%5d %11.4f %11.4f\n",l,soil->NO3[l],soil->NH4[l]);
      sum_NO3+=soil->NO3[l];
      sum_NH4+=soil->NH4[l];
    }
    fputs("\t----- ----------- -----------\n",file);
    fprintf(file,"\tTOTAL %11.4f %11.4f\n",
            sum_NO3,sum_NH4);
  }
  else
  {
    fputs("Carbon pools:\n"
          "\tlayer slow (gC/m2) fast (gC/m2)\n"
          "\t----- ------------ ------------\n",file);
    forrootsoillayer(l)
    {
      fprintf(file,"\t%5d %12.2f %12.2f\n",
              l,soil->pool[l].slow.carbon,soil->pool[l].fast.carbon);
      sum.slow.carbon+=soil->pool[l].slow.carbon;
      sum.fast.carbon+=soil->pool[l].fast.carbon;
    }
    fputs("\t----- ------------ ------------\n",file);
    fprintf(file,"\tTOTAL %12.2f %12.2f\n",
            sum.slow.carbon,sum.fast.carbon);
  }
  fprintf(file,"\nRootlayer:\t%d\n",l);
  fprintf(file,"Decomp_mean carbon:\t%.2f (gC/m2)\n",soil->decomp_litter_mean.carbon);
  if(with_nitrogen)
    fprintf(file,"Decomp_mean nitrogen:\t%.2f (gC/m2)",soil->decomp_litter_mean.nitrogen);
  fputs("\nLitter:",file);
  fprintlitter(file,&soil->litter,with_nitrogen);
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
  fprintf(file,"Snowheight:\t%g (mm)\n",soil->snowheight);
  fprintf(file,"Snowfraction:\t%g\n",soil->snowfraction);
  fprintf(file,"rw_buffer:\t%g (mm)\n",soil->rw_buffer);
} /* of 'fprintsoil' */

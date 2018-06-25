/**************************************************************************************/
/**                                                                                \n**/
/**     f  p  r  i  n  t  c  o  u  n  t  r  y  p  a  r  .  c                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprintcountrypar(FILE *file, /**< pointer to text file */
                      const Countrypar *countrypar, /**< country params to print */
                      int ncountry, /**< number of country params */
                      int ncft      /**< number of crop PFTs */
                     )
{
  char *irrigsys[]={"SURF","SPRINK","DRIP"};
  int i,j;
  fputs("Country                                     ",file);
  if(countrypar[0].laimax_cft!=NULL)
    for(i=0;i<ncft;i++)
      fprintf(file," %2d",i+1);
  else
    fputs(" cereal maize",file);
  fputs(" Irrigsys\n",file);
  fputs("--------------------------------------------",file);
  if(countrypar[0].laimax_cft!=NULL)
    for(i=0;i<ncft;i++)
      fputs(" --",file);
  else
    fputs(" ------ -----",file);
  fputs(" --------\n",file);
  for(i=0;i<ncountry;i++)
  {
    fprintf(file,"%44s",countrypar[i].name);
    if(countrypar[i].laimax_cft!=NULL)
      for(j=0;j<ncft;j++)
        fprintf(file," %2.0g",countrypar[i].laimax_cft[j]);
    else
      fprintf(file," %6.1g %5.1g",countrypar[i].laimax_tempcer,
              countrypar[i].laimax_maize);
    fprintf(file," %s\n",irrigsys[countrypar[i].default_irrig_system-1]);
  }
  fputs("--------------------------------------------",file);
  if(countrypar[0].laimax_cft!=NULL)
    for(i=0;i<ncft;i++)
      fputs(" --",file);
  else
    fputs(" ------ -----",file);
  fputs(" --------\n",file);
} /* of 'fprintcountrypar' */

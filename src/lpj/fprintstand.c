/**************************************************************************************/
/**                                                                                \n**/
/**                 f  p  r  i  n  t  s  t  a  n  d  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints stand variables in text file                               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprintstand(FILE *file,           /**< Pointer to text file */
                 const Stand *stand,   /**< Stand pointer */
                 const Pftpar *pftpar, /**< PFT parameter array */
                 int ntotpft,          /**< total number of PFTs */
                 int with_nitrogen     /**< nitrogen cycle enabled */
                )
{
  int l;
  fprintf(file,"Standfrac:\t%g\n",stand->frac);
  fprintf(file,"Standtype:\t%s\n",stand->type->name);
  /* print stand-specific data */
  stand->type->fprint(file,stand,pftpar);
  fprintsoil(file,&stand->soil,pftpar,ntotpft,with_nitrogen);
  fputs("Frac_g:\t\t",file);
  for(l=0;l<NSOILLAYER;l++)
    fprintf(file,"%.2f ",stand->frac_g[l]);
  fputc('\n',file);
  fprintpftlist(file,&stand->pftlist,with_nitrogen);
} /* of 'fprintstand' */

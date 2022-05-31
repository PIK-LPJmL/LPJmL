/**************************************************************************************/
/**                                                                                \n**/
/**                p  r  i  n  t  p  f  t  n  a  m  e  s  .  c                     \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes PFT names to text file                                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprintpftnames(FILE *file,            /**< pointer to text file */
                    const Pftpar pftpar[], /**< PFT parameter array */
                    int npft               /**< size of array */
                   )
{
  int p;
  for(p=0;p<npft;p++)
    fprintf(file,(p==0) ? "\"%s\"" : ",\"%s\"",pftpar[p].name);
} /* of 'fprintpftnames' */

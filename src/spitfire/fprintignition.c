/**************************************************************************************/
/**                                                                                \n**/
/**            f  p  r  i  n  t  i  g  n  i  t  i  o  n  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints ignition data to text file                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprintignition(FILE *file,const Ignition *ignition)
{
  fprintf(file,"Nesterov acum:\t%g\n"
          "Nesterov max:\t%g\n"
          "Nesterov days:\t%d\n",
          ignition->nesterov_accum,ignition->nesterov_max,ignition->nesterov_day);
} /* of 'fprintignition' */

/**************************************************************************************/
/**                                                                                \n**/
/**           f  p  r  i  n  t  p  a  r  _  g  r  a  s  s  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints PFT parameter                                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grass.h"

void fprintpar_grass(FILE *file,const Pftpar *par)
{
  const Pftgrasspar *pargrass;
  pargrass=par->data;
  fprintf(file,"turnover:\t%g %g (yr)\n"
               "C:N ratio:\t%g %g\n"
               "reprod cost:\t%g\n",
          pargrass->turnover.leaf,pargrass->turnover.root,
          par->respcoeff*param.k/pargrass->cn_ratio.leaf,
          par->respcoeff*param.k/pargrass->cn_ratio.root,
          pargrass->reprod_cost);
} /* of 'fprintpar_grass' */

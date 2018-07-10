/**************************************************************************************/
/**                                                                                \n**/
/**                 f  p  r  i  n  t  _  g  r  a  s  s  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
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


void fprint_grass(FILE *file,const Pft *pft)
{
  Pftgrass *grass;
  grass=pft->data;
  fprintf(file,"Mass:\t\t");
  fprintgrassphys(file,grass->ind);
  fprintf(file,"\n");
  fprintf(file,"\nExcess carbon:\t%g (gC/m2)\n",grass->excess_carbon);
  fprintf(file,"falloc:\t\t%g %g\n",grass->falloc.leaf,grass->falloc.root);
} /* of 'fprint_grass' */

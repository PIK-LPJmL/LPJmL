/**************************************************************************************/
/**                                                                                \n**/
/**                 f  p  r  i  n  t  _  g  r  a  s  s  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints grass-specific state variables in text file                \n**/
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

void fprint_grass(FILE *file,    /**< pointer to text file */
                  const Pft *pft /**< pointer to grass PFT */
                 )
{
  Pftgrass *grass;
  grass=pft->data;
  fputs("Cmass:\t\t",file);
  fprintgrassphys(file,grass->ind);
  fputc('\n',file);
} /* of 'fprint_grass' */

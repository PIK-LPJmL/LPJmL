/**************************************************************************************/
/**                                                                                \n**/
/**            f  p  r  i  n  t  h  y  d  r  o  p  a  r  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function prints hydrotope parameters to text file                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void fprinthydropar(FILE *file,              /**< pointer to text file */
                    const Hydropar *hydropar /**< hydrological parameter */
                   )
{
  fprintf(file, "wland_min:\t%g\n"
          "cti_max:\t%g\n"
          "wtab_thres:\t%g\n"
          "cti_thres:\t%g\n",
          hydropar->wland_min, hydropar->cti_max,
          hydropar->wtab_thres, hydropar->cti_thres);
} /* of 'fprinthydropar' */

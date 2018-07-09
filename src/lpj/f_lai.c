/**************************************************************************************/
/**                                                                                \n**/
/**                            f  _  l  a  i  .  c                                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function implements equation C11 in Smith et al. (2014)                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define k_l 0.08 /* changed from 0.12 in Smith et al. (2014) */

Real f_lai(Real lai)
{
  if(lai<1.0)
    return max(0.1,lai);
  return exp(k_l*min(lai,7));
} /* of 'f_lai' */

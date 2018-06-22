/**************************************************************************************/
/**                                                                                \n**/
/**         n  o  e  s  t  a  b  l  i  s  h  m  e  n  t  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Default establishment function                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real noestablishment(Pft * UNUSED(pft),
                     Real UNUSED(fpc_total),
                     Real UNUSED(fpc_tree),
                     int UNUSED(n_est)
                    )
{
  return 0;
} /* of 'noestablishment' */

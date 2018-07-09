/**************************************************************************************/
/**                                                                                \n**/
/**         n  u  p  t  a  k  e  _  t  e  m  p  _  f  c  n  .  c                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates temperature limitation in N uptake of roots            \n**/
/**     after Thornley (1991)                                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define T_m 15.0
#define T_0 (-25.0)
#define T_r 15.0

Real nuptake_temp_fcn(Real soiltemp)
{
 return max((soiltemp-T_0)*(2*T_m-T_0-soiltemp)/(T_r-T_0)/(2*T_m-T_0-T_r),0);
} /* of 'nuptake_temp_fcn' */

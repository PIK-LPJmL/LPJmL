/**************************************************************************************/
/**                                                                                \n**/
/**              a  l  b  e  d  o  _  s  o  i  l  .  c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates soil albedo in dependence on soil moisture             \n**/
/**     after S. Gascoin, A. Ducharne, P. Ribstein, E. Perroy, and                 \n**/
/**     P. Wagnon, Sensitivity of bare soil albedo to surface soil                 \n**/
/**     moisture on the moraine of the Zongo glacier (Bolivia)                     \n**/
/**     GEOPHYSICAL RESEARCH LETTERS, VOL. 36, L02405, 2009                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define R_dry 0.15
#define R_sat 0.46
#define c 12.7

Real albedo_soil(Real vwc /* volumetric water content (m3/m3) */
                )         /* returns soil albedo */

{
  return (R_sat-R_dry)*exp(-c *vwc)+R_sat;
} /* of 'albedo_soil' */

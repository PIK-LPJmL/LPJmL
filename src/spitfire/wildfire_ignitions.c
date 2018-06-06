/**************************************************************************************/
/**                                                                                \n**/
/**     w  i  l  d  f  i  r  e  _  i  g  n  i  t  i  o  n  s  .  c                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real wildfire_ignitions(Real d_fdi,    /* daily fire danger index */
                        Real ignition, /* ignition */
                        Real area      /* cell area m^2 */
                       )
{
  Real d_numfire;
  d_numfire= d_fdi*ignition * (area*1e-4);
  return d_numfire;
} /* of 'wildfire_ignitions' */

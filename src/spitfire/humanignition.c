/**************************************************************************************/
/**                                                                                \n**/
/**               h  u  m  a  n  i  g  n  i  t  i  o  n  .  c                      \n**/
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

Real humanignition(Real popdens,
                   Ignition* ignition)
{
  Real a_nd, human_ign, k_pd;

  a_nd = ignition->human;
  k_pd = 6.8*(exp(-0.5*sqrt(popdens)));
  human_ign = k_pd * a_nd * popdens;
  return human_ign;
} /* of 'humanignition' */

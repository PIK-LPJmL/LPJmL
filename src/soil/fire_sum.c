/**************************************************************************************/
/**                                                                                \n**/
/**                f  i  r  e  _  s  u  m  .  c                                    \n**/
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

Real fire_sum(const Litter *litter, /**< Litter pool */
              Real w_surf           /**< surface water */
             )                      /** \return probability of the occurrence of at least one fire */
{
  Real moist_factor;
  moist_factor=moistfactor(litter);
  return (moist_factor>0) ? 
            exp(-M_PI*(w_surf/moist_factor)*(w_surf/moist_factor)) : 0;
} /* of 'fire_sum' */

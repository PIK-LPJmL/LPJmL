/**************************************************************************************/
/**                                                                                \n**/
/**                f  i  r  e  _  p  r  o  b  .  c                                 \n**/
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

#define minfuel 200.0 /* fuel threshold to carry fire (gC/m2) */

Real fire_prob(const Litter *litter, /**< pointer to litter */
               Real fire_sum         /**<  cumulative daily fire probability */
               )                     /** \return fire probability */
{
  Real fire_index,sm,fire_frac;
  fire_index=fire_sum/NDAYYEAR;
  sm=fire_index-1;
  fire_frac=fire_index*exp(sm/(0.45*sm*sm*sm+2.83*sm*sm+2.96*sm+1.04));
#ifdef SAFE
  if (fire_frac>1.0)
    fail(INVALID_FIRE_PROB_ERR,TRUE,"fire: probability of fire=%g >1.0",
         fire_frac);
#endif
  return (fire_frac<0.001 ||(litter_ag_sum(litter)<minfuel)) ?
            0.001 : fire_frac;
} /* of 'fire_prob' */

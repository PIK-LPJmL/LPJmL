/**************************************************************************************/
/**                                                                                \n**/
/**                 f  i  r  e  _  g  r  a  s  s  .  c                             \n**/
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
#include "grass.h"

Stocks fire_grass(Pft *pft,      /**< pointer to grass PFT */
                  Real *fireprob /**< fire probability */
                 )               /** \return burnt stock (gC/m2,gN/m2) */
{
  Real disturb;
  Stocks flux;
  Pftgrass *grass;
  grass=pft->data;
  disturb=(1-pft->par->resist)**fireprob;
  flux.carbon=disturb*pft->nind*(grass->ind.leaf.carbon+grass->excess_carbon);
  flux.nitrogen=disturb*pft->nind*grass->ind.leaf.nitrogen;
  grass->ind.leaf.carbon*=(1-disturb);
  grass->ind.leaf.nitrogen*=(1-disturb);
  grass->excess_carbon*=(1-disturb);
  return flux;
} /* of 'fire_grass' */

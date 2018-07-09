/**************************************************************************************/
/**                                                                                \n**/
/**               v  m  a  x  l  i  m  i  t  _  g  r  a  s  s  .  c                \n**/
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

Real vmaxlimit_grass(const Pft *pft, /**< pointer to PFT */
                     Real daylength, /**< day length (h) */
                     Real temp       /**< temperature (deg C) */
                    )                /** \return vmax (gC/m2/day) */
{
  const Pftgrass *grass;
  grass=pft->data; 
  return (grass->ind.leaf.nitrogen-param.n0*0.001*(grass->ind.leaf.carbon))/exp(-param.k_temp*(temp-25))/f_lai(lai_grass(pft))/param.p/0.02314815*daylength;
  } /* of 'vmaxlimit_grass' */

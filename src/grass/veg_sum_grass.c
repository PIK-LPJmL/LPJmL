/**************************************************************************************/
/**                                                                                \n**/
/**           v  e  g  _  s  u  m  _  g  r  a  s  s  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates vegetation carbon and nitrogen of grass PFTs           \n**/
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

Real vegc_sum_grass(const Pft *pft /**< pointer to grass PFT */
                   )               /** \return vegetation carbon (gC/m2) */
{
  const Pftgrass *grass;
  grass=pft->data;
  return (phys_sum_grass(grass->ind)+grass->excess_carbon)*pft->nind-grass->turn_litt.leaf.carbon-grass->turn_litt.root.carbon;
} /* of 'vegc_sum_grass' */

Real vegn_sum_grass(const Pft *pft /**< pointer to grass PFT */
                   )               /** \return vegetation nitrogen (gN/m2) */
{
  const Pftgrass *grass;
  grass=pft->data;
  return phys_sum_grass_n(grass->ind)*pft->nind+pft->nbalance_cor-grass->turn_litt.leaf.nitrogen-grass->turn_litt.root.nitrogen;
} /* of 'vegn_sum_grass' */

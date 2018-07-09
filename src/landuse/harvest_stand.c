/**************************************************************************************/
/**                                                                                \n**/
/**         h  a  r  v  e  s  t  _  s  t  a  n  d  .  c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function harvests grassland stand                                          \n**/
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
#include "agriculture.h"
#include "grassland.h"

Harvest harvest_grass(Stand *stand, /**< pointer to stand */
                      Real hfrac    /**< harvest fractions */
                     )              /** \return harvested grass (gC/m2) */
{
  Harvest harvest;
  Harvest sum={{0,0},{0,0},{0,0},{0,0}};
  Pftgrass *grass;
  Pft *pft;
  int p;

  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    harvest.harvest.carbon=grass->ind.leaf.carbon*hfrac;
    harvest.harvest.nitrogen=grass->ind.leaf.nitrogen*hfrac;
    grass->ind.leaf.carbon*=(1-hfrac);
    grass->ind.leaf.nitrogen*=(1-hfrac);
    sum.harvest.carbon+=harvest.harvest.carbon;
    sum.harvest.nitrogen+=harvest.harvest.nitrogen;
    grass->max_leaf = grass->ind.leaf.carbon;
    pft->phen=0.3;
    pft->gdd=30;
  }
  return sum;
} /* of 'harvest_grass' */

Harvest harvest_stand(Output *output, /**< Output data */
                      Stand *stand,   /**< pointer to grassland stand */
                      Real hfrac      /**< harvest fraction */
                     )                /** \return harvested carbon (gC/m2) */
{
  Harvest harvest;

  harvest=harvest_grass(stand,hfrac);
  output->flux_harvest.carbon+=(harvest.harvest.carbon+harvest.residual.carbon)*stand->frac;
  output->flux_harvest.nitrogen+=(harvest.harvest.nitrogen+harvest.residual.nitrogen)*stand->frac;
  output->dcflux+=(harvest.harvest.carbon+harvest.residual.carbon)*stand->frac;
  return harvest;

} /* of 'harvest_stand' */

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
#include "grassland.h"

static Harvest harvest_grass(Stand *stand, /**< pointer to stand */
                             Real hfrac    /**< harvest fraction */
                            )              /** \return harvested grass (gC/m2) */
{
  Harvest harvest;
  Harvest sum={0,0,0,0};
  Pftgrass *grass;
  Pft *pft;
  int p;

  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    harvest.harvest=grass->ind.leaf*hfrac;
    grass->ind.leaf*=(1-hfrac);
    sum.harvest+=harvest.harvest*pft->nind;
    grass->max_leaf = grass->ind.leaf;
    pft->phen=0.3;
    pft->gdd=30;
  }
  return sum;
} /* of 'harvest_grass' */

static Harvest harvest_grass_mowing(Stand *stand)
{
  Harvest harvest;
  Harvest sum={0,0,0,0};
  Pftgrass *grass;
  Pft *pft;
  int p;
  Real fpc_sum=0.0;
  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    fpc_sum+=(1.0-exp(-param.k_beer*lai_grass(pft)*pft->nind));
  }
  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    harvest.harvest=grass->ind.leaf - STUBBLE_HEIGHT_MOWING*pft->fpc/fpc_sum;
    grass->ind.leaf = STUBBLE_HEIGHT_MOWING*pft->fpc/fpc_sum;
    sum.harvest+=harvest.harvest*pft->nind;
    pft->gdd=pft->phen=0.0; // change -> relative from ind.leaf
  }
  return sum;
} /* of 'harvest_grass_mowing' */

static Harvest harvest_grass_grazing_ext(Stand *stand)
{
  Grassland *grassland;
  Harvest sum={0,0,0,0};
  Pftgrass *grass;
  Pft *pft;
  int p;
  Real bm_grazed;
  Real fact;
  Real bm_tot=0.0;
  Real fpc_sum=0.0;
  Real bm_grazed_pft;
  grassland=stand->data;
  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    bm_tot+=grass->ind.leaf;
    fpc_sum+=(1.0-exp(-param.k_beer*lai_grass(pft)*pft->nind));
  }
//  bm_grazed = stand->cell->ml.nr_of_lsus_ext * DEMAND_COW_EXT;
  bm_grazed = 1e-4* grassland->nr_of_lsus_ext * DEMAND_COW_EXT;

  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    if (bm_tot < 1e-5) // to avoid division by zero!
      fact = 1;
    else
      fact = grass->ind.leaf / bm_tot;
    bm_grazed_pft = bm_grazed * fact;
    if (grass->ind.leaf - bm_grazed_pft < GRAZING_STUBLE*pft->fpc/fpc_sum)
      bm_grazed_pft = grass->ind.leaf - GRAZING_STUBLE*pft->fpc/fpc_sum;
    if (bm_grazed_pft < 0)
      bm_grazed_pft = 0;
    pft->gdd = (1-bm_grazed_pft/grass->ind.leaf) * pft->gdd;

    grass->ind.leaf -= bm_grazed_pft;
    sum.harvest += (1-MANURE)*bm_grazed_pft*pft->nind;                       // 60% atmosphere, 15% cows
    stand->soil.cpool->fast += MANURE * bm_grazed_pft*pft->nind;
    //stand->soil.cpool[0].fast += MANURE * bm_grazed_pft;             // 25% back to soil
    // pft->phen recalculated in phenology_grass
  }
  return sum;
} /* of 'harvest_grass_grazing_ext' */


static Harvest harvest_grass_grazing_int(Stand *stand)
{
  Harvest sum={0,0,0,0};
  Pftgrass *grass;
  Pft *pft;
  int p;
  Real rotation_len;
  Real fact;
  Real bm_grazed;
  Real bm_tot=0.0;
  Real bm_grazed_pft;
  Grassland *grassland;
  Real fpc_sum=0.0;

  grassland=stand->data;
  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    bm_tot+=grass->ind.leaf;
    fpc_sum+=(1.0-exp(-param.k_beer*lai_grass(pft)*pft->nind));
  }

  if (grassland->rotation.mode == RM_UNDEFINED) //initial calculate grazing days and recovery days
  {
    rotation_len = (bm_tot - GRAZING_STUBLE) / (1e4*grassland->nr_of_lsus_int * DEMAND_COW_INT) ;
    if (rotation_len > MAX_ROTATION_LENGTH)
      rotation_len = MAX_ROTATION_LENGTH;

    if (rotation_len > MIN_ROTATION_LENGTH) // otherwise wait for more growth
    {
      grassland->rotation.grazing_days = (int)ceil(rotation_len/MAX_PADDOCKS);
      grassland->rotation.paddocks = (int)floor((rotation_len/grassland->rotation.grazing_days) + 0.5);
      grassland->rotation.recovery_days = (grassland->rotation.paddocks-1) * grassland->rotation.grazing_days;
      grassland->rotation.mode = RM_GRAZING;
    }
  }

  if (grassland->rotation.mode == RM_GRAZING)
  {
    bm_grazed = grassland->nr_of_lsus_int * DEMAND_COW_INT * grassland->rotation.paddocks;
    foreachpft(pft,p,&stand->pftlist)
    {
      grass=pft->data;
      fact = grass->ind.leaf / bm_tot;
      bm_grazed_pft = bm_grazed * fact;

      if (grass->ind.leaf - bm_grazed_pft < GRAZING_STUBLE*pft->fpc/fpc_sum)
        bm_grazed_pft = grass->ind.leaf - GRAZING_STUBLE*pft->fpc/fpc_sum;

      if (bm_grazed_pft < 0)
        bm_grazed_pft =0;

      pft->gdd = (1-bm_grazed_pft/grass->ind.leaf) * pft->gdd;

      grass->ind.leaf -= bm_grazed_pft;
      sum.harvest += (1-MANURE)*bm_grazed_pft*pft->nind;              // 60% atmosphere, 15% cows
      stand->soil.cpool->fast += MANURE * bm_grazed_pft*pft->nind;
      // stand->soil.cpool[0].fast += MANURE * bm_grazed_pft;    // 25% back to soil
    }

    grassland->rotation.grazing_days--;
    if (grassland->rotation.grazing_days == 0)
      grassland->rotation.mode = RM_RECOVERY;
  }
  else if (grassland->rotation.mode == RM_RECOVERY)
  {
    grassland->rotation.recovery_days--;
    if (grassland->rotation.recovery_days == 0)
      grassland->rotation.mode = RM_UNDEFINED;
  }
  return sum;
} /* of 'harvest_grass_grazing_int' */

/*
 * called in function daily_grassland() when managed grassland
 * is harvested
 *
 */

Harvest harvest_stand(Output *output, /**< Output data */
                      Stand *stand,   /**< pointer to grassland stand */
                      Real hfrac      /**< harvest fraction */
                     )                /** \return harvested carbon (gC/m2) */
{
  Harvest harvest;
  if (stand->type->landusetype == GRASSLAND)
  {
    switch (stand->cell->ml.grass_scenario)
    {
      case GS_DEFAULT: // default
        harvest=harvest_grass(stand,hfrac);
         break;
      case GS_MOWING: // mowing
        harvest=harvest_grass_mowing(stand);
        break;
      case GS_GRAZING_EXT: // ext. grazing
        harvest=harvest_grass_grazing_ext(stand);
        break;
      case GS_GRAZING_INT: // int. grazing
        harvest=harvest_grass_grazing_int(stand);
        break;
    }
  }
  else /* option for biomass_grass */
  {
    harvest=harvest_grass(stand,hfrac);
  }
  output->flux_harvest+=(harvest.harvest+harvest.residual)*stand->frac;
  output->dcflux+=(harvest.harvest+harvest.residual)*stand->frac;
  return harvest;

} /* of 'harvest_stand' */

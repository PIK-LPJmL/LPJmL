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
  Output *output;
  output=&stand->cell->output;

  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    harvest.harvest.carbon=grass->ind.leaf.carbon*hfrac;
    harvest.harvest.nitrogen=grass->ind.leaf.nitrogen*hfrac*0.5; /*0.25*/
    stand->soil.NH4[0]+=grass->ind.leaf.nitrogen*hfrac*0.5*pft->nind;
    grass->ind.leaf.carbon*=(1-hfrac);
    grass->ind.leaf.nitrogen*=(1-hfrac);
    stand->soil.litter.bg[pft->litter].carbon+=grass->ind.root.carbon*hfrac*param.rootreduction*pft->nind;
    output->alittfall.carbon+=grass->ind.root.carbon*hfrac*param.rootreduction*pft->nind*stand->frac;
    stand->soil.litter.bg[pft->litter].nitrogen+=grass->ind.root.nitrogen*hfrac*param.rootreduction*pft->nind*pft->par->fn_turnover;
    output->alittfall.nitrogen+=grass->ind.root.nitrogen*hfrac*param.rootreduction*pft->nind*stand->frac*pft->par->fn_turnover;
    pft->bm_inc.nitrogen+=grass->ind.root.nitrogen*hfrac*param.rootreduction*pft->nind*pft->nind*(1-pft->par->fn_turnover);
    grass->ind.root.carbon*=(1-hfrac*param.rootreduction);
    grass->ind.root.nitrogen*=(1-hfrac*param.rootreduction);
    sum.harvest.carbon+=harvest.harvest.carbon*pft->nind;
    sum.harvest.nitrogen+=harvest.harvest.nitrogen*pft->nind;
    grass->max_leaf = grass->ind.leaf.carbon;
    pft->phen=1; /*0.3;*/
    pft->gdd=30;
  }
  return sum;
} /* of 'harvest_grass' */

static Harvest harvest_grass_mowing(Stand *stand)
{
  Harvest harvest;
  Harvest sum={{0,0},{0,0},{0,0},{0,0}};
  Pftgrass *grass;
  Pft *pft;
  int p;
  Real fpc_sum=0.0;
  Real hfrac;
  Output *output;
  output=&stand->cell->output;
  foreachpft(pft,p,&stand->pftlist)
    fpc_sum+=pft->fpc;

  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    harvest.harvest.carbon = grass->ind.leaf.carbon - STUBBLE_HEIGHT_MOWING*pft->fpc/fpc_sum;
    hfrac=harvest.harvest.carbon/grass->ind.leaf.carbon;
    harvest.harvest.nitrogen = hfrac*grass->ind.leaf.nitrogen*0.25;
    stand->soil.NH4[0]+=hfrac*grass->ind.leaf.nitrogen*0.75*pft->nind;
    grass->ind.leaf.carbon = STUBBLE_HEIGHT_MOWING*pft->fpc/fpc_sum;
    grass->ind.leaf.nitrogen -= harvest.harvest.nitrogen;

    stand->soil.litter.bg[pft->litter].carbon+=grass->ind.root.carbon*hfrac*param.rootreduction*pft->nind;
    output->alittfall.carbon+=grass->ind.root.carbon*hfrac*param.rootreduction*pft->nind*stand->frac;
    stand->soil.litter.bg[pft->litter].nitrogen+=grass->ind.root.nitrogen*hfrac*param.rootreduction*pft->nind*pft->par->fn_turnover;
    output->alittfall.nitrogen+=grass->ind.root.nitrogen*hfrac*param.rootreduction*pft->nind*stand->frac*pft->par->fn_turnover;
    pft->bm_inc.nitrogen+=grass->ind.root.nitrogen*hfrac*param.rootreduction*pft->nind*pft->nind*(1-pft->par->fn_turnover);

    grass->ind.root.carbon*=(1-hfrac*param.rootreduction);
    grass->ind.root.nitrogen*=(1-hfrac*param.rootreduction);
    sum.harvest.carbon+=harvest.harvest.carbon*pft->nind;
    sum.harvest.nitrogen+=harvest.harvest.nitrogen*pft->nind;
    pft->gdd=0.0; // change -> relative from ind.leaf
  }
  return sum;
} /* of 'harvest_grass_mowing' */

static Harvest harvest_grass_grazing_ext(Stand *stand)
{
  Harvest sum={{0,0},{0,0},{0,0},{0,0}};
  Pftgrass *grass;
  Pft *pft;
  int p;
  Real bm_grazed;
  Real fact;
  Stocks bm_tot =  {0.0,0.0};
  Stocks bm_grazed_pft;
  Real fpc_sum=0.0;
  Real hfrac;
  Output *output;
  output=&stand->cell->output;
  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    bm_tot.carbon += grass->ind.leaf.carbon;
    bm_tot.nitrogen+= grass->ind.leaf.nitrogen;
    fpc_sum+=pft->fpc;
  }
//  bm_grazed = stand->cell->ml.nr_of_lsus_ext * DEMAND_COW_EXT;
  bm_grazed = 1e-4* stand->cell->ml.nr_of_lsus_ext * DEMAND_COW_EXT;

  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    if (bm_tot.carbon < 1e-5) // to avoid division by zero!
      fact = 1;
    else
      fact = grass->ind.leaf.carbon/bm_tot.carbon;

    bm_grazed_pft.carbon   = bm_grazed * fact;
    if (grass->ind.leaf.carbon - bm_grazed_pft.carbon < GRAZING_STUBBLE*pft->fpc/fpc_sum)
      bm_grazed_pft.carbon = grass->ind.leaf.carbon - GRAZING_STUBBLE*pft->fpc/fpc_sum;
    if (bm_grazed_pft.carbon < 0)
      bm_grazed_pft.carbon = 0;
    hfrac=bm_grazed_pft.carbon/grass->ind.leaf.carbon;
    pft->gdd = (1-hfrac) * pft->gdd;


    /* Nitrogen */
    //bm_grazed_pft.nitrogen = bm_grazed * fact;
    bm_grazed_pft.nitrogen = hfrac*grass->ind.leaf.nitrogen;

    grass->ind.leaf.carbon -= bm_grazed_pft.carbon;
    sum.harvest.carbon     += (1-MANURE)*bm_grazed_pft.carbon*pft->nind;                       // 60% atmosphere, 15% cows
    stand->soil.pool->fast.carbon += MANURE * bm_grazed_pft.carbon*pft->nind;             // 25% back to soil

    grass->ind.leaf.nitrogen -=  bm_grazed_pft.nitrogen;
    sum.harvest.nitrogen     += (1-MANURE)*bm_grazed_pft.nitrogen*pft->nind;                       // 60% atmosphere, 15% cows
    stand->soil.pool->fast.nitrogen += MANURE * bm_grazed_pft.nitrogen*pft->nind;             // 25% back to soil

    stand->soil.litter.bg[pft->litter].carbon+=grass->ind.root.carbon*hfrac*param.rootreduction*pft->nind;
    output->alittfall.carbon+=grass->ind.root.carbon*hfrac*param.rootreduction*pft->nind*stand->frac;
    stand->soil.litter.bg[pft->litter].nitrogen+=grass->ind.root.nitrogen*hfrac*param.rootreduction*pft->nind*pft->par->fn_turnover;
    output->alittfall.nitrogen+=grass->ind.root.nitrogen*hfrac*param.rootreduction*pft->nind*stand->frac*pft->par->fn_turnover;
    pft->bm_inc.nitrogen+=grass->ind.root.nitrogen*hfrac*param.rootreduction*pft->nind*pft->nind*(1-pft->par->fn_turnover);

    grass->ind.root.carbon*=(1-hfrac*param.rootreduction);
    grass->ind.root.nitrogen*=(1-hfrac*param.rootreduction);
    // pft->phen recalculated in phenology_grass
  }
  return sum;
} /* of 'harvest_grass_grazing_ext' */

static Harvest harvest_grass_grazing_int(Stand *stand)
{
  Harvest sum={{0,0},{0,0},{0,0},{0,0}};
  Pftgrass *grass;
  Pft *pft;
  int p;
  Real rotation_len;
  Real fact;
  Real bm_grazed;
  Stocks bm_tot = {0,0};
  Stocks bm_grazed_pft;
  Rotation *rotation;
  Real fpc_sum=0.0;
  Real hfrac;
  Output *output;
  output=&stand->cell->output;
  rotation = &(stand->cell->ml.rotation);
  foreachpft(pft,p,&stand->pftlist)
  {
    grass=pft->data;
    bm_tot.carbon += grass->ind.leaf.carbon;
    bm_tot.nitrogen += grass->ind.leaf.nitrogen;
    fpc_sum+=pft->fpc;
  }

  if (rotation->rotation_mode == RM_UNDEFINED) //initial calculate grazing days and recovery days
  {
    rotation_len = (bm_tot.carbon - GRAZING_STUBBLE) / (1e-4*stand->cell->ml.nr_of_lsus_int * DEMAND_COW_INT) ;
    if (rotation_len > MAX_ROTATION_LENGTH)
      rotation_len = MAX_ROTATION_LENGTH;

    if (rotation_len > MIN_ROTATION_LENGTH) // otherwise wait for more growth
    {
      rotation->grazing_days = (int)ceil(rotation_len/MAX_PADDOCKS);
      rotation->paddocks = (int)floor((rotation_len/rotation->grazing_days) + 0.5);
      rotation->recovery_days = (rotation->paddocks-1) * rotation->grazing_days;
      rotation->rotation_mode = RM_GRAZING;
    }
  }

  if (rotation->rotation_mode == RM_GRAZING)
  {
    bm_grazed = 1e-4*stand->cell->ml.nr_of_lsus_int * DEMAND_COW_INT * rotation->paddocks;
    foreachpft(pft,p,&stand->pftlist)
    {
      grass=pft->data;
      fact = grass->ind.leaf.carbon / bm_tot.carbon;
      bm_grazed_pft.carbon = bm_grazed * fact;

      if (grass->ind.leaf.carbon - bm_grazed_pft.carbon < GRAZING_STUBBLE*pft->fpc/fpc_sum)
        bm_grazed_pft.carbon = grass->ind.leaf.carbon - GRAZING_STUBBLE*pft->fpc/fpc_sum;

      if (bm_grazed_pft.carbon < 0)
        bm_grazed_pft.carbon =0;

      hfrac=bm_grazed_pft.carbon/grass->ind.leaf.carbon;
      pft->gdd = (1-hfrac) * pft->gdd;

      /* Nitrogen */
      bm_grazed_pft.nitrogen = hfrac*grass->ind.leaf.nitrogen;

      grass->ind.leaf.carbon -= bm_grazed_pft.carbon;
      sum.harvest.carbon     += (1-MANURE)*bm_grazed_pft.carbon*pft->nind;              // 60% atmosphere, 15% cows
      stand->soil.pool->fast.carbon += MANURE * bm_grazed_pft.carbon*pft->nind;    // 25% back to soil

      grass->ind.leaf.nitrogen -= bm_grazed_pft.nitrogen;
      sum.harvest.nitrogen     += (1-MANURE)*bm_grazed_pft.nitrogen*pft->nind;              // 60% atmosphere, 15% cows
      stand->soil.pool->fast.nitrogen += MANURE * bm_grazed_pft.nitrogen*pft->nind;    // 25% back to soil

      stand->soil.litter.bg[pft->litter].carbon+=grass->ind.root.carbon*hfrac*param.rootreduction*pft->nind;
      output->alittfall.carbon+=grass->ind.root.carbon*hfrac*param.rootreduction*pft->nind*stand->frac;
      stand->soil.litter.bg[pft->litter].nitrogen+=grass->ind.root.nitrogen*hfrac*param.rootreduction*pft->nind*pft->par->fn_turnover;
      output->alittfall.nitrogen+=grass->ind.root.nitrogen*hfrac*param.rootreduction*pft->nind*stand->frac*pft->par->fn_turnover;
      pft->bm_inc.nitrogen+=grass->ind.root.nitrogen*hfrac*param.rootreduction*pft->nind*pft->nind*(1-pft->par->fn_turnover);

      grass->ind.root.carbon*=(1-hfrac*param.rootreduction);
      grass->ind.root.nitrogen*=(1-hfrac*param.rootreduction);
    }

    rotation->grazing_days -= 1;
    if (rotation->grazing_days == 0)
      rotation->rotation_mode = RM_RECOVERY;
  }
  else if (rotation->rotation_mode == RM_RECOVERY)
  {
    rotation->recovery_days -= 1;
    if (rotation->recovery_days == 0)
      rotation->rotation_mode = RM_UNDEFINED;
  }
  return sum;
} /* of 'harvest_grass_grazing_int' */


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
  output->flux_harvest.carbon+=(harvest.harvest.carbon+harvest.residual.carbon)*stand->frac;
  output->flux_harvest.nitrogen+=(harvest.harvest.nitrogen+harvest.residual.nitrogen)*stand->frac;
  output->dcflux+=(harvest.harvest.carbon+harvest.residual.carbon)*stand->frac;
  stand->growing_days=0;
  return harvest;
}
 /* of 'harvest_stand' */

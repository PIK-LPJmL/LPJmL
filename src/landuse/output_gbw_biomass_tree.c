/**************************************************************************************/
/**                                                                                \n**/
/**          o u t p u t _ g b w _ b i o m a s s _ t r e e . c                     \n**/
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
#include "agriculture.h"
#include "biomass_tree.h"

void output_gbw_biomass_tree(Output *output,      /**< output data */
                             const Stand *stand,  /**< stand pointer */
                             Real frac_g_evap,
                             Real evap,           /**< evaporation (mm) */
                             Real evap_blue,      /**< evaporation of irrigation water (mm) */
                             Real return_flow_b,  /**< irrigation return flows from surface runoff, lateral runoff and percolation (mm) */
                             const Real aet_stand[LASTLAYER],
                             const Real green_transp[LASTLAYER],
                             Real intercep_stand,  /**< stand interception (mm) */
                             Real intercep_stand_blue, /**< stand interception from irrigation (mm) */
                             int ncft,            /**< number of CROPS */
                             Bool pft_output_scaled
                            )
{
  int l;
  Real total_g,total_b;
  Irrigation *data;
  data=stand->data;
  total_g=total_b=0;

  total_g+=intercep_stand-intercep_stand_blue;
  total_b+=intercep_stand_blue;

  total_g+=evap*frac_g_evap;
  total_b+=evap_blue;
  forrootsoillayer(l)
  {
    total_g+=green_transp[l];
    total_b+=aet_stand[l]-green_transp[l];
  }
  if(pft_output_scaled)
  {
    output->cft_consump_water_g[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=total_g*stand->cell->ml.landfrac[data->irrigation].biomass_tree;
    output->cft_consump_water_b[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=total_b*stand->cell->ml.landfrac[data->irrigation].biomass_tree;
    forrootsoillayer(l)
    {
      output->cft_transp[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=aet_stand[l]*stand->cell->ml.landfrac[data->irrigation].biomass_tree;
      output->cft_transp_b[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=(aet_stand[l]-green_transp[l])*stand->cell->ml.landfrac[data->irrigation].biomass_tree;
    }

    output->cft_evap[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=evap*stand->cell->ml.landfrac[data->irrigation].biomass_tree;
    output->cft_evap_b[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=evap_blue*stand->cell->ml.landfrac[data->irrigation].biomass_tree;
    output->cft_interc[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=intercep_stand*stand->cell->ml.landfrac[data->irrigation].biomass_tree;
    output->cft_interc_b[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=intercep_stand_blue*stand->cell->ml.landfrac[data->irrigation].biomass_tree;
    output->cft_return_flow_b[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=return_flow_b*stand->cell->ml.landfrac[data->irrigation].biomass_tree;
  }
  else
  {
    output->cft_consump_water_g[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=total_g;
    output->cft_consump_water_b[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=total_b;
    forrootsoillayer(l)
    {
      output->cft_transp[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=aet_stand[l];
      output->cft_transp_b[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=aet_stand[l]-green_transp[l];
    }

    output->cft_evap[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=evap;
    output->cft_evap_b[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=evap_blue;
    output->cft_interc[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=intercep_stand;
    output->cft_interc_b[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=intercep_stand_blue;
    output->cft_return_flow_b[rbtree(ncft)+data->irrigation*(ncft+NGRASS+NBIOMASSTYPE)]+=return_flow_b;
  }

  if(data->irrigation)
  {
    output->mgcons_irr+=total_g*stand->cell->ml.landfrac[1].biomass_tree;
    output->mbcons_irr+=total_b*stand->cell->ml.landfrac[1].biomass_tree;
  }
  else
  {
    output->mgcons_rf+=total_g*stand->cell->ml.landfrac[0].biomass_tree;
    output->mgcons_rf+=total_b*stand->cell->ml.landfrac[0].biomass_tree;
  }
} /* of 'output_gbw_biomass_tree' */


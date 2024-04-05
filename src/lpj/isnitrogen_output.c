/**************************************************************************************/
/**                                                                                \n**/
/**      i  s  n  i  t  r  o  g  e  n  _  o  u  t  p  u  t  .  c                   \n**/
/**                                                                                \n**/
/** Function determines whether output is nitrogen output                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool isnitrogen_output(int index /**< index for output file */
                      )          /** \return output is nitrogen output (TRUE/FALSE) */
{
  switch(index)
  {
    case CFT_ABOVEGBMN: case PFT_HARVESTN: case PFT_RHARVESTN: case RHARVEST_BURNTN:
    case RHARVEST_BURNT_IN_FIELDN: case FLUX_ESTABN: case VEGN: case SOILN: case LITN:
    case SOILNH4: case SOILNO3: case HARVESTN: case SOILN_LAYER: case SOILN_SLOW:
    case PFT_NUPTAKE: case NUPTAKE: case LEACHING: case N2O_DENIT: case N2O_NIT:
    case N2_EMIS: case BNF: case PFT_NDEMAND: case N_MINERALIZATION: case FIREN:
    case N_IMMO: case PFT_NLEAF: case PFT_VEGN: case NEGN_FLUXES: case PFT_NROOT:
    case PFT_NSAPW: case PFT_NHAWO: case PFT_HARVESTN2: case PFT_RHARVESTN2:
    case CFT_ABOVEGBMN2: case N_VOLATILIZATION: case PFT_NLIMIT: case SOILNO3_LAYER:
    case SOILNH4_LAYER: case LITFALLN: case LITFALLN_WOOD: case MGRASS_SOILN:
    case MGRASS_LITN: case LITFALLN_AGR: case HARVESTN_AGR: case NFERT_AGR:
    case NMANURE_AGR: case NDEPO_AGR: case NMINERALIZATION_AGR: case NIMMOBILIZATION_AGR:
    case NUPTAKE_AGR: case NLEACHING_AGR: case N2O_DENIT_AGR: case N2O_NIT_AGR:
    case NH3_AGR: case N2_AGR: case PFT_BNF: case TIMBER_HARVESTN: case PRODUCT_POOL_FAST_N:
    case PRODUCT_POOL_SLOW_N: case DEFOREST_EMIS_N: case PROD_TURNOVER_N: case FLUX_FIREWOOD_N:
    case ESTAB_STORAGE_N: case FLUX_ESTABN_MG: case BNF_MG: case NAPPLIC_MG: case NDEPO_MG:
      return TRUE;
    default:
      return FALSE;
  }
} /* of 'isnitrogen_output' */ 

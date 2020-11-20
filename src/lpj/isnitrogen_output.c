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
    case D_NUPTAKE: case D_N2O_DENIT: case D_N2O_NIT: case D_N2_DENIT: case D_LEACHING:
    case D_BNF: case D_NLEAF: case D_NROOT: case D_NSO: case D_NPOOL: case D_NO3:
    case D_NH4: case D_NSOIL_SLOW: case D_NSOIL_FAST: case D_NLIMIT: case D_VSCAL:
    case CFT_ABOVEGBMN: case PFT_HARVESTN: case PFT_RHARVESTN: case RHARVEST_BURNTN:
    case RHARVEST_BURNT_IN_FIELDN: case FLUX_ESTABN: case VEGN: case SOILN: case LITN:
    case SOILNH4: case SOILNO3: case HARVESTN: case SOILN_LAYER: case SOILN_SLOW:
    case PFT_NUPTAKE: case MNUPTAKE: case MLEACHING: case MN2O_DENIT: case MN2O_NIT:
    case MN2_EMIS: case MBNF: case PFT_NDEMAND: case MN_MINERALIZATION: case FIREN:
    case MN_IMMO: case PFT_NLEAF: case PFT_VEGN: case NEGN_FLUXES: case PFT_NROOT:
    case PFT_NSAPW: case PFT_NHAWO: case PFT_HARVESTN2: case PFT_RHARVESTN2:
    case CFT_ABOVEGBMN2: case MN_VOLATILIZATION: case PFT_NLIMIT: case SOILNO3_LAYER:
    case SOILNH4_LAYER: case ALITFALLN:
      return TRUE;
    default:
      return FALSE;
  }
} /* of 'isnitrogen_output' */ 

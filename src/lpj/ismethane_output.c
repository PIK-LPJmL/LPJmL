/**************************************************************************************/
/**                                                                                \n**/
/**         i  s  m  e  t  h  a  n  e  _  o  u  t  p  u  t  .  c                   \n**/
/**                                                                                \n**/
/** Function determines whether output is methane output                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool ismethane_output(int index /**< index for output file */
                     )          /** \return output is methane output (TRUE/FALSE) */
{
  switch(index)
  {
     case PCH4: case METHANEC_MGRASS: case CH4_OXIDATION: case CH4_EMISSIONS:
     case CH4_SINK: case CH4_AGR: case MEANSOILO2: case MEANSOILCH4: case CH4_EBULLITION:
     case CH4_PLANT_GAS: case CH4_RICE_EM: case CH4_EMISSIONS_WET: case METHANOGENESIS:
     case CH4_LITTER: case CH4_GRASSLAND:
      return TRUE;
    default:
      return FALSE;
  }
} /* of 'ismethane_output' */ 

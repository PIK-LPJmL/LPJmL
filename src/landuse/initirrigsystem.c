/**************************************************************************************/
/**                                                                                \n**/
/**                  i  n  i  t  i  r  r  i  g  s  y  s  t  e  m  .  c             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** Function initializes irrigation system struct to a specified value             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void initirrigsystem(Irrig_system *irrig_system, /**< pointer to irrigation system */
                     IrrigationType irrig,       /**< irrigation type to initialize with */
                     int ncft,                   /**< number of crop PFTs */
                     int nagtree                 /**< number of agricultural trees */
                    )
{
  int j;
  for(j=0;j<ncft;j++)
    irrig_system->crop[j]=irrig;
  for(j=0;j<NGRASS;j++)
    irrig_system->grass[j]=irrig;
  for(j=0;j<nagtree;j++)
    irrig_system->ag_tree[j]=irrig;
  irrig_system->biomass_grass=irrig_system->biomass_tree=irrig;
  irrig_system->woodplantation = irrig;
} /* of 'initirrigsystem' */

/**************************************************************************************/
/**                                                                                \n**/
/**            a  g  r  i  c  u  l  t  u  r  e    _  t  r  e  e  .  c              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Definition of agricultural tree stand                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "natural.h"
#include "agriculture.h"
#include "tree.h"
#include "agriculture_tree.h"

Standtype agriculture_tree_stand={AGRICULTURE_TREE,"agriculture_tree",
                                  new_biomass_tree,
                                  free_agriculture,fwrite_biomass_tree,
                                  fread_biomass_tree,fprint_biomass_tree,
                                  daily_agriculture_tree,
                                  annual_agriculture_tree,NULL,NULL};

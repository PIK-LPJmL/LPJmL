/**************************************************************************************/
/**                                                                                \n**/
/**                    b  i  o  m  a  s  s  _  t  r  e  e  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Definition of biomass tree stand                                           \n**/
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

Standtype biomass_tree_stand={BIOMASS_TREE,"biomass_tree",new_biomass_tree,
                              free_agriculture,fwrite_biomass_tree,
                              fread_biomass_tree,fprint_biomass_tree,
                              daily_biomass_tree,annual_biomass_tree,NULL,NULL};

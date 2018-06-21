/**************************************************************************************/
/**                                                                                \n**/
/**                    b  i  o  m  a  s  s  _  t  r  e  e  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Definition of biomass stand                                                \n**/
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
#include "biomass_tree.h"

Standtype biomass_tree_stand={BIOMASS_TREE,"biomass_tree",new_agriculture,
                              free_agriculture,fwrite_agriculture,
                              fread_agriculture,fprint_agriculture,
                              daily_biomass_tree,annual_biomass_tree,NULL};

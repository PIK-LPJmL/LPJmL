/**************************************************************************************/
/**                                                                                \n**/
/**              n  e  w  _  b  i  o  m  a  s  s  _  t  r  e  e  .  c              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function allocates biomass tree data of stand                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "biomass_tree.h"

void new_biomass_tree(Stand *stand)
{
  Biomass_tree *biomass_tree;
  biomass_tree=new(Biomass_tree);
  check(biomass_tree);
  stand->fire_sum=0.0;
  stand->growing_days=0;
  stand->data=biomass_tree;
  biomass_tree->growing_time=biomass_tree->age=1;
  init_irrigation(&biomass_tree->irrigation);
} /* of 'new_biomass_tree' */

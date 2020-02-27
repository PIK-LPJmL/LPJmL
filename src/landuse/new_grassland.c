/**************************************************************************************/
/**                                                                                \n**/
/**              n  e  w  _  g  r  a  s  s  l  a  n  d  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function allocates grassland data of stand                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grassland.h"

void new_grassland(Stand *stand)
{
  Grassland *grassland;
  grassland=new(Grassland);
  check(grassland);
  stand->fire_sum=0.0;
  stand->growing_days=0;
  stand->data=grassland;
  init_irrigation(&grassland->irrigation);
  grassland->nr_of_lsus_ext= grassland->nr_of_lsus_int=0.0;
  grassland->rotation.grazing_days=grassland->rotation.paddocks=0;
  grassland->rotation.recovery_days=0;
  grassland->rotation.mode=RM_UNDEFINED;
} /* of 'new_grassland' */

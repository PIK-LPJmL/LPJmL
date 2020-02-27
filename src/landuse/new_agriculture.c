/**************************************************************************************/
/**                                                                                \n**/
/**              n  e  w  _  a  g  r  i  c  u  l  t  u  r  e  .  c                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function allocates irrigation data of stand                                \n**/
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

void new_agriculture(Stand *stand)
{
  Irrigation *irrigation;
  irrigation=new(Irrigation);
  check(irrigation);
  stand->fire_sum=0.0;
  stand->growing_days=0;
  stand->data=irrigation;
  init_irrigation(irrigation);
} /* of 'new_agriculture' */

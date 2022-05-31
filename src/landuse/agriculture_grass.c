/**************************************************************************************/
/**                                                                                \n**/
/**            a  g  r  i  c  u  l  t  u  r  e  _  g  r  a  s  s  . c              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Definition of agricultural, setaside and kill stand                        \n**/
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
#include "grassland.h"
#include "agriculture_grass.h"

Standtype agriculture_grass_stand={AGRICULTURE_GRASS,"agriculture_grass",
                                   new_agriculture,
                                   free_agriculture,fwrite_agriculture,
                                   fread_agriculture,fprint_agriculture,
                                   daily_agriculture_grass,annual_agriculture_grass,NULL,isdailyoutput_agriculture};

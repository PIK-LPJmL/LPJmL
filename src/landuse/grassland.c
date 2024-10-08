/**************************************************************************************/
/**                                                                                \n**/
/**                     g  r  a  s  s  l  a  n  d  .  c                            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Definition of grassland stand                                              \n**/
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

char *grassland_names[NGRASS]={"others","grassland"};

Standtype grassland_stand={GRASSLAND,"grassland",new_grassland,
                           free_agriculture,fwrite_grassland,
                           fread_grassland,fprint_grassland,
                           daily_grassland,annual_grassland,dailyfire};

Standtype others_stand={OTHERS,"others",new_grassland,
                        free_agriculture,fwrite_grassland,
                        fread_grassland,fprint_grassland,
                        daily_grassland,annual_grassland,dailyfire};

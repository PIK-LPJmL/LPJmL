/**************************************************************************************/
/**                                                                                \n**/
/**                             u  r  b  a  n  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Definition of urban stand                                                  \n**/
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
#include "urban.h"

Standtype urban_stand={URBAN,"urban",{0.0,0.0},0,new_agriculture,
                       free_agriculture,fwrite_agriculture,
                       fread_agriculture,fprint_agriculture,
                       daily_natural,annual_agriculture,
                       NULL};

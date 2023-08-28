/**************************************************************************************/
/**                                                                                \n**/
/**                     w  e  t  l  a  n  d  .  c                                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Definition of wetland stand                                                \n**/
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
#include "wetland.h"
#include "agriculture.h"

Standtype wetland_stand = {WETLAND,"wetland",new_natural,free_natural,
                           fwrite_natural,fread_natural,fprint_natural,
                           daily_natural,annual_natural,dailyfire};
Standtype setaside_wetland = {SETASIDE_WETLAND,"wetland setaside",new_agriculture,
                              free_agriculture,fwrite_agriculture,
                              fread_agriculture,fprint_agriculture,
#ifdef DAILY_ESTABLISHMENT
        daily_setaside,
#else
        daily_natural,
#endif
        annual_setaside,NULL};

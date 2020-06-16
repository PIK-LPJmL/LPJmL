/**************************************************************************************/
/**                                                                                \n**/
/**                    a  g  r  i  c  u  l  t  u  r  e  .  c                       \n**/
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
#include "natural.h"
#include "agriculture.h"

Standtype setaside_rf_stand={SETASIDE_RF,"setaside_rf",new_agriculture,
                             free_agriculture,fwrite_agriculture,
                             fread_agriculture,fprint_agriculture,
#ifdef DAILY_ESTABLISHMENT
                             daily_setaside,
#else
                             daily_natural,
#endif
                             annual_setaside,NULL};

Standtype setaside_ir_stand={SETASIDE_IR,"setaside_ir",new_agriculture,
                             free_agriculture,fwrite_agriculture,
                             fread_agriculture,fprint_agriculture,
#ifdef DAILY_ESTABLISHMENT
                             daily_setaside,
#else
                             daily_natural,
#endif
                             annual_setaside,NULL};

Standtype managedforest_stand={MANAGEDFOREST,"managed forest",NULL,NULL,NULL,
                               NULL,NULL,NULL,NULL,NULL};

Standtype kill_stand={KILL,"kill",NULL,free_agriculture,NULL,NULL,NULL,NULL,NULL,
                      NULL};

Standtype agriculture_stand={AGRICULTURE,"agriculture",new_agriculture,
                             free_agriculture,fwrite_agriculture,
                             fread_agriculture,fprint_agriculture,
                             daily_agriculture,annual_agriculture,NULL};

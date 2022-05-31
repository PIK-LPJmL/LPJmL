/**************************************************************************************/
/**                                                                                \n**/
/**                    w o o d p l a n t a t i o n . c                             \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Definition of woodplantation stand                                         \n**/
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
#include "woodplantation.h"

const char *woodplantation_names[NWPTYPE]={"woodplantation"};

Standtype woodplantation_stand={WOODPLANTATION,"woodplantation",new_biomass_tree,
                                free_agriculture,fwrite_biomass_tree,
                                fread_biomass_tree,fprint_biomass_tree,
                                daily_woodplantation,annual_woodplantation,NULL};

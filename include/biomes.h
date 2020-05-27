/**************************************************************************************/
/**                                                                                \n**/
/**                      b  i  o  m  e  s  .  h                                    \n**/
/**                                                                                \n**/
/**     extension of LPJ to classify biomes based on FPC values only               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef BIOMES_H /* already included? */
#define BIOMES_H

/* definition of constants for biomes */

#define MANAGED_LAND 1
#define IJS 7  // ICE was already used
#define TUNDRA 8
#define WOODED_TUNDRA 9
#define BOREAL_FOREST 10
#define COOL_CONIFER_FOREST 11
#define TEMPERATE_MIXED_FOREST 12
#define TEMPERATE_DECIDUOUS_FOREST 13
#define WARM_MIXED_FOREST 14
#define GRASSLAND_STEPPE 15
#define HOT_DESERT 16
#define SCRUBLAND 17
#define SAVANNAH 18
#define TROPICAL_WOODLAND 19
#define TROPICAL_FOREST 20


/* Declaration of functions */

extern int biome_classification(Real,Real,const  Stand *,int);

#endif /*BIOMES_H*/

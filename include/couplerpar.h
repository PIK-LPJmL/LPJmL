/**************************************************************************************/
/**                                                                                \n**/
/**                      c  o  u  p  l  e  r  p  a  r  .  h                        \n**/
/**                                                                                \n**/
/**     Definition of coupler input indices                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/


#ifndef COUPLERPAR_H /* already included? */
#define COUPLERPAR_H

/* List of input data streams */

#define N_IN 27         /* Number of available input data streams */

#define CLOUD_DATA 0
#define TEMP_DATA 1
#define PREC_DATA 2
#define SWDOWN_DATA 3
#define LWNET_DATA 4
#define CO2_DATA 5
#define LANDUSE_DATA 6
#define TILLAGE_DATA 7
#define RESIDUE_DATA 8
#define TMIN_DATA 9
#define TMAX_DATA 10
#define TAMP_DATA 11
#define WET_DATA 12
#define BURNTAREA_DATA 13
#define HUMID_DATA 14
#define WIND_DATA 15
#define NH4_DATA 16
#define NO3_DATA 17
#define FERTILIZER_DATA 18
#define MANURE_DATA 19
#define WATERUSE_DATA 20
#define POPDENS_DATA 21
#define HUMAN_IGNITION_DATA 22
#define LIGHTNING_DATA 23
#define GRASSLAND_LSUHA 24
#define SDATE_DATA 25
#define CROP_PHU_DATA 26

#endif /* COUPLERPAR_H */

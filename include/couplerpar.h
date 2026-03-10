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

#define N_IN 26         /* Number of available input data streams */

#define TEMP_DATA 0
#define PREC_DATA 1
#define SWDOWN_DATA 2
#define LWNET_DATA 3
#define CO2_DATA 4
#define LANDUSE_DATA 5
#define TILLAGE_DATA 6
#define RESIDUE_DATA 7
#define TMIN_DATA 8
#define TMAX_DATA 9
#define TAMP_DATA 10
#define WET_DATA 11
#define BURNTAREA_DATA 12
#define HUMID_DATA 13
#define WIND_DATA 14
#define NH4_DATA 15
#define NO3_DATA 16
#define FERTILIZER_DATA 17
#define MANURE_DATA 18
#define WATERUSE_DATA 19
#define POPDENS_DATA 20
#define HUMAN_IGNITION_DATA 21
#define LIGHTNING_DATA 22
#define GRASSLAND_LSUHA 23
#define SDATE_DATA 24
#define CROP_PHU_DATA 25

#endif /* COUPLERPAR_H */

/**************************************************************************************/
/**                                                                                \n**/
/**                      c  o  p  a  n  .  h                                       \n**/
/**                                                                                \n**/
/**     Declaration of COPAN coupler functions and related datatypes               \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/


#ifndef COPAN_H /* already included? */
#define COPAN_H

/* Definition of constants */

#define COPAN_COUPLER_VERSION 1        /* Protocol version */
#define LPJCOPAN "LPJCOPAN"            /* Environment variable for COPAN coupler */
#define DEFAULT_COPAN_HOST "localhost" /* Default host for COPAN model */
#define DEFAULT_COPAN_PORT 2224        /* Default port for in and outgoing connection */

/* List of tokens */

#define GET_DATA 0      /* Receiving data from COPAN */
#define PUT_DATA 1      /* Sending data to COPAN */
#define GET_DATA_SIZE 2 /* Receiving data size from COPAN */
#define PUT_DATA_SIZE 3 /* Sending data size to COPAN */
#define END_DATA 4      /* Ending communication */

#define N_IN 22         /* Number of available input data streams */
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

/* Declaration of functions */

extern Bool open_copan(Config *);
extern void close_copan(const Config *);
extern Bool receive_real_copan(int,Real *,int,int,const Config *);
extern Bool receive_real_scalar_copan(Real *,int,const Config *);
extern Bool receive_int_copan(int,int *,int,int,const Config *);
extern void send_flux_copan(const Flux *,Real,int,const Config *);
extern void send_token_copan(int,int,const Config *);

#endif /* COPAN_H */

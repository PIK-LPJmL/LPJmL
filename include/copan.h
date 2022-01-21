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

#include "copanpar.h"

/* Definition of constants */

#define COPAN_COUPLER_VERSION 2        /* Protocol version */
#define LPJCOPAN "LPJCOPAN"            /* Environment variable for COPAN coupler */
#define LPJWAIT "LPJWAIT"              /* Time to wait for connection */
#define DEFAULT_COPAN_HOST "localhost" /* Default host for COPAN model */
#define DEFAULT_COPAN_PORT 2224        /* Default port for in and outgoing connection */
#define DEFAULT_WAIT 0                 /* Default time to wait for connection */

/* List of tokens */

#define GET_DATA 0      /* Receiving data from COPAN */
#define PUT_DATA 1      /* Sending data to COPAN */
#define GET_DATA_SIZE 2 /* Receiving data size from COPAN */
#define PUT_DATA_SIZE 3 /* Sending data size to COPAN */
#define END_DATA 4      /* Ending communication */

/* List of input data streams */

#define N_IN 24         /* Number of available input data streams */

/* Declaration of functions */

extern Bool open_copan(Config *);
extern void close_copan(const Config *);
extern Bool receive_real_copan(int,Real *,int,int,const Config *);
extern Bool receive_real_scalar_copan(Real *,int,const Config *);
extern Bool receive_int_copan(int,int *,int,int,const Config *);
extern Bool send_flux_copan(const Flux *,Real,int,const Config *);
extern Bool send_token_copan(int,int,const Config *);

#endif /* COPAN_H */

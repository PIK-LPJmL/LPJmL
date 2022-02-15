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

#define COPAN_COUPLER_VERSION 3        /* Protocol version */
#define LPJCOPAN "LPJCOPAN"            /* Environment variable for COPAN coupler */
#define LPJWAIT "LPJWAIT"              /* Time to wait for connection */
#define DEFAULT_COPAN_HOST "localhost" /* Default host for COPAN model */
#define DEFAULT_COPAN_PORT 2224        /* Default port for in and outgoing connection */
#define DEFAULT_WAIT 0                 /* Default time to wait for connection */

#define COPAN_OK 0                     /* COPAN status ok */
#define COPAN_ERR -1                   /* error occurred in COPAN */

/* List of tokens */

typedef enum
{
  GET_DATA,      /* Receiving data from COPAN */
  PUT_DATA,      /* Sending data to COPAN */
  GET_DATA_SIZE, /* Receiving data size from COPAN */
  PUT_DATA_SIZE, /* Sending data size to COPAN */
  END_DATA,      /* Ending communication */
  GET_STATUS,    /* Check status of COPAN */
  FAIL_DATA,     /* Ending communication on error */
  PUT_INIT_DATA  /* Send init data to COPAN */
} Token;


extern char *token_names[];

/* Declaration of functions */

extern Bool open_copan(Config *);
extern Socket *connect_copan(int,int);
extern int check_copan(Config *);
extern void close_copan(Bool,const Config *);
extern Bool receive_copan(int,void *,Type,int,int,const Config *);
extern Bool receive_real_copan(int,Real *,int,int,const Config *);
extern Bool receive_scalar_copan(int,void *,Type,int,int,const Config *);
extern Bool send_scalar_copan(int,const void *,Type,int,int,const Config *);
extern Bool receive_real_scalar_copan(int,Real *,int,int,const Config *);
extern Bool send_real_scalar_copan(int,const Real *,int,Real,int,const Config *);
extern Bool send_token_copan(Token,int,const Config *);
extern Bool receive_token_copan(Socket *,Token *,int *);
extern Bool openinput_copan(int,Type,int,int *,const Config *);
extern Bool openoutput_copan(int,int,int,int,Type,const Config *);
extern void send_output_copan(int,int,int,const Config *);

/* Definitions of macros */

#define receive_int_copan(index,data,size,year,config) receive_copan(index,data,LPJ_INT,size,year,config)
#define send_flux_copan(flux,scalar,year,config) send_real_scalar_copan(GLOBALFLUX,(Real *)flux,sizeof(Flux)/sizeof(Real),scalar,year,config)

#endif /* COPAN_H */

/**************************************************************************************/
/**                                                                                \n**/
/**                      c  o  u  p  l  e  r  .  h                                 \n**/
/**                                                                                \n**/
/**     Declaration of coupler functions and related datatypes                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef COUPLER_H /* already included? */
#define COUPLER_H

#include "couplerpar.h"

/* Definition of constants */

#define COUPLER_VERSION 3                /* Protocol version */
#define LPJCOUPLEDHOST "LPJCOUPLEDHOST"  /* Environment variable for coupled host */
#define LPJWAIT "LPJWAIT"                /* Time to wait for connection */
#define DEFAULT_COUPLED_HOST "localhost" /* Default host for coupled model */
#define DEFAULT_COUPLER_PORT 2224        /* Default port for in and outgoing connection */
#define DEFAULT_WAIT 0                   /* Default time to wait for connection */

#define COUPLER_OK 0                     /* coupler status ok */
#define COUPLER_ERR -1                   /* error occurred in coupler */

/* List of tokens */

typedef enum
{
  GET_DATA,      /* Receiving data from coupled model */
  PUT_DATA,      /* Sending data to coupled model */
  GET_DATA_SIZE, /* Receiving data size from coupled model */
  PUT_DATA_SIZE, /* Sending data size to coupled model */
  END_DATA,      /* Ending communication */
  GET_STATUS,    /* Check status of coupled model */
  FAIL_DATA,     /* Ending communication on error */
  PUT_INIT_DATA  /* Send init data to coupled model */
} Token;


extern char *token_names[];

/* Declaration of functions */

extern Bool open_coupler(Config *);
extern Socket *connect_coupler(int,int);
extern int check_coupler(Config *);
extern void close_coupler(Bool,const Config *);
extern Bool receive_coupler(int,void *,Type,int,int,const Config *);
extern Bool receive_real_coupler(int,Real *,int,int,const Config *);
extern Bool receive_scalar_coupler(int,void *,Type,int,int,const Config *);
extern Bool send_scalar_coupler(int,const void *,Type,int,int,const Config *);
extern Bool receive_real_scalar_coupler(int,Real *,int,int,const Config *);
extern Bool send_real_scalar_coupler(int,const Real *,int,Real,int,const Config *);
extern Bool send_token_coupler(Token,int,const Config *);
extern Bool receive_token_coupler(Socket *,Token *,int *);
extern Bool openinput_coupler(int,Type,int,int *,const Config *);
extern Bool openoutput_coupler(int,int,int,int,Type,const Config *);
extern void send_output_coupler(int,int,int,const Config *);

/* Definitions of macros */

#define receive_int_coupler(index,data,size,year,config) receive_coupler(index,data,LPJ_INT,size,year,config)
#define send_flux_coupler(flux,scalar,year,config) send_real_scalar_coupler(GLOBALFLUX,(Real *)flux,sizeof(Flux)/sizeof(Real),scalar,year,config)
#define send_conc_coupler(id,value,year,config) send_real_scalar_coupler(id,value,1,1.0,year,config)

#endif /* COUPLER_H */

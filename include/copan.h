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

#define COPAN_COUPLER_VERSION 1
#define LPJCOPAN "LPJCOPAN"            /* Environment variable for COPAN coupler */
#define DEFAULT_COPAN_HOST "localhost" /* Default host for COPAN model */
#define DEFAULT_COPAN_PORT 2224        /* Default port for in and outgoing connection */

extern Bool open_copan(Config *);
extern void close_copan(const Config *);
extern Bool receive_real_copan(Real *,int,const Config *);
extern Bool receive_real_scalar_copan(Real *,int,const Config *);
extern Bool receive_int_copan(int *,int,const Config *);
extern void send_flux_copan(const Flux *,Real,int,const Config *);

#endif /* COPAN_H */

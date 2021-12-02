/**************************************************************************************/
/**                                                                                \n**/
/**                      i  m  a  g  e  .  h                                       \n**/
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

/* Definition of constants for biomass pools used in COPAN */

#define LPJCOPAN "LPJCOPAN"  /* Environment variable for COPAN coupler */
#define LPJWAITCOPAN "LPJWAITCOPAN"  /* time to wait for IMAGE connection */
#define DEFAULT_COPAN_HOST "localhost" /* Default host for COPAN model */
#define DEFAULT_COPAN_PORT 2224 /* Default port for outgoing connection */
#define WAIT_COPAN 12000             /* wait for COPAN connection (sec) */
extern Bool open_copan(Config *);
extern void close_copan(const Config *);
extern Bool receive_copan(Real *,int,const Config *);

#endif /* COPAN_H */

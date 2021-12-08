/**************************************************************************************/
/**                                                                                \n**/
/**                s  e  n  d  _  f  l  u  x  _  c  o  p  a  n  .  c               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function writes fluxes into socket                                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void send_flux_copan(const Flux *flux,    /**< Carbon and water fluxes */
                     Real scale,          /**< scaling factor */
                     int year,            /**< Simulation year (AD) */
                     const Config *config /**< LPJ configuration */
                    )
{
  float *vec;
  size_t i;
  send_token_copan(PUT_DATA,GLOBALFLUX,config);
  writeint_socket(config->socket,&year,1);
  vec=newvec(float,sizeof(Flux)/sizeof(Real));
  check(vec);
  for(i=0;i<sizeof(Flux)/sizeof(Real);i++)
    vec[i]=(float)(((const Real *)flux)[i]*scale);
  writefloat_socket(config->socket,vec,sizeof(Flux)/sizeof(Real));
  free(vec);
} /* of 'send_flux_copan' */

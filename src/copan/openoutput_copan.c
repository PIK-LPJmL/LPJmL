/**************************************************************************************/
/**                                                                                \n**/
/**            o  p  e  n  o  u  t  p  u  t  _  c  o  p  a  n  .  c                \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with COPAN                           \n**/
/**     Opens output stream to COPAN model                                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool openoutput_copan(int id,              /**< id of output stream */
                      int ncell,           /**< number of cells */
                      int nstep,           /**< number of time steps per year */
                      int nbands,          /**< number of bands */
                      Type type,           /**< datatype of stream */
                      const Config *config /**< LPJmL configuration */
                     )                     /** \return TRUE on error */
{
  int status;
  send_token_copan(PUT_DATA_SIZE,id,config);
#ifdef DEBUG_COPAN
  printf("Sending ncell=%d, nstep=%d, nbands=%d, type=%s",ncell,nstep,nbands,typenames[type]);
  fflush(stdout);
#endif
#if COPAN_COUPLER_VERSION == 4
  writeint_socket(config->socket,&ncell,1);
#endif
  writeint_socket(config->socket,&nstep,1);
  writeint_socket(config->socket,&nbands,1);
  writeint_socket(config->socket,&type,1);
#ifdef DEBUG_COPAN
  printf(", done.\nReceiving status");
  fflush(stdout);
#endif
  readint_socket(config->socket,&status,1);
#ifdef DEBUG_COPAN
  printf(", %d received.\n",status);
  fflush(stdout);
#endif
  return status!=COPAN_OK;
} /* of 'openoutput_copan' */

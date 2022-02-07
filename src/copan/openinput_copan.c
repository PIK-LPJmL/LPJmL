/**************************************************************************************/
/**                                                                                \n**/
/**            o  p  e  n  i  n  p  u  t  _  c  o  p  a  n  .  c                   \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with COPAN                           \n**/
/**     Opens input stream to COPAN model                                          \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool openinput_copan(int id,              /**< id of input stream */
                     Type type,           /**< datatype of stream */
                     int ncell,           /**< number of cells */
                     int *nbands,         /**< number of bands */
                     const Config *config /**< LPJmL configuration */
                    )                     /** \return TRUE on error */
{
  if(isroot(*config))
  {
    send_token_copan(GET_DATA_SIZE,id,config);
#ifdef DEBUG_COPAN
    printf("Sending type=%s",typenames[type]);
    fflush(stdout);
#endif
    writeint_socket(config->socket,&type,1);
#ifdef DEBUG_COPAN
    printf(", done.\n");
    fflush(stdout);
#endif
#if COPAN_COUPLER_VERSION == 4
#ifdef DEBUG_COPAN
    printf("Sending ncell=%d",ncell);
#endif
    writeint_socket(config->socket,&ncell,1);
#ifdef DEBUG_COPAN
    printf(", done.\n");
    fflush(stdout);
#endif
#endif
#ifdef DEBUG_COPAN
    printf("Receiving nbands");
    fflush(stdout);
#endif
    readint_socket(config->socket,nbands,1);
#ifdef DEBUG_COPAN
    printf(", %d, received.\n",*nbands);
    fflush(stdout);
#endif
  }
#ifdef USE_MPI
  MPI_Bcast(nbands,1,MPI_INT,0,config->comm);
#endif
  if(isroot(*config) && *nbands==COPAN_ERR)
    fputs("ERROR311: Cannot open input socket stream.\n",stderr);
  return *nbands==COPAN_ERR;
} /* of 'openinput_copan' */

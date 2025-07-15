/**************************************************************************************/
/**                                                                                \n**/
/**          r e c e i v e _ s c a l a r _ c o u p l e r . c                       \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool receive_scalar_coupler(int index,           /**< index of input stream */
                            void *data,          /**< data read from socket */
                            Type type,           /**< datatype of data */
                            int size,            /**< size of data array */
                            int year,            /**< year */
                            const Config *config /**< LPJmL configuration */
                           )                     /** \return TRUE on error */
{
#if COUPLER_VERSION == 4
  int rc;
#endif
  if(isroot(*config))
  {
    send_token_coupler(GET_DATA,index,config);
    writeint_socket(config->socket,&year,1);
#if COUPLER_VERSION == 4
#ifdef DEBUG_COUPLER
    printf("Receiving status");
    fflush(stdout);
#endif
    readint_socket(config->socket,&rc,1);
#ifdef DEBUG_COUPLER
    printf(", %d received.\n",rc);
    fflush(stdout);
#endif
    if(rc!=COUPLER_OK)
      fprintf(stderr,"ERROR312: Cannot receive data from socket.\n");
#endif
  }
#if COUPLER_VERSION == 4
#ifdef USE_MPI
  MPI_Bcast(&rc,1,MPI_INT,0,config->comm);
#endif
  if(rc!=COUPLER_OK)
    return TRUE;
#endif
  if(isroot(*config))
  {
#ifdef DEBUG_COUPLER
    printf("Receiving %d %s values",size,typenames[type]);
    fflush(stdout);
#endif
    switch(type)
    {
      case LPJ_BYTE:
        read_socket(config->socket,data,size);
        break;
      case LPJ_SHORT:
        readshort_socket(config->socket,data,size);
        break;
      case LPJ_INT:
        readint_socket(config->socket,data,size);
        break;
      case LPJ_FLOAT:
        readfloat_socket(config->socket,data,size);
        break;
      case LPJ_DOUBLE:
        readdouble_socket(config->socket,data,size);
        break;
      default:
        return TRUE;
    }
#ifdef DEBUG_COUPLER
    printf(", done.\n");
    fflush(stdout);
#endif
  }
#ifdef USE_MPI
  MPI_Bcast(data,size*typesizes[type],MPI_BYTE,0,config->comm);
#endif
  return FALSE;
} /* of 'receive_scalar_coupler' */

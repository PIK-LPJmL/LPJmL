/**************************************************************************************/
/**                                                                                \n**/
/**           r  e  c  e  i  v  e  _  c  o  p  a  n  .  c                          \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with COPAN                           \n**/
/**     Receive data from socket and distribute to all tasks                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#ifdef USE_MPI
static MPI_Datatype mpi_types[]={MPI_BYTE,MPI_SHORT,MPI_INT,MPI_FLOAT,MPI_DOUBLE};
#endif

Bool receive_copan(int index,           /**< index of input file */
                   void *data,          /**< data received from socket */
                   Type type,           /**< type of data */
                   int size,            /**< number of items per cell */
                   int year,            /**< year (AD) */
                   const Config *config /**< LPJmL configuration */
                  )                     /** \return TRUE on error */
{
#ifdef USE_MPI
  int *counts;
  int *offsets;
#endif
  int rc;
  if(isroot(*config))
  {
    send_token_copan(GET_DATA,index,config);
#ifdef DEBUG_COPAN
    printf("Sending year %d",year);
    fflush(stdout);
#endif
    writeint_socket(config->socket,&year,1);
#ifdef DEBUG_COPAN
    printf(", done.\n");
    fflush(stdout);
#endif
#if COPAN_COUPLER_VERSION == 4
#ifdef DEBUG_COPAN
    printf("Receiving status");
    fflush(stdout);
#endif
    readint_socket(config->socket,&rc,1);
#ifdef DEBUG_COPAN
    printf(", %d received.\n",rc);
    fflush(stdout);
#endif
    if(rc!=COPAN_OK)
      fprintf(stderr,"ERROR312: Cannot receive data from socket.\n");
#endif
  }
#if COPAN_COUPLER_VERSION == 4
#ifdef USE_MPI
  MPI_Bcast(&rc,1,MPI_INT,0,config->comm);
#endif
  if(rc!=COPAN_OK)
    return TRUE;
#endif
#ifdef DEBUG_COPAN
  if(isroot(*config))
  {
    printf("Receiving %d %s values",config->nall*size,typenames[type]);
    fflush(stdout);
  }
#endif
#ifdef USE_MPI
  counts=newvec(int,config->ntask);
  check(counts);
  offsets=newvec(int,config->ntask);
  check(offsets);
  getcounts(counts,offsets,config->nall,size,config->ntask);
  if(mpi_read_socket(config->socket,data,mpi_types[type],config->nall*size,
                     counts,offsets,config->rank,config->comm))
  {
    free(offsets);
    free(counts);
    return TRUE;
  }
  free(offsets);
  free(counts);
#ifdef DEBUG_COPAN
  if(isroot(*config))
  {
    printf(", received.\n");
    fflush(stdout);
  }
#endif
  return FALSE;
#else
  switch(type)
  {
    case LPJ_BYTE:
      rc=read_socket(config->socket,data,config->nall*size);
      break;
    case LPJ_SHORT:
      rc=readshort_socket(config->socket,data,config->nall*size);
    case LPJ_INT:
      rc=readint_socket(config->socket,data,config->nall*size);
      break;
    case LPJ_FLOAT:
      rc=readfloat_socket(config->socket,data,config->nall*size);
      break;
    case LPJ_DOUBLE:
      rc=readdouble_socket(config->socket,data,config->nall*size);
      break;
  }
#ifdef DEBUG_COPAN
  printf(", received.\n");
  fflush(stdout);
#endif
  return rc;
#endif
} /* of 'receive_copan' */

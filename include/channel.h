/**************************************************************************************/
/**                                                                                \n**/
/**                     c  h  a  n  n  e  l  .  h                                  \n**/
/**                                                                                \n**/
/**     Header file for communication library using TCP/IP sockets                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#ifndef CHANNEL_H
#define CHANNEL_H

#ifdef USE_MPI
#include <mpi.h>
#endif
#ifdef _WIN32
#include <winsock.h> /* required for definition of SOCKET */
#endif

/* Definition of datatype */

typedef struct
{
#ifdef _WIN32
  SOCKET channel; /* Socket ID for Windows */
#else 
  int channel;    /* Socket ID */
#endif
  Bool swap;      /* Byte order has to be changed */
} Socket; 

/* Declarations of functions */

extern Socket *open_socket(int,int);
extern Socket *opentdt_socket(int,int);
extern Socket *connect_socket(const char *,int,int);
extern Socket *connecttdt_socket(const char *,int);
extern Bool write_socket(Socket *,const void *,int);
extern Bool read_socket(Socket *,void *,int);
extern Bool readdouble_socket(Socket *,double *,int);
extern Bool readfloat_socket(Socket *,float *,int);
extern Bool readint_socket(Socket *,int *,int);
extern Bool readlong_socket(Socket *,long long *,int);
extern Bool readshort_socket(Socket *,short *,int);
extern Bool writestring_socket(Socket *,const char *);
extern char *readstring_socket(Socket *);
extern void close_socket(Socket *);
extern char *getclientname(const Socket *);
#ifdef USE_MPI
extern Bool mpi_write_socket(Socket *,void *,MPI_Datatype,int,int [],
                            int [],int,MPI_Comm);
extern Bool mpi_read_socket(Socket *,void *,MPI_Datatype,int,int [],
                           int [],int,MPI_Comm);
#endif

/* Definition of macros */

#define writeshort_socket(socket,data,n) write_socket(socket,data,(n)*sizeof(short))
#define writeint_socket(socket,data,n) write_socket(socket,data,(n)*sizeof(int))
#define writelong_socket(socket,data,n) write_socket(socket,data,(n)*sizeof(long long))
#define writedouble_socket(socket,data,n) write_socket(socket,data,(n)*sizeof(double))
#define writefloat_socket(socket,data,n) write_socket(socket,data,(n)*sizeof(float))
#ifdef _WIN32
#define isinvalid_socket(socket) ((socket)==INVALID_SOCKET)
#else
#define isinvalid_socket(socket) ((socket)<0)
#endif

#endif /* CHANNEL_T */

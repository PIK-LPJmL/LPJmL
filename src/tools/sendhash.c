/**************************************************************************************/
/**                                                                                \n**/
/**                  s  e  n  d  h  a  s  h  .  c                                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function sends/receives content of hash using MPI                          \n**/
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

#define MSGTAG 102

typedef struct
{
  short id;
  Byte len;
} Data;

void sendhash(const Hash hash, /**< pointer to hash */
              int task,        /**< task id to send hash data */
              MPI_Comm comm    /**< MPI communicator */
             )
{
  /* Function sends hash to specified task */
  int i,count;
  Hashitem *items;
  Data data;
  count=gethashcount(hash);
  items=hash2array(hash);
  check(items);
  MPI_Send(&count,1,MPI_INT,task,MSGTAG,comm);
  for(i=0;i<count;i++)
  {
    data.id= *((short *)items[i].data);
    data.len=strlen(items[i].key);
    MPI_Send(&data,sizeof(data),MPI_BYTE,task,MSGTAG,comm);
    MPI_Send(items[i].key,data.len,MPI_BYTE,task,MSGTAG,comm);
  }
} /* of 'sendhash' */

void receivehash(Hash hash,    /**< pointer to hash */
                 int task,     /**< task id to receive hash data */
                 MPI_Comm comm /**< MPI communicator */
                )
{
  MPI_Status status;
  Data data;
  int i,count;
  short *id;
  char *key;
  MPI_Recv(&count,1,MPI_INT,task,MSGTAG,comm,&status);
  for(i=0;i<count;i++)
  {
    MPI_Recv(&data,sizeof(data),MPI_BYTE,task,MSGTAG,comm,&status);
    id=new(short);
    *id=data.id;
    key=malloc((int)data.len+1);
    check(key);
    MPI_Recv(key,data.len,MPI_BYTE,task,MSGTAG,comm,&status);
    key[data.len]='\0';
    addhashitem(hash,key,id);
  }
} /* of 'receivehash' */

#endif

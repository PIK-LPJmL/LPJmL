/**************************************************************************************/
/**                                                                                \n**/
/**           r  e  c  e  i  v  e  _  r  e  a  l  _  c  o  p  a  n  .  c           \n**/
/**                                                                                \n**/
/**     extension of LPJ to couple LPJ online with COPAN                           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool receive_real_copan(int index,Real *data,int size,int year,const Config *config)
{
#ifdef USE_MPI
  int *counts;
  int *offsets;
#endif
  float *f;
  int i;
  f=newvec(float,config->ngridcell*size);
  check(f);
  if(isroot(*config))
  {
    send_token_copan(GET_DATA,index,config);
    writeint_socket(config->socket,&year,1);
  }
#ifdef USE_MPI
  counts=newvec(int,config->ntask);
  check(counts);
  offsets=newvec(int,config->ntask);
  check(offsets);

  getcounts(counts,offsets,config->nall,size,config->ntask);
  if(mpi_read_socket(config->socket,f,MPI_FLOAT,config->nall*size,
                     counts,offsets,config->rank,config->comm))
  {
    free(f);
    free(offsets);
    free(counts);
    return TRUE;
  }
  free(offsets);
  free(counts);
#else
  if(readfloat_socket(config->socket,f,config->nall*size))
  {
    free(f);
    return TRUE;
  }
#endif
  for(i=0;i<config->ngridcell*size;i++)
    data[i]=f[i];
  free(f);
  return FALSE;
} /* of 'receive_real_copan' */

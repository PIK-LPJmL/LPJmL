/**************************************************************************************/
/**                                                                                \n**/
/**          r e c e i v e _ r e a l _ s c a l a r _ c o p a n . c                 \n**/
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

Bool receive_real_scalar_copan(Real *data,int size,const Config *config)
{
  float *f;
  int i;
  f=newvec(float,size);
  check(f);
  if(isroot(*config))
    readfloat_socket(config->socket,f,size);
#ifdef USE_MPI
  MPI_Bcast(f,size,MPI_FLOAT,0,config->comm);
#endif
  for(i=0;i<size;i++)
    data[i]=f[i];
  free(f);
  return FALSE;
} /* of 'receive_real_scalar_copan' */

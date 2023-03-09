/**************************************************************************************/
/**                                                                                \n**/
/**          r e c e i v e _ r e a l _ s c a l a r _ c o u p l e r . c             \n**/
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

Bool receive_real_scalar_coupler(int index,           /**< index of input stream */
                                 Real *data,          /**< data read from socket */
                                 int size,            /**< size of data array */
                                 int year,            /**< year */
                                 const Config *config /**< LPJmL configuration */
                                )                     /** \return TRUE on error */
{
  float *f;
  int i;
  f=newvec(float,size);
  check(f);
  if(receive_scalar_coupler(index,f,LPJ_FLOAT,size,year,config))
  {
    free(f);
    return TRUE;
  }
  for(i=0;i<size;i++)
    data[i]=f[i];
  free(f);
  return FALSE;
} /* of 'receive_real_scalar_coupler' */

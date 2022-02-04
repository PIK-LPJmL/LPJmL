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

Bool receive_real_copan(int index,           /**< index of input file */
                        Real data[],         /**< data received from socket */
                        int size,            /**< number of items per cell */
                        int year,            /**< year (AD) */
                        const Config *config /**< LPJmL configuration */
                       )                     /** \return TRUE on error */
{
  float *f;
  int i;
  f=newvec(float,config->ngridcell*size);
  check(f);
  if(receive_copan(index,f,LPJ_FLOAT,size,year,config))
  {
    free(f);
    return TRUE;
  }
  for(i=0;i<config->ngridcell*size;i++)
    data[i]=f[i];
  free(f);
  return FALSE;
} /* of 'receive_real_copan' */

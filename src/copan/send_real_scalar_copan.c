/**************************************************************************************/
/**                                                                                \n**/
/**   s  e  n  d  _  r  e  a  l  _  s  c  a  l  a  r  _  c  o  p  a  n  .  c       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function global real data into socket                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool send_real_scalar_copan(int index,           /**< index of output stream */
                            const Real *data,    /**< data sent */
                            int size,            /**< size of data */
                            Real scale,          /**< scaling factor */
                            int year,            /**< Simulation year (AD) */
                            const Config *config /**< LPJ configuration */
                           )                     /** \return TRUE on error */
{
  float *vec;
  int i;
  Bool rc;
  vec=newvec(float,size);
  if(vec==NULL)
  {
    printallocerr("vec");
    return TRUE;
  }
  for(i=0;i<size;i++)
    vec[i]=(float)(data[i]*scale);
  rc=send_scalar_copan(index,vec,LPJ_FLOAT,size,year,config);
  free(vec);
  return rc;
} /* of 'send_real_scalar_copan' */

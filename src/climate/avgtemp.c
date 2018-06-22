/**************************************************************************************/
/**                                                                                \n**/
/**                         a  v  g  t  e  m  p  .  c                              \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function gets annual average temperature                                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real avgtemp(const Climate *climate, /**< pointer to climate data */
             int cell                /**< cell index */
             )                       /** \return annual average temperature */
{
  int i,n;
  Real avg;
  n=(climate->file_temp.isdaily)  ? NDAYYEAR : NMONTH;
  avg=0;
  for(i=0;i<n;i++)
    avg+=climate->data.temp[cell*n+i];
  return avg/n;
} /* of 'avgtemp' */   

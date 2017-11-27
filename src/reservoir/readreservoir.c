/**************************************************************************************/
/**                                                                                \n**/
/**          r  e  a  d  r  e  s  e  r  v  o  i  r  .  c                           \n**/
/**                                                                                \n**/
/**     Function reads reservoir data from binary file                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool readreservoir(Reservoir *res, /**< reservoir data to read */
                   Bool swap,      /**< Byte order has to be changed (TRUE/FALSE) */
                   FILE *file      /**< pointer to binary file */
                  )                /** \return TRUE on error */
{
  if(freadint1(&res->year,swap,file)!=1)
    return TRUE;
  if(freadfloat(&res->capacity,1,swap,file)!=1)
    return TRUE;
  res->capacity*=1e12;
  if(freadfloat(&res->area,1,swap,file)!=1)
    return TRUE;
  if(freadint1(&res->inst_cap,swap,file)!=1)
    return TRUE;
  if(freadint1(&res->height,swap,file)!=1)
    return TRUE;
  if(freadint(res->purpose,NPURPOSE,swap,file)!=NPURPOSE)
    return TRUE;
  return FALSE;
} /* of 'readreservoir' */

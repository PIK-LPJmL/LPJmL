/**************************************************************************************/
/**                                                                                \n**/
/**             r  e  a  d  c  o  u  n  t  r  y  c  o  d  e  .  c                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads country and region code from binary file                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool readcountrycode(FILE *file, /**< pointer to binary file */
                     Code *code, /**< country and region code read */
                     Type type,  /**< LPJ datatypes */
                     Bool swap   /**< byte order has to be swapped */
                    )            /** \return TRUE on error */
{
  int data[2];
  if(readintvec(file,data,2,swap,type))
    return TRUE;
  code->country=(short)data[0];
  code->region=(short)data[1];
  return FALSE;
} /* of *readcountrycode' */

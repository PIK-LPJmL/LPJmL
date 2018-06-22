/**************************************************************************************/
/**                                                                                \n**/
/**              f  r  e  a  d  s  o  i  l  c  o  d  e  .  c                       \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads soil code from file                                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool freadsoilcode(FILE *file,   /**< file pointer */
                   unsigned int *soilcode,/**< soil code  (0..nsoil) */
                   Bool swap,    /**< byte order has to be changed (TRUE/FALSE) */
                   Type type     /**< Number of soil types */
                  )              /** \return TRUE on error */
{
  return readuintvec(file,soilcode,1,swap,type);
} /* of 'freadsoilcode */

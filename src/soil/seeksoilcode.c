/**************************************************************************************/
/**                                                                                \n**/
/**              s  e  e  k  s  o  i  l  c  o  d  e  .  c                          \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function seeks in soil code file to specified position                     \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int seeksoilcode(FILE *file,    /**< file pointer */
                 int pos,       /**< position in soil code file */
                 size_t offset, /**< offset in binary file */
                 Type type      /**< data type of soil codes */
                )               /** \return code of fseek */
{
  return fseek(file,pos*typesizes[type]+offset,SEEK_SET);
} /* of 'seeksoilcode' */

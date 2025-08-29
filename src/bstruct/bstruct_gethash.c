/**************************************************************************************/
/**                                                                                \n**/
/**                   b  s  t  r  u  c  t  _  g  e  t  h  a  s  h  .  c            \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Functions for reading/writing JSON-like objects from binary file           \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "bstruct_intern.h"

Hash bstruct_gethash(Bstruct bstruct /**< pointer to restart file */
                     )                /** \return pointer to hash */
{
  return bstruct->hash;
} /* of 'bstruct_gethash' */

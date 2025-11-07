/**************************************************************************************/
/**                                                                                \n**/
/**               f  i  n  d  l  a  n  d  u  s  e  t  y  p  e  .  c                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int findlandusetype(const Standlist standlist, /**< stand list */
                    Landusetype landusetype    /**< land-use type */
                   ) /** \return index of stand found or NOT_FOUND */
{
  int s;
  const Stand *stand;
  foreachstand(stand,s,standlist)
    if(getlandusetype(stand)==landusetype)
      return s;
  return NOT_FOUND;
} /* of 'findlandusetype' */

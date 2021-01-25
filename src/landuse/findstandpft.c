/**************************************************************************************/
/**                                                                                \n**/
/**                  f  i  n  d  s  t  a  n  d  p  f  t  .  c                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJ, derived from the Fortran/C++ version              \n**/
/**                                                                                \n**/
/**     Function finds agruculture grass or tree stand with specified PFT and      \n**/
/**     irrigation                                                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "agriculture.h"

int findstandpft(const Standlist standlist, /**< Stand list */
                 int pft_id,                /**< PFT id */
                 Bool irrigation            /**< irrigated (TRUE/FALSE) */
                ) /** \return index of stand found or NOT_FOUND */
{
  int s;
  const Stand *stand;
  Irrigation *data;
  foreachstand(stand,s,standlist)
    if(stand->type->landusetype==AGRICULTURE_TREE || stand->type->landusetype==AGRICULTURE_GRASS)
    {
      data=stand->data;
      if(data->irrigation==irrigation && data->pft_id==pft_id)
        return s; /* return index of stand in stand list */
    }
  return NOT_FOUND;
}/* of 'findstandpft' */

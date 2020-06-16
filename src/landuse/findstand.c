/**************************************************************************************/
/**                                                                                \n**/
/**                  f  i  n  d  s  t  a  n  d  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function finds stand with specified landusetype and irrigation             \n**/
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

int findstand(const Standlist standlist, /**< Stand list */
              Landusetype landusetype,   /**< landuse type */
              Bool irrigation            /**< irrigated (TRUE/FALSE) */
             ) /** \return index of stand found or NOT_FOUND */
{
  int s;
  const Stand *stand;
  Irrigation *data; 
  foreachstand(stand,s,standlist)
    if(stand->type->landusetype==landusetype && stand->data!=NULL)
    {
      data=stand->data;
      if(data->irrigation==irrigation)
        return s; /* return index of stand in stand list */
    }
  return NOT_FOUND;
}/* of findstand*/

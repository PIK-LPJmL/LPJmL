/**************************************************************************************/
/**                                                                                \n**/
/**                  c  h  e  c  k  _  l  u  .  c                                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function is called in sowing(). Checks if according to the input           \n**/
/**     landuse data the considered cft (without/with irrigation) could            \n**/
/**     be sown in the pixel (landfrac>0).                                         \n**/
/**     -> if TRUE: checks if a stand of such cft (with/without                    \n**/
/**        irrigation) still exists                                                \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "crop.h"
#include "agriculture.h"

Bool check_lu(const Standlist standlist, /**< List of stands */
              Real landfrac,             /**< land fraction for crop */
              int id,                    /**< PFT index of crop */
              Bool irrigation            /**< irrigated (TRUE/FALSE) */
             )                           /** \return TRUE if crop stand can
                                              be established */
{
  const Stand *stand;
  const Pft *pft;
  Irrigation *data;
  int s;

  if(landfrac>0)
  {
    foreachstand(stand,s,standlist)
      if(stand->type->landusetype==AGRICULTURE)
      {
        pft=getpft(&stand->pftlist,0);
        data=stand->data;
        if(pft->par->id==id && data->irrigation==irrigation)
          return FALSE;
      }
    return TRUE;
  }
  else 
    return FALSE;
} /* of 'check_lu' */

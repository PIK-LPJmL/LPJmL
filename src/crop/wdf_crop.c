/**************************************************************************************/
/**                                                                                \n**/
/**                    w  d  f  _  c  r  o  p  .  c                                \n**/
/**                                                                                \n**/
/**     Function calculates water deficit fraction                                 \n**/
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
#include "crop.h"

Real wdf_crop(Pft *pft,    /**< PFT variables */
              Real demand, /**< water demand */
              Real supply  /**< water supply */
             )             /** \return water deficit fraction (%) */
{
  Pftcrop *crop;
  crop=pft->data;
  crop->demandsum+=demand;
  crop->supplysum+=(supply>demand) ? demand : supply;
  return (crop->demandsum>0.0) ? 100.0*crop->supplysum/crop->demandsum: 100.0;
} /* of 'wdf_crop' */

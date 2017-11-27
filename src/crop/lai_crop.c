/**************************************************************************************/
/**                                                                                \n**/
/**                  l  a  i  _  c  r  o  p  .  c                                  \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "crop.h"

Real lai_crop(const Pft *pft /**< pointer to PFT */
             )               /** \return leaf area index (LAI) */
{
  Pftcrop *crop;
  crop=pft->data;

  return max(0,(crop->lai-crop->lai_nppdeficit));
} /* of 'lai_crop' */
/*
- this function is not called, pendant to lai_tree()/lai_grass()
*/

Real actual_lai_crop(const Pft *pft /**< pointer to PFT */
                    )               /** \return actual leaf area index (LAI) */
{
  Pftcrop *crop;
  crop=pft->data;

  return max(0,(crop->lai-crop->lai_nppdeficit));
} /* of 'actual_lai_crop' */

/*
- this function is called in interception() and returns the actual lai of a cft 
*/

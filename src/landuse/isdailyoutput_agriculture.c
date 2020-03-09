/**************************************************************************************/
/**                                                                                \n**/
/**                i s d a i l y o u t p u t _ a g r i c u l t u r e . c           \n**/
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
#include "agriculture.h"

Bool isdailyoutput_agriculture(const Output *output, /**< Output data */
                               const Stand *stand    /**< stand pointer */
                              )
{
  const Irrigation *data;
  const Pft *pft;
  int p;
  data = stand->data;
  if(output->daily.irrigation != data->irrigation)
    return FALSE;
  foreachpft(pft,p,&stand->pftlist)
    if (pft->par->id == output->daily.cft)
      return TRUE;
  return FALSE;
} /* of 'isdailyoutput_agriculture' */

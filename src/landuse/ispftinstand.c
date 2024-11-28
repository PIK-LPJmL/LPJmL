/**************************************************************************************/
/**                                                                                \n**/
/**                i  s  p  f  t  i  n  s  t  a  n  d  .  c                        \n**/
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

Bool ispftinstand(const Pftlist *pftlist, /**< array of PFTs */
                  int pft_id              /**< PFT index to find */
                 )                        /**< return TRUE if PFT was found */
{
  const Pft *pft;
  int p;
  foreachpft(pft,p,pftlist)
    if(pft->par->id==pft_id)
      return TRUE;
  return FALSE;
} /* of 'ispftinstand' */

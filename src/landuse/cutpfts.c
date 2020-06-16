/**************************************************************************************/
/**                                                                                \n**/
/**                       c  u  t  p  f  t  s  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function deletes all PFTs and transfers carbon to litter                   \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void cutpfts(Stand *stand /**< Pointer to stand */
            )
{
  int p;
  Pft *pft;
  foreachpft(pft,p,&stand->pftlist)
    litter_update(&stand->soil.litter,pft,pft->nind);
  freepftlist(&stand->pftlist);
} /* of 'cutpfts' */

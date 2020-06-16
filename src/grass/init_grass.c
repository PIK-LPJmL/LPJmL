/**************************************************************************************/
/**                                                                                \n**/
/**             i  n  i  t  _  g  r  a  s  s  .  c                                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initializes annual variables                                      \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#include "grass.h"

void init_grass(Pft *pft /**< pointer to grass PFT */
               )
{
  pft->bm_inc=0;
  pft->nind=1;
#ifdef DAILY_ESTABLISHMENT
  pft->established=FALSE;
#endif
} /* of 'init_grass' */

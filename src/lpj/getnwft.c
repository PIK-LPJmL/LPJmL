/**************************************************************************************/
/**                                                                                \n**/
/**                   g  e  t  n  w  f  t  .  c                                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function count number of WFTs.                                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml                                   \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

int getnwft(const Pftpar pftpar[], /**< PFT parameter array */
            int npft               /**< number of natural PFTs */
           )                       /**< returns number of WFTs */
{
  int p,nwft;
  nwft=0;
#if defined INCLUDEWP
  for(p=0;p<npft;p++)
    if (pftpar[p].type==TREE && pftpar[p].cultivation_type==WP)
      nwft++;
#endif
  return nwft;
} /* of 'getnwft' */

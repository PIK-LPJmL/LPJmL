/**************************************************************************************/
/**                                                                                \n**/
/**                p  f  t  l  i  t  t  e  r  _  a  g  t  o  p  .  c               \n**/
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

void pftlitter_agtop(Real carbon[],       /**< PFT-specfic litter carbon (gC/m2) */
                     const Litter *litter /**< pointer to litter data */
                    )
{
  int i,l;
  for(l=0;l<litter->n;l++)
  {
    carbon[litter->item[l].pft->id]=litter->item[l].agtop.leaf.carbon;
    for(i=0;i<NFUELCLASS;i++)
      carbon[litter->item[l].pft->id]+=litter->item[l].agtop.wood[i].carbon;
  }
} /* of 'pft_litteragtop' */

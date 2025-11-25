/**************************************************************************************/
/**                                                                                \n**/
/**        a  l  l  o  c  a  t  i  o  n  _  t  o  d  a  y  .  c                    \n**/
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
#include "grass.h"

void allocation_today(Stand *setasidestand, /**< pointer to setaside stand */
                      const Config *config  /**< LPJmL configuration */
                     )
{
  int p,npft;
  Real *fpc_inc;
  Pft *pft;
  npft=getnpft(&setasidestand->pftlist); /* get number of established PFTs */
  if(npft>0) /* nonzero? */
  {
    fpc_inc=newvec(Real,npft);
    check(fpc_inc);
    foreachpft(pft,p,&setasidestand->pftlist)
    {
      /* only grass PFTs are established on setaside stand */
      if(allocation_grass(&setasidestand->soil.litter,pft,fpc_inc+p,config))
      {
        /* kill PFT from list of established PFTs */
        fpc_inc[p]=fpc_inc[getnpft(&setasidestand->pftlist)-1]; /*moved here by W. von Bloh */
        litter_update_grass(&setasidestand->soil.litter,pft,pft->nind,config);
        delpft(&setasidestand->pftlist,p);
        p--; /* adjust loop variable */
      }
      else
       pft->bm_inc.carbon=0;
    } /* of foreachpft */
    light(setasidestand,fpc_inc,config);
    free(fpc_inc);
  }
} /* of 'allocation_today' */

/*
- called in functions sowing.c and update_daily.c
- calls specific function allocation() for each pft
- sets bm_inc for each pft to 0
- deletes pft if it could not survive
- calls specific function light()
*/

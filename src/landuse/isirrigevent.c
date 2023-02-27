/**************************************************************************************/
/**                                                                                \n**/
/**                i  s  i  r  r  i  g  e  v  e  n  t  .  c                        \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** Function determines whether irrigation is necessary                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool isirrigevent(const Stand *stand /**< pointer to stand */
                 )                   /** \return irrigation is necessary (TRUE/FALSE) */
{
  const Pft *pft;
  Real wr,irrig_threshold;
  int p;
  foreachpft(pft,p,&stand->pftlist)
  {
    wr=getwr(&stand->soil,pft->par->rootdist);
    if(stand->cell->climbuf.aprec<param.aprec_lim)
      irrig_threshold=pft->par->irrig_threshold.dry;
    else
      irrig_threshold=pft->par->irrig_threshold.humid;
    if(wr<irrig_threshold)
      return TRUE;
      /* if one of possibly two (grass) pfts requests irrigation, both get irrigated */
  } /* of foreachpft() */
  return FALSE;
} /* of 'isirrigevent' */

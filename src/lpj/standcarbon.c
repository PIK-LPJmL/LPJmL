/**************************************************************************************/
/**                                                                                \n**/
/**                s  t  a  n  d  c  a  r  b  o  n  .  c                           \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function computes total carbon in stand                                    \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real standcarbon(const Stand *stand /**< pointer to stand */
                )                   /** \return carbon sum (g/m2) */
{
  int p;
  const Pft *pft;
  Real totc;
  totc=soilcarbon(&stand->soil); /* get carbon in soil */
  foreachpft(pft,p,&stand->pftlist)
    totc+=vegc_sum(pft); /* sum up carbon in PFTs */
  return totc;
} /* of 'standcarbon' */

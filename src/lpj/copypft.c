/**************************************************************************************/
/**                                                                                \n**/
/**                       c  o  p  y  p  f  t  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function copies PFT variables                                              \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void copypft(Pft *dst, const Pft *src)
{
  dst->fpc = src->fpc;
  dst->nind = src->nind;
  dst->gdd = src->gdd;
  dst->bm_inc = src->bm_inc;
  dst->gdd = src->gdd;
  dst->wscal = src->wscal;
  dst->wscal_mean = src->wscal_mean;
  dst->phen = src->phen;
  dst->aphen = src->aphen;
  /* call PFT-specific copy function */
  src->par->copy(dst, src);
} /* of 'copypft' */

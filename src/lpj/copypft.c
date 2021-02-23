/***************************************************************************/
/**                                                                       **/
/**                       c  o  p  y  p  f  t  .  c                       **/
/**                                                                       **/
/**     C implementation of LPJ, derived from the Fortran/C++ version     **/
/**                                                                       **/
/**     Function copies PFT variables                                     **/
/**                                                                       **/
/**     written by Werner von Bloh, Sibyll Schaphoff                      **/
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change: $Date:: 2016-07-04 08:53:55 +0200 (Mo, 04 Jul 2016#$ **/
/**     By         : $Author:: bloh                            $          **/
/**                                                                       **/
/***************************************************************************/

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

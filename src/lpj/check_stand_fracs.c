/**************************************************************************************/
/**                                                                                \n**/
/**     c  h  e  c  k  _  s  t  a  n  d  _  f  r  a  c  s  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function checks stand fractions on consisteny. Sum of stand                \n**/
/**     must be <=1 and stand fractions must be non-negative                       \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"
#if defined IMAGE && defined COUPLED
#define accuracy 1e-7
#else
#define accuracy 1e-4
#endif

void check_stand_fracs(const Cell *cell, /**< pointer to cell */
                       Real lakefrac     /**< lake fraction (0..1) */
                      )
{
  int s;
  Real frac_sum;
  const Stand *stand;
  String line;

  frac_sum=lakefrac; 
  foreachstand(stand,s,cell->standlist)
  {
    if(stand->frac>0) 
      frac_sum+=stand->frac;
    else 
    {
      foreachstand(stand,s,cell->standlist)
        fprintf(stderr,"frac[%s]=%g\n",stand->type->name,stand->frac);
      fail(NEGATIVE_STAND_FRAC_ERR,TRUE,"Negative or zero stand fraction %g for %s stand, lakefrac: %g, cell (%s)",
           stand->frac,stand->type->name,lakefrac,sprintcoord(line,&cell->coord));
    }
  }
  
  if(fabs(frac_sum-1)>accuracy)
#if defined IMAGE && defined COUPLED
    fail(STAND_FRAC_SUM_ERR,TRUE,"Sum of stand fractions differs from 1 by %g, frac_sum %g, lakefrac: %g, cell (%s), reservoirfrac %g",
         fabs(frac_sum-1),frac_sum, lakefrac,sprintcoord(line,&cell->coord),cell->ml.reservoirfrac);
#else
    fail(STAND_FRAC_SUM_ERR,TRUE,"Sum of stand fractions differs from 1 by %g, lakefrac: %g, cell (%s)",
         fabs(frac_sum-1),lakefrac,sprintcoord(line,&cell->coord));
#endif
} /* of 'check_stand_fracs' */

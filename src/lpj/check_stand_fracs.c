/**************************************************************************************/
/**                                                                                \n**/
/**     c  h  e  c  k  _  s  t  a  n  d  _  f  r  a  c  s  .  c                    \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function checks stand fractions on consisteny. Sum of stand                \n**/
/**     must be 1 and stand fractions must be non-negative                         \n**/
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

Bool check_stand_fracs(const Cell *cell, /**< pointer to cell */
                       Bool isfail       /**< terminate on error (TRUE/FALSE) */
                      )                  /** \return TRUE on error */
{
  int s;
  Real frac_sum;
  const Stand *stand;
  String line;

  frac_sum=cell->lakefrac+cell->ml.reservoirfrac;
  foreachstand(stand,s,cell->standlist)
  {
    if(stand->frac>0)
      frac_sum+=stand->frac;
    else
    {
      if(isfail)
        fail(NEGATIVE_STAND_FRAC_ERR,TRUE,"Negative or zero stand fraction %g for %s stand, lakefrac: %g, cell (%s)",
             stand->frac,stand->type->name,cell->lakefrac,sprintcoord(line,&cell->coord));
      fprintf(stderr,"ERROR%03d: Negative or zero stand fraction %g for %s stand, lakefrac: %g, cell (%s)",
              NEGATIVE_STAND_FRAC_ERR,stand->frac,stand->type->name,cell->lakefrac,sprintcoord(line,&cell->coord));
      return TRUE;
    }
  }

  if(fabs(frac_sum-1)>accuracy)
  {
    if(isfail)
      fail(STAND_FRAC_SUM_ERR,TRUE,"Sum of stand fractions differs from 1 by %g, frac_sum %g, lakefrac: %g, cell (%s), reservoirfrac %g",
           fabs(frac_sum-1),frac_sum, cell->lakefrac,sprintcoord(line,&cell->coord),cell->ml.reservoirfrac);
    fprintf(stderr,"ERROR%03d: Sum of stand fractions differs from 1 by %g, frac_sum %g, lakefrac: %g, cell (%s), reservoirfrac %g",
            STAND_FRAC_SUM_ERR,fabs(frac_sum-1),frac_sum, cell->lakefrac,sprintcoord(line,&cell->coord),cell->ml.reservoirfrac);
    return TRUE;
  }
  return FALSE;
} /* of 'check_stand_fracs' */

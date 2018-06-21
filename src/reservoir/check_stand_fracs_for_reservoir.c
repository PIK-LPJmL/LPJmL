/**************************************************************************************/
/**                                                                                \n**/
/**    c h e c k _ s t a n d _ f r a c s _ f o r _ r e s e r v o i r . c           \n**/
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

#define accuracy 1e-4

Bool check_stand_fracs_for_reservoir(const Cell *cell, /**< cell pointer */
                                     Real *difffrac    /** \param[out] fraction above 1 */
                                     )                 /** \return TRUE if frac_sum > 1 */
{
  int s;
  Real frac_sum;
  const Stand *stand;

  frac_sum=cell->lakefrac+cell->ml.reservoirfrac; 
  foreachstand(stand,s,cell->standlist)
  {
    if(stand->frac>0) 
      frac_sum+=stand->frac;
    else 
      fail(NEGATIVE_STAND_FRAC_ERR,TRUE,"negative STAND_FRAC :%g %g %g",
           stand->frac,cell->lakefrac,cell->ml.reservoirfrac);
  }
  *difffrac=frac_sum-1;

  return (fabs(frac_sum-1)>accuracy);
      
} /* of 'check_stand_fracs_for_reservoir' */

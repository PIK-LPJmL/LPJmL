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

void check_stand_fracs2(const Cell *cell,    /**< pointer to cell */
                        Real lakefrac,       /**< lake fraction (0..1) */
                        int ncft,            /**< number of crop PFTs */
                        const char *routine, /**< function name called from */
                        int linenr           /**< line number called from */
                       )
{
  int s,p;
  Real frac_sum;
  const Stand *stand,*checkstand;
  const Pft* pft;
  String line;
  frac_sum=lakefrac;
  foreachstand(stand,s,cell->standlist)
  {
    if(stand->frac>0)
      frac_sum+=stand->frac;
    else
    {
      foreachstand(checkstand,s,cell->standlist)
      {
        fprintf(stderr,"frac[%s]=%g\n",checkstand->type->name,checkstand->frac);
        foreachpft(pft,p,&checkstand->pftlist)
           fprintf(stderr,"frac[%s]= %g standNR: %d PFT: %s \n",checkstand->type->name,checkstand->frac,s,pft->par->name);
      }
      fail(NEGATIVE_STAND_FRAC_ERR,TRUE,TRUE,"Negative or zero stand fraction %g for %s stand, lakefrac: %g, cell (%s) from function %s() in line %d.\n",
           stand->frac,stand->type->name,lakefrac,sprintcoord(line,&cell->coord), routine,linenr);
    }
  }

  if(fabs(frac_sum-1)>accuracy)
  {
#if defined IMAGE && defined COUPLED
    fail(STAND_FRAC_SUM_ERR,TRUE,TRUE,"Sum of stand fractions differs from 1 by %g, frac_sum %g, lakefrac: %g, cell (%s), reservoirfrac %g from routine %s() in line %d.\n",
         fabs(frac_sum-1),frac_sum, lakefrac,sprintcoord(line,&cell->coord),cell->ml.reservoirfrac,routine,linenr);
#else
    Real frac_sum2;
    int j;
    //for(j=0;j<ncft;j++)
      //fprintf(stderr,"landfrac_rainfed: %g landfrac_irr: %g\n", cell->ml.landfrac[0].crop[j],cell->ml.landfrac[1].crop[j]);
    fprintf(stderr,"grassfrac_rainfed: %g grassfrac_irr: %g\n",(cell->ml.landfrac[0].grass[0]+cell->ml.landfrac[0].grass[1]),
        (cell->ml.landfrac[1].grass[0]+cell->ml.landfrac[1].grass[1]));
    fprintf(stderr,"ml.cropfrac_rf: %g ml.cropfrac_ir: %g  ml.cropfrac_wl: %g \n",
        cell->ml.cropfrac_rf,cell->ml.cropfrac_ir,cell->ml.cropfrac_wl);
    frac_sum2=0;
    foreachstand(stand,s,cell->standlist)
    {
      frac_sum2+=stand->frac;
      fprintf(stderr,"frac[%s]= %g standNR: %d iswetland: %d\n",stand->type->name,stand->frac,s,stand->soil.iswetland);
    }
//    foreachstand(stand,s,cell->standlist)
//     foreachpft(pft,p,&stand->pftlist)
//            fprintf(stderr,"frac[%s]= %g standNR: %d PFT: %s \n",stand->type->name,stand->frac,s,pft->par->name);
    fail(STAND_FRAC_SUM_ERR,TRUE,FALSE,
        "Sum of stand fractions differs from 1 by %g, lakefrac: %g, fracsum: %g  fracsum2: %g reservoirfrac: %g cell (%s) from functtion %s() in line %d.\n",
         (frac_sum-1),cell->lakefrac,frac_sum,frac_sum2,cell->ml.reservoirfrac,sprintcoord(line,&cell->coord),routine,linenr);
#endif
  }
} /* of 'check_stand_fracs2' */

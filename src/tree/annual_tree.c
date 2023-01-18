/**************************************************************************************/
/**                                                                                \n**/
/**            a  n  n  u  a  l  _  t  r  e  e  .  c                               \n**/
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
#include "tree.h"
#define inun_mort_max 0.5

Bool annual_tree(Stand *stand,       /**< pointer to stand */
                 Pft *pft,           /**< pointer to PFT */
                 Real *fpc_inc,      /**< FPC increment */
                 Bool isdaily,       /**< daily temperature data (TRUE/FALSE) */
                 const Config *config /**< LPJmL configuration */
                )                    /** \return TRUE on death */
{
  Stocks turnover_ind={0,0};
  Bool isdead;
#ifdef CHECK_BALANCE
  Real end = 0;
  Real start = 0;
  Real estab=pft->establish.carbon;
  Real bm_inc_carbon=pft->bm_inc.carbon;
  start = standstocks(stand).carbon + soilmethane(&stand->soil)-pft->establish.carbon;
#endif
  turnover_ind=turnover_tree(&stand->soil.litter,pft,config);
  isdead=allocation_tree(&stand->soil.litter,pft,fpc_inc,config);
  fpc_tree(pft);

  if(!isdead)
  {
    isdead=mortality_tree(&stand->soil.litter,pft,turnover_ind.carbon,
                          stand->cell->climbuf.temp_max,isdaily,config);
  if (!(stand->prescribe_landcover==LANDCOVERFPC && (stand->type->landusetype==NATURAL || stand->type->landusetype==WETLAND)) &&
      !isdead)  /* still not dead? */
      isdead=!survive(pft->par,&stand->cell->climbuf);
  }
#ifdef CHECK_BALANCE
  end = standstocks(stand).carbon + soilmethane(&stand->soil)-pft->establish.carbon;
  if (fabs(end-start)>0.01)
    fprintf(stderr, "C_ERROR annaul_tree: %.4f start : %.4f end : %.4f bm_inc.carbon: %g  bm_inc.carbon.start: %g estab: %g flux_estab.carbon: %g  stand.frac:%g type:%s PFT:%s nind: %g \n\n",
        end-start, start, end,pft->bm_inc.carbon,bm_inc_carbon, estab,pft->establish.carbon,stand->frac,stand->type->name,pft->par->name,pft->nind);
#endif
  return isdead;
} /* of 'annual_tree' */

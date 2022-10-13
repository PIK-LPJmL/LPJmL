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
  Real mort_max,nind_kill,nind_alt;
  Real stress;
#ifdef CHECK_BALANCE
  Real end = 0;
  Real start = 0;
  Real estab=pft->establish.carbon;
  Real bm_inc_carbon=pft->bm_inc.carbon;
  start = standstocks(stand).carbon + soilmethane(&stand->soil)-pft->establish.carbon;
//  fprintf(stderr, "1:C_ERROR annaul_tree: %g start : %g end : %g bm_inc.carbon: %g  bm_inc.carbon.start: %g estab: %g flux_estab.carbon: %g  stand.frac:%g type:%s PFT:%s nind: %g \n\n",
//      ende-anfang, anfang, ende,pft->bm_inc.carbon,bm_inc_carbon, estab,pft->establish.carbon,stand->frac,stand->type->name,pft->par->name,pft->nind);
#endif
  turnover_ind=turnover_tree(&stand->soil.litter,pft,config);
  //start = standstocks(stand).carbon + soilmethane(&stand->soil)-pft->establish.carbon;
  isdead=allocation_tree(&stand->soil.litter,pft,fpc_inc,config);
  nind_alt=pft->nind;

/*
  if(pft->inun_count>pft->par->inun_dur)
  {*/
    stress=pft->inun_count/pft->par->inun_dur;
/*    if(stress>2)
      isdead=TRUE;
  }
*/
  mort_max=inun_mort_max;                           /*should be an global parameter*/
  nind_kill=pft->nind*(1-pow(mort_max,stress));
  litter_update_tree(&stand->soil.litter,pft,nind_kill,config);
  if(pft->nind>0)
    pft->bm_inc.nitrogen*=(pft->nind-nind_kill)/pft->nind;
  pft->nind-=nind_kill;
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

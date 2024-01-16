/**************************************************************************************/
/**                                                                                \n**/
/**            a  n  n  u  a  l  _  g  r  a  s  s  .  c                            \n**/
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
#include "grass.h"
#define TURN FALSE
Bool annual_grass(Stand *stand,        /**< pointer to stand */
                  Pft *pft,            /**< pointer to PFT variables */
                  Real *fpc_inc,       /**< FPC increment */
                  Bool UNUSED(isdaily), /**< daily temperature data? */
                  const Config *config /**< LPJmL configuration */
                 )                     /** \return TRUE on death */
{
  Bool isdead=FALSE;
  Real stress;
#ifdef CHECK_BALANCE
  Stocks start = {0,0};
  Real end = 0;
#endif
  stress = 0;
#ifdef CHECK_BALANCE
  start.carbon = standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4-pft->establish.carbon;
  start.nitrogen = standstocks(stand).nitrogen -pft->establish.nitrogen;
#endif
  if(stand->type->landusetype!=GRASSLAND && stand->type->landusetype!=OTHERS && stand->type->landusetype!=BIOMASS_GRASS  && stand->type->landusetype != SETASIDE_WETLAND && stand->type->landusetype != SETASIDE_IR && stand->type->landusetype != SETASIDE_RF)
  {
    turnover_grass(&stand->soil.litter,pft,(Real)stand->growing_days/NDAYYEAR,config);
#ifdef CHECK_BALANCE
  end = standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4-pft->establish.carbon;
  if (fabs(end-start.carbon)>0.01)
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,TRUE,"Invalid carbon balance in %s: %g start : %g end : %g bm_inc.carbon: %g stand.frac: %g type:%s  PFT: %s nind: %g\n",
         __FUNCTION__,end-start.carbon, start.carbon, end,pft->bm_inc.carbon,stand->frac,stand->type->name,pft->par->name,pft->nind);
  end = standstocks(stand).nitrogen-pft->establish.nitrogen;
  if (fabs(end-start.nitrogen)>0.01)
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,TRUE, "Invalid nitrogen balance in %s: %g start : %g end : %g  bm_inc.nitrogen: %g stand.frac: %g type:%s  PFT:%s nind: %g establish.nitrogen: %g\n",
        __FUNCTION__,end-start.nitrogen, start.nitrogen, end,pft->bm_inc.nitrogen,stand->frac,stand->type->name,pft->par->name,pft->nind,pft->establish.nitrogen);
#endif

    isdead=allocation_grass(&stand->soil.litter,pft,fpc_inc,config);
  }
  if (pft->inun_count>pft->par->inun_dur)
    stress = pft->inun_count / pft->par->inun_dur;
  if (stress>3) isdead = TRUE;
  if (!(stand->prescribe_landcover==LANDCOVERFPC && (stand->type->landusetype==NATURAL || stand->type->landusetype==WETLAND)) &&
      !isdead)  /* still not dead? */
    isdead=!survive(pft->par,&stand->cell->climbuf);
#ifdef CHECK_BALANCE
  end = standstocks(stand).carbon + soilmethane(&stand->soil)*WC/WCH4-pft->establish.carbon;
  if (fabs(end-start.carbon)>0.01)
    fail(INVALID_CARBON_BALANCE_ERR,FAIL_ON_BALANCE,TRUE,"Invalid carbon balance in %s: %g start : %g end : %g growing days: %d bm_inc.carbon: %g stand.frac: %g type:%s  PFT:%s nind: %g",
         __FUNCTION__,end-start.carbon, start.carbon, end, stand->growing_days,pft->bm_inc.carbon,stand->frac,stand->type->name,pft->par->name,pft->nind);
  end = standstocks(stand).nitrogen-pft->establish.nitrogen;
  if (fabs(end-start.nitrogen)>0.01)
    fail(INVALID_NITROGEN_BALANCE_ERR,FAIL_ON_BALANCE,TRUE, "Invalid nitrogen balance in %s: %g start : %g end : %g  bm_inc.nitrogen: %g stand.frac: %g type:%s  PFT:%s nind: %g",
         __FUNCTION__,end-start.nitrogen, start.nitrogen, end,pft->bm_inc.nitrogen,stand->frac,stand->type->name,pft->par->name,pft->nind);
#endif
  stand->growing_days=0;
  return isdead;
} /* of 'annual_grass' */

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
  Real start = 0;
  Real end = 0;
#endif
  stress = 0;
#ifdef CHECK_BALANCE
  start = standstocks(stand).carbon + soilmethane(&stand->soil)-pft->establish.carbon;
#endif
  if(stand->type->landusetype!=GRASSLAND && stand->type->landusetype!=BIOMASS_GRASS)
  {
    turnover_grass(&stand->soil.litter,pft,(Real)stand->growing_days/NDAYYEAR,config);
    isdead=allocation_grass(&stand->soil.litter,pft,fpc_inc,config);
  }
  if (pft->inun_count>pft->par->inun_dur)
    stress = pft->inun_count / pft->par->inun_dur;
  if (stress>3) isdead = TRUE;
  if (!(stand->prescribe_landcover==LANDCOVERFPC && (stand->type->landusetype==NATURAL || stand->type->landusetype==WETLAND)) &&
      !isdead)  /* still not dead? */
    isdead=!survive(pft->par,&stand->cell->climbuf);
#ifdef CHECK_BALANCE
  end = standstocks(stand).carbon + soilmethane(&stand->soil)-pft->establish.carbon;
  if (fabs(end-start)>0.01)
    fprintf(stderr, "C_ERROR annaul_grass: %g start : %g endS : %g growing days: %d bm_inc.carbon: %g stand.frac: %g type:%s  PFT:%s nind: %g\n", end-start, start, end, stand->growing_days,pft->bm_inc.carbon,stand->frac,stand->type->name,pft->par->name,pft->nind);
#endif
  stand->growing_days=0;
  return isdead;
} /* of 'annual_grass' */

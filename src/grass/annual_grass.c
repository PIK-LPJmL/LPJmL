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
  Real ende = 0;
  Real anfang = 0;
#endif
  stress = 0;
#ifdef CHECK_BALANCE
  anfang = standstocks(stand).carbon + soilmethane(&stand->soil);
#endif
  if(stand->type->landusetype!=GRASSLAND && stand->type->landusetype!=BIOMASS_GRASS)
  {
    turnover_grass(&stand->soil.litter,pft,(Real)stand->growing_days/NDAYYEAR,config);
    isdead=allocation_grass(&stand->soil.litter,pft,fpc_inc,config);
  }
  stand->growing_days=0;
  if (pft->inun_count>pft->par->inun_dur)
    stress = pft->inun_count / pft->par->inun_dur;
  if (stress>2) isdead = TRUE;
  if (!(pft->stand->prescribe_landcover==LANDCOVERFPC && pft->stand->type->landusetype==NATURAL) &&
      !isdead)  /* still not dead? */
    isdead=!survive(pft->par,&stand->cell->climbuf);
#ifdef CHECK_BALANCE
  ende = standstocks(stand).carbon + soilmethane(&stand->soil);
  if (fabs(anfang - ende)>epsilon)
    fprintf(stdout, "C_ERROR annaul_grass: %g anfang : %g ende : %g stand.frac: %g\n", anfang - ende, anfang, ende, stand->frac);
#endif
  return isdead;
} /* of 'annual_grass' */

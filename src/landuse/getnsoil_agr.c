/**************************************************************************************/
/**                                                                                \n**/
/**               g  e  t  n  s  o  i  l  _  a  g  r  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function sums N content in soil and vegetation for all agricultural stands \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

void getnsoil_agr(Real *norg_soil_agr, /**< [out] soil organic N in agriculture (gN/m2) */
                  Real *nmin_soil_agr, /**< [out] soil mineral N in agriculture (gN/m2) */
                  Real *nveg_soil_agr, /**< [out] vegetation N in agriculture (gN/m2) */
                  const Cell *cell     /**< [in] pointer to cell */
                 )
{
  const Stand *stand;
  const Pft *pft;
  int s,p,l;
  *norg_soil_agr=*nmin_soil_agr=*nveg_soil_agr=0;
  foreachstand(stand,s,cell->standlist)
    if(stand->type->landusetype==SETASIDE_RF || stand->type->landusetype==SETASIDE_IR || stand->type->landusetype==AGRICULTURE)
    {
      *norg_soil_agr+=litterstocks(&stand->soil.litter).nitrogen*stand->frac;
      forrootsoillayer(l)
      {
        *norg_soil_agr+=(stand->soil.pool[l].slow.nitrogen+stand->soil.pool[l].fast.nitrogen)*stand->frac;
        *nmin_soil_agr+=(stand->soil.NO3[l]+stand->soil.NH4[l])*stand->frac;
      }
      foreachpft(pft,p,&stand->pftlist)
        *nveg_soil_agr+=vegn_sum(pft)*stand->frac;
    }
} /* of 'getnsoil_agr' */

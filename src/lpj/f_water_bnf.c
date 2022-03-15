 /**************************************************************************************/
/**                                                                                \n**/
/**                    f  _  s  w  c  _  b  n  f  .  c                             \n**/
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

Real f_water_bnf(Pft *pft,             /**< PFT */
               Real swc             /**< relative soil water content (-) */
              )                      /** \return soil water dependent reduction of BNF */
{
  Real f_water=0.0;
  if(swc <= pft->par->swc_bnf.low)
    f_water = 0;
  if(swc > pft->par->swc_bnf.low && swc < pft->par->swc_bnf.high)
    f_water = pft->par->phi_bnf[0]+pft->par->phi_bnf[1]*swc;
  if(swc >= pft->par->swc_bnf.high)
    f_water = 1.0;
return(f_water);
}

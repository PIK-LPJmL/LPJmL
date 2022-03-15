 /**************************************************************************************/
/**                                                                                \n**/
/**                 f  _  t  e  m  p  _  b  n  f  .  c                             \n**/
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

Real f_temp_bnf(Pft *pft,             /**< PFT */
                Real temp            /**< soil temperature (deg C) */
               )                      /** \return soil temperature dependent reduction of BNF */
{
  Real f_temp=0.0;
  if(temp < pft->par->temp_bnf_lim.low || temp > pft->par->temp_bnf_lim.high)
    f_temp = 0.0;
  if(temp >= pft->par->temp_bnf_lim.low && temp < pft->par->temp_bnf_opt.low)
    f_temp = (temp-pft->par->temp_bnf_lim.low)/(pft->par->temp_bnf_opt.low-pft->par->temp_bnf_lim.low);
  if(temp >= pft->par->temp_bnf_opt.low && temp <= pft->par->temp_bnf_opt.high)
    f_temp = 1.0;
  if(temp > pft->par->temp_bnf_opt.high && temp <= pft->par->temp_bnf_lim.high)
    f_temp = (pft->par->temp_bnf_lim.high-temp)/(pft->par->temp_bnf_lim.high-pft->par->temp_bnf_opt.high);
  return(f_temp);
}

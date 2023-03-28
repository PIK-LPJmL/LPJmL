 /**************************************************************************************/
/**                                                                                \n**/
/**     m  a  _  b  i  o  l  o  g  i  c  a  l  _  n  _  f  i  x  a  t  i  o  n     \n**/
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


static Real f_temp_bnf(Pft *pft,             /**< PFT */
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

static Real f_water_bnf(Pft *pft,             /**< PFT */
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


Real ma_biological_n_fixation(Pft *pft,             /**< PFT */
                              Soil *soil,           /**< Soil */
                              Real n_deficit,       /**< nitrogen deficit of today */
                              const Config *config  /**< LPJmL configuration  */
               )                      /** \return biologically fixed nitrogen (gN/m2) */
{
  Real rootdist_n[LASTLAYER];
  Real bnf=0.0;
  Real npp_requ=0.0;
  Real max_cost=0.0;
  int l;
  if(config->permafrost)
    getrootdist(rootdist_n,pft->par->rootdist,soil->mean_maxthaw);
  else
    forrootsoillayer(l)
      rootdist_n[l]=pft->par->rootdist[l];

  for(l=0; l<=1; l++) //only first 50cm of soil considered for BNF
    bnf+=pft->par->nfixpot*rootdist_n[l]*f_temp_bnf(pft, soil->temp[l])*f_water_bnf(pft, soil->w[l]);
  if(bnf > n_deficit)
    bnf = n_deficit;
  npp_requ = pft->par->bnf_cost*bnf;
  max_cost = pft->npp_bnf*pft->par->maxbnfcost;
  if(npp_requ > max_cost)
  {
    npp_requ = max_cost;
    bnf =  max_cost / pft->par->bnf_cost;
  }
  pft->npp_bnf=npp_requ;//overwrite npp_bnf to contain npp required for bnf instead of assimilated npp
  return(bnf);
}

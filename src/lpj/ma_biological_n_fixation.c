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

Real ma_biological_n_fixation(Pft *pft,             /**< PFT */
                              Soil *soil,           /**< Soil */
                              Real n_deficit,       /**< nitrogen deficit of today */
                              const Config *config  /**< LPJmL configuration  */
               )                      /** \return biologically fixed nitrogen (gN/m2) */
{
  Real rootdist_n[LASTLAYER];
  Real bnf=0.0;
  Real npp_requ=0.0;
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
  npp_requ = 6*bnf;
  if(npp_requ > (pft->npp_bnf*pft->par->maxbnfcost))
  {
    npp_requ = pft->npp_bnf*pft->par->maxbnfcost;
    bnf =  pft->npp_bnf*pft->par->maxbnfcost / 6;
  }
  pft->npp_bnf=npp_requ;//overwrite npp_bnf to contain npp required for bnf instead of assimilated npp
  return(bnf);
}

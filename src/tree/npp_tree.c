/**************************************************************************************/
/**                                                                                \n**/
/**                    n  p  p  _  t  r  e  e  .  c                                \n**/
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

Real npp_tree(Pft *pft, /**< PFT variables */
              Real gtemp_air, /**< value of air temperature response function */
              Real gtemp_soil, /**< value of soil temperature response function */
              Real assim  /**< assimilation (gC/m2) */
             ) /* \return net primary productivity (gC/m2) */
{
  Pfttree *tree;
  const Pfttreepar *par;
  Real mresp,npp; 
  tree=pft->data;
  par=pft->par->data;
  mresp=pft->nind*(tree->ind.sapwood*par->cn_ratio.sapwood*gtemp_air+
                   tree->ind.root*par->cn_ratio.root*gtemp_soil*pft->phen);
  npp=(assim<mresp) ? assim-mresp : (assim-mresp)*(1-param.r_growth);
  pft->bm_inc+=npp;
  return npp;
} /* of 'npp_tree' */

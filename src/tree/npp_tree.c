/**************************************************************************************/
/**                                                                                \n**/
/**                    n  p  p  _  t  r  e  e  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates daily net primary productivity of trees                \n**/
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

Real npp_tree(Pft *pft,        /**< PFT variables */
              Real gtemp_air,  /**< value of air temperature response function */
              Real gtemp_soil, /**< value of soil temperature response function */
              Real assim       /**< assimilation (gC/m2) */
             )                 /** \return net primary productivity (gC/m2) */
{
  Pfttree *tree;
  const Pfttreepar *par;
  Real mresp,npp;
  Real nc_root,nc_sapwood;
  tree=pft->data;
  par=pft->par->data;
  if(tree->ind.sapwood.carbon>epsilon)
    nc_sapwood=tree->ind.sapwood.nitrogen/tree->ind.sapwood.carbon;
  else
    nc_sapwood=par->nc_ratio.sapwood;
  if(tree->ind.root.carbon>epsilon)
    nc_root=tree->ind.root.nitrogen/tree->ind.root.carbon;
  else
    nc_root=par->nc_ratio.root;
  mresp=pft->nind*(tree->ind.sapwood.carbon*pft->par->respcoeff*param.k*nc_sapwood*gtemp_air+
        tree->ind.root.carbon*pft->par->respcoeff*param.k*nc_root*gtemp_soil*pft->phen);
  npp=(assim<mresp) ? assim-mresp : (assim-mresp)*(1-param.r_growth);
  pft->bm_inc.carbon+=npp;
  return npp;
} /* of 'npp_tree' */

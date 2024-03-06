/**************************************************************************************/
/**                                                                                \n**/
/**               n  d  e  m  a  n  d  _  t  r  e  e  .  c                         \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates nitrogen demand for trees                              \n**/
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

Real ndemand_tree(const Pft *pft,     /**< pointer to PFT */
                  Real *ndemand_leaf, /**< N demand of leaf (gN/m2) */
                  Real vmax,          /**< vmax (gC/m2/day) */
                  Real daylength,     /**< day length (h) */
                  Real temp           /**< temperature (deg C) */
                 )                    /** \return total N demand  (gN/m2) */
{
  Real nc_ratio;
  const Pfttree *tree;
  const Pfttreepar *treepar;
  Real ndemand_tot;
  tree=pft->data; 
  treepar=pft->par->data;
  //*ndemand_leaf=((daylength==0) ?  0: param.p*0.02314815/daylength*vmax*exp(-param.k_temp*(temp-25))*f_lai(lai_tree(pft)))+param.n0*0.001*(pft->bm_inc.carbon*tree->falloc.leaf+tree->ind.leaf.carbon*pft->nind);
  //*ndemand_leaf=((daylength==0) ?  0: param.p*0.02314815/daylength*vmax*exp(-param.k_temp*(temp-25))*f_lai(lai_tree(pft)))+pft->par->ncleaf.low*(pft->bm_inc.carbon*tree->falloc.leaf+tree->ind.leaf.carbon*pft->nind);
  //*ndemand_leaf=((daylength==0) ?  0: param.p*0.02314815/daylength*vmax*exp(-param.k_temp*(temp-25))*f_lai(lai_tree(pft)))+pft->par->ncleaf.low*(tree->ind.leaf.carbon*pft->nind);
  //*ndemand_leaf=((daylength==0) ?  0: param.p*0.02314815/daylength*vmax*exp(-param.k_temp*(temp-25))*f_lai(lai_tree(pft)))+pft->par->ncleaf.median*(tree->ind.leaf.carbon*pft->nind);
  *ndemand_leaf=((daylength==0) ?  0: param.p*0.02314815/daylength*vmax*exp(-param.k_temp*(temp-25))*f_lai(lai_tree(pft)))+pft->par->ncleaf.median*(pft->bm_inc.carbon*tree->falloc.leaf+tree->ind.leaf.carbon*pft->nind-tree->turn_litt.leaf.carbon);

  if(tree->ind.leaf.carbon-tree->turn.leaf.carbon+pft->bm_inc.carbon*tree->falloc.leaf/pft->nind==0)
    nc_ratio=pft->par->ncleaf.low;
  else
    nc_ratio=(tree->ind.leaf.nitrogen-tree->turn.leaf.nitrogen+pft->bm_inc.nitrogen*tree->falloc.leaf/pft->nind)/(tree->ind.leaf.carbon-tree->turn.leaf.carbon+pft->bm_inc.carbon*tree->falloc.leaf/pft->nind);
  if(nc_ratio>pft->par->ncleaf.high)
    nc_ratio=pft->par->ncleaf.high;
  else if(nc_ratio<pft->par->ncleaf.low)
    nc_ratio=pft->par->ncleaf.low;
  ndemand_tot=*ndemand_leaf+(tree->ind.root.nitrogen+tree->ind.sapwood.nitrogen-tree->turn.root.nitrogen)*pft->nind+nc_ratio*(tree->excess_carbon*pft->nind+pft->bm_inc.carbon)*(tree->falloc.root/treepar->ratio.root+tree->falloc.sapwood/treepar->ratio.sapwood);
#ifdef DEBUG_N
  fprinttreephys2(stdout,tree->ind,pft->nind);
  printf("\n");
  printf("ndemand_to = %g vegn %g vegc %g ndemand_leaf %g nc_ratio %g %g \n",ndemand_tot,vegn_sum_tree(pft)+pft->bm_inc.nitrogen-tree->ind.heartwood.nitrogen*pft->nind,vegc_sum_tree(pft),*ndemand_leaf,nc_ratio,*ndemand_leaf/(tree->ind.leaf.carbon*pft->nind+pft->bm_inc.carbon*tree->falloc.leaf));
#endif
  return ndemand_tot;
} /* of 'ndemand_tree' */

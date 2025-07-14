/**************************************************************************************/
/**                                                                                \n**/
/**               v  m  a  x  l  i  m  i  t  _  t  r  e  e  .  c                   \n**/
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

Real vmaxlimit_tree(const Pft *pft, /**< pointer to PFT */
                    Real daylength, /**< day length (h) */
                    Real temp       /**< temperature (deg C) */
                   )                /** \return vmax (gC/m2/day) */
{
  Real vmax,vcmax25,Nleaf,leafarea;
  const Pfttree *tree;
  tree=pft->data; 
  //vmax=(pft->nleaf-param.n0*0.001*(pft->bm_inc.carbon*tree->falloc.leaf+tree->ind.leaf.carbon*pft->nind-tree->turn_litt.leaf.carbon)/CCpDM)/exp(-param.k_temp*(temp-25))/f_lai(lai_tree(pft))/param.p/9.6450617e-4;
  leafarea=tree->ind.leaf.carbon*pft->par->sla*pft->nind*pft->phen;
  if(leafarea>0)
  { 
    Nleaf=pft->nleaf/leafarea; // from gN/m2 leaf area to gN/m2 ground area
    if(pft->par->phenology==EVERGREEN)
      vcmax25=(Nleaf - 0.596 - 0.006*pft->par->nfixing/(pft->par->sla*CCpDM) + 0.146*pft->par->nfixing - 0.008/(pft->par->sla*CCpDM))/0.006;
    else
      vcmax25=(Nleaf - 0.267 - 0.001*pft->par->nfixing/(pft->par->sla*CCpDM) - 0.444*pft->par->nfixing - 0.014/(pft->par->sla*CCpDM))/0.006;
    vmax=vcmax25/exp(66530/8.314*(1/degCtoK(temp)-1/degCtoK(25)))*(NSECONDSDAY*cmass*1e-6)*leafarea;
  }
  else
  {
    vmax=0;
  }
  return min(pft->vmax,max(vmax,0.0001));
} /* of 'vmaxlimit_tree' */

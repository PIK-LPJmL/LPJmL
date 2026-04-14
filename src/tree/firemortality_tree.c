/**************************************************************************************/
/**                                                                                \n**/
/**                f i r e m o r t a l i t y _ t r e e . c                         \n**/
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

Real firemortality_tree(Pft *pft,const Fuel *fuel, Livefuel *livefuel,
                        Real surface_fi, Real fire_frac)
{

  Pfttree *tree;
  const Pfttreepar *treepar;
  Real tau_c,tau_l,crown_length_tree,scorch_height;
  Real ck,postfire_mort_ck;
  Real postfire_mort_total;
  Real nind_fa,nind_kill;

  tree=pft->data;
  treepar=getpftpar(pft,data);

  livefuel->disturb=ck=postfire_mort_ck=0;

  /* tree architecture*/

  tau_c=2.9 * tree->barkthickness * tree->barkthickness;
  crown_length_tree=tree->height*treepar->crownlength;

  /* scorch height per PFT */

  scorch_height=treepar->scorchheight_f_param*pow(surface_fi,0.667);

  /* residence time calculated as in Albini 1976 */
  tau_l=(fuel->char_sigma>0) ? 5*384/30.48/fuel->char_sigma : 0;

  /* crown kill in [%] assuming the crown shape being a cylinder
   * crown height as a fraction of tree height definded per PFT
   * propn of canopy burnt = (SH - (height - cl))/cl = (SH - height + cl)/cl
   */

  if (scorch_height < (tree->height - crown_length_tree))
    ck=0.0;
  else if(scorch_height < tree->height)
    ck=((scorch_height - tree->height + crown_length_tree)*(tree->height - scorch_height + crown_length_tree))/pow(crown_length_tree,2);
  else
    ck=1.0;

  /*Peterson & Ryan mortality*/
  postfire_mort_total= (tau_l > 0 && ck > 0) ? pow(ck,tau_c/tau_l-0.5) : 0;
  if(postfire_mort_total > 1)
    postfire_mort_total = 1;
#ifdef SAFE
  if(postfire_mort_total > 1)
  {
    printf("postfire_mort_total = %f, ck = %f, tau_c = %f, tau_l = %f in firemortality_tree.c\n",postfire_mort_total, ck, tau_c, tau_l);
    fflush(stdout);
  }
#endif
  /* number of indivs affected by fire in grid cell */
  nind_fa=fire_frac * pft->nind;
  nind_kill = postfire_mort_total * nind_fa;
  if(nind_kill<0)
    nind_kill=0;
  /* live fuel consumption of fire affected plant individuals */
  livefuel->disturb = fire_frac * ck;
  livefuel->non_combust = nind_kill * (1-ck);
  return nind_kill;
} /* of 'firemortality_tree' */

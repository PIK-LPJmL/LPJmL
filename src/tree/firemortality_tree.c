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
  Real ck,postfire_mort_ck,postfire_mort_camb;
  Real postfire_mort_total;
  Real nind_fa,nind_kill;

  tree=pft->data;
  treepar=getpftpar(pft,data);

  livefuel->disturb=ck=postfire_mort_ck=0;

  /* tree architecture*/

  tau_c=2.9 * tree->barkthickness * tree->barkthickness;
  crown_length_tree=tree->height*treepar->crownlength;

  /*scorch height per PFT */

  scorch_height=treepar->scorchheight_f_param*pow(surface_fi,0.667);

  /* post-fire mortality from cambial damage */
  /*     tau_r=cf/Gamma, tau_l=tau_r, if not tau_l=2*tau_r */
  if (fuel->gamma <= 0)
    tau_l = 0;
  else
    tau_l=2.0*(fuel->cf/fuel->gamma);

  /* crown kill in [%] assuming the crown shape being a cylinder
   * crown height as a fraction of tree height definded per PFT
   * propn of canopy burnt = (SH - (height - cl))/cl = (SH - height + cl)/cl
   */

  if (scorch_height < (tree->height - crown_length_tree))
    ck=0.0;
  else if(scorch_height < tree->height)
    ck=(scorch_height - tree->height + crown_length_tree) / crown_length_tree;
  else
    ck=1.0;

  /*post-fire mortality from crown scorching */
  postfire_mort_ck = treepar->crown_mort_rck*pow(ck,treepar->crown_mort_p);

  /* post-fire mortality from cambial damage */
  /* Allan's version after Peterson&Ryan */
  if(tau_c>0)
  {
    if(tau_l/tau_c>=1.9)
      postfire_mort_camb=1.0;
    else if(tau_l/tau_c>0.22)
      postfire_mort_camb=(0.562525*tau_l/tau_c)-0.125;
    else
      postfire_mort_camb=0.0;
  }
  else
    postfire_mort_camb=0.0;

#ifdef SAFE
  if(postfire_mort_camb > 1)
  {
    printf("postfire_mort_camb = %f, tau_l/tau_c = %f, in firemortality_tree.c\n",postfire_mort_camb,(tau_l/tau_c));
    fflush(stdout);
  }
#endif

  /*Calculate total post-fire mortality from crown scorching AND cambial kill*/
  postfire_mort_total=postfire_mort_camb+postfire_mort_ck-(postfire_mort_camb*postfire_mort_ck);
  /*number of indivs affected by fire in grid cell */
#ifdef SAFE
  if(postfire_mort_total > 1)
  {
    printf("postfire_mort_total = %f in firemortality_tree.c\n",postfire_mort_total);
    fflush(stdout);
  }
#endif
  nind_fa=fire_frac * pft->nind;
  nind_kill = postfire_mort_total * nind_fa;
  if(nind_kill<0)
    nind_kill=0;
  /* live fuel consumption of fire affected plant individuals */
  livefuel->disturb = fire_frac * ck;
  livefuel->non_combust = nind_kill * (1-ck);
  return nind_kill;
} /* of 'firemortality_tree' */

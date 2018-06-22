/**************************************************************************************/
/**                                                                                \n**/
/**               a  l  b  e  d  o  _  t  r  e  e  .  c                            \n**/
/**                                                                                \n**/
/**     Computation of albedo and FAPAR for a PFT                                  \n**/
/**     Computation of albedo as described by Bob Gallimore (in an e-mail)         \n**/
/**     and used by him in the coupling of FOAM to LPJ.                            \n**/
/**     FAPAR computation added by Matthias Forkel                                 \n**/
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

#define c_fstem      0.70 /* Masking of the ground by stems and branches without leaves. Source: FOAM/LPJ */
#define c_hbot       0.20 /* Height of canopy bottom as fraction of the tree height (0-1) */
#define height_min   0.5

void albedo_tree(Pft *pft,         /**< pointer to PFT variables */
                 Real snowheight,  /**< snowheight (m) */
                 Real snowfraction /**< fractional coverage of snow at the ground */
                )           
{
  Pfttree *tree;
  Real albedo_pft, albedo_green_leaves, albedo_brown_stem, albedo_brown_litter, snow_green_canopy;
  Real frs, frs1, frs2, htop, hbot;

  tree=pft->data;
  /* calculate fraction of snow in canopy */
  htop=hbot=0.0;
  if (tree->height>height_min)
  {
    htop = tree->height;
    hbot = c_hbot*tree->height;
  }

  frs1 = snowfraction*(pft->phen * getpftpar(pft,snowcanopyfrac) + (1 - pft->phen) * (1 - c_fstem));
  frs2 = 1;
  if (snowheight<=hbot)
    frs2 = 0;
  else if (snowheight<htop)
    frs2 = (snowheight-hbot)/(htop-hbot);

  frs = frs1 + (1-frs1)*frs2;
  if (snowheight>htop)
    frs = 1;

  /* albedo of PFT with green foliage */
  albedo_green_leaves = pft->fpc *  pft->phen * getpftpar(pft, albedo_leaf);
  
  /* albedo of PFT without green leaves: albedo from stems  */
  albedo_brown_stem = pft->fpc *  (1 - pft->phen) * c_fstem * getpftpar(pft, albedo_stem);
  
  /* albedo of PFT without green leaves: albedo from background (litter) */
  albedo_brown_litter = pft->fpc * (1 - pft->phen) * (1-c_fstem) * getpftpar(pft, albedo_litter);
  
  /* albedo of PFT (green + brown) */  
  albedo_pft = albedo_green_leaves + albedo_brown_stem + albedo_brown_litter; 
  
  /* total albedo of PFT including snow-covered parts */
  pft->albedo = pft->fpc * frs * c_albsnow + (1 - frs) * albedo_pft;
  
  /* fraction of snow in green leave canopy */
  snow_green_canopy = pft->phen * frs; 
  
  /* FAPAR of green canopy excluding snow */
  pft->fapar = max(epsilon,pft->fpc * ((pft->phen- snow_green_canopy) *(1-getpftpar(pft, albedo_leaf)) -  ((1 - pft->phen)*c_fstem * getpftpar(pft, albedo_stem))));                //lpjml4.2

} /* of 'albedo_tree' */

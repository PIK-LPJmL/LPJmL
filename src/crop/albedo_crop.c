/**************************************************************************************/
/**                                                                                \n**/
/**               a  l  b  e  d  o  _  c  r  o  p  .  c                            \n**/
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
#include "crop.h"

void albedo_crop(Pft *pft,         /**< pointer to PFT variables */
                 Real snowheight,  /**< snowheight (m) */
                 Real snowfraction /**< fractional coverage of snow at the ground */
                )           
{
  Pftcrop *crop;
  Real albedo_pft, albedo_green_leaves, albedo_brown_litter, snow_green_canopy;
  Real frs, frs1, frs2;
  Real R_tr;  //radiation transmitted (frac)

  crop=pft->data;

  /* calculate fraction of snow in canopy */
  frs1 = snowfraction*(pft->phen * getpftpar(pft,snowcanopyfrac) + (1 - pft->phen));
  frs2 = 1;
  if (snowheight<=0)
    frs2 = 0;
  frs = frs1 + (1-frs1)*frs2;
  if (snowheight>0)
    frs = 1;

  /* albedo of PFT with green foliage */
  albedo_green_leaves = pft->fpc * pft->phen * getpftpar(pft, albedo_leaf);
  
  /* albedo of PFT without green foliage (litter background albedo) */
  albedo_brown_litter = pft->fpc * (1 - pft->phen) * getpftpar(pft, albedo_litter);

  /* albedo of PFT (green + brown) */  
  albedo_pft = albedo_green_leaves + albedo_brown_litter;

  /* total albedo of PFT including snow-covered parts */
  pft->albedo = pft->fpc * frs * c_albsnow + (1 - frs) * albedo_pft;
  
  /* fraction of snow in green leave canopy */
  snow_green_canopy = pft->phen * frs; 
    
  /* FAPAR of green canopy excluding snow */
  if(pft->par->id==MAIZE)
    R_tr=1-min(1,max(0,0.2558*(crop->lai-crop->lai_nppdeficit)-0.0024));
  else
    R_tr=(1-exp(-param.k_beer*actual_lai(pft)));
   
  pft->fapar = max(epsilon, R_tr * (pft->phen - snow_green_canopy)*(1-getpftpar(pft, albedo_leaf)));
} /* of 'albedo_crop' */

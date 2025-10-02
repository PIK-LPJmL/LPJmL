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
  Real albedo_green_leaves, albedo_brown_litter, soil_albedo, albedo_soil;
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

  /* FPAR of green canopy excluding snow */
  if(!strcmp(pft->par->name,"maize"))
    R_tr=min(1,max(0,0.2558*max(0.01,crop->lai-crop->lai_nppdeficit)-0.0024));
  else
    R_tr=(1-exp(-pft->par->lightextcoeff*actual_lai(pft)));

  #ifdef COUPLING_WITH_FMS
    VolWatercontent = max(0,soil->w[0]*soil->whc[0]);
    soil_albedo = c_albedo_wet_soil + c_albedo_bare_soil*exp(-decay_alb_moist*VolWatercontent);/*gives the moisture dependence of the bare soil*/
  #else
    soil_albedo=c_albsoil;
  #endif

  /* albedo of PFT with green foliage */
  albedo_green_leaves = R_tr * getpftpar(pft, albedo_leaf);
  /* snow-covered green leaves */
  albedo_green_leaves=albedo_green_leaves*(1-frs)+R_tr*frs*c_albsnow; 

  /* albedo of PFT without green foliage (litter background albedo) */
  albedo_brown_litter = pft->stand->soil.litter.agtop_cover * (1 - R_tr) * getpftpar(pft, albedo_litter);
  /* snow-covered brown litter */
  albedo_brown_litter=albedo_brown_litter*(1-frs)+pft->stand->soil.litter.agtop_cover*(1-R_tr)*frs*c_albsnow;

  /* albedo of soil */
  albedo_soil = (1 - pft->stand->soil.litter.agtop_cover) * (1 - R_tr) * soil_albedo;
  /* snow-covered soil */
  albedo_soil=albedo_soil*(1-frs)+(1-pft->stand->soil.litter.agtop_cover)*(1-R_tr)*pft->stand->soil.snowfraction*c_albsnow;

  /* albedo of PFT (green + brown) */  
  pft->albedo = albedo_green_leaves + albedo_brown_litter + albedo_soil;

  pft->snowcover=frs;
   
  pft->fapar = max(epsilon, R_tr * (1-frs)*(1-getpftpar(pft, albedo_leaf)));
} /* of 'albedo_crop' */

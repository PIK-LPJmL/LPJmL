/**************************************************************************************/
/**                                                                                \n**/
/**                    a  l  b  e  d  o  _  s  t  a  n  d  .  c                    \n**/
/**                                                                                \n**/
/**     Computation of albedo as described by Bob Gallimore (in an e-mail)         \n**/
/**     and used by him in the coupling of FOAM to LPJ.                            \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define c_albAntarctica  0.7
#define c_roughness      0.06 /* Roughness height of vegetation below the canopy. Source: FOAM/LPJ */

Real albedo_stand(Stand *stand /**< pointer to stand */
                 )             /** \return albedo (0..1) */
{
  int p;
  Real albstot;/* Sum of all albs over all pfts */
  Real fbare;  /* Fraction bare soil */
  Real albedo; /* albedo of the stand */
  Real sfr;
  Pft *pft;
  Soil *soil;

  soil = &stand->soil;
  sfr = soil->snowfraction; /* snow fraction in the stand */
  albstot=fbare=albedo=0;
  
  if (stand->soil.par->type == ICE /* ICE */)
  {
    if (stand->cell->coord.lat<-60)
      return c_albAntarctica;
    return c_albsnow;
  }

  foreachpft(pft,p,&stand->pftlist) 
  {
    albedo_pft(pft, soil->snowheight, soil->snowfraction);
    albstot += pft->albedo; 
    fbare += pft->fpc;
  } 
  fbare = max((1-fbare),0.0);

  albedo = albstot + fbare * (sfr * c_albsnow + (1 - sfr) * c_albsoil);
  return albedo;
} /* end of albedo_stand() */

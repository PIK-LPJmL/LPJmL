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
#define c_albedo_bare_soil  0.31 /* albedo of the bare soil as used by Gascoin et al 2009 */
#define c_albedo_wet_soil  0.15  /* see above */
#define decay_alb_moist  12.7    /* Describes how fast the moisture dependence is */

Real albedo_stand(const Stand *stand /**< pointer to stand */
                 )             /** \return albedo (0..1) */
{
  int p;
  Real albstot;/* Sum of all albs over all pfts */
  Real fbare;  /* Fraction bare soil */
  Real albedo; /* albedo of the stand */
  Real sfr;
  Pft *pft;
  const Soil *soil;
  Real soil_albedo;
#ifdef COUPLING_WITH_FMS
  Real VolWatercontent;
#endif

  soil = &stand->soil;
  sfr = soil->snowfraction; /* snow fraction in the stand */
  albstot=fbare=albedo=0;
#ifdef COUPLING_WITH_FMS
  VolWatercontent = max(0,soil->w[0]*soil->whc[0]);
  soil_albedo = c_albedo_wet_soil + c_albedo_bare_soil*exp(-decay_alb_moist*VolWatercontent);/*gives the moisture dependence of the bare soil*/
#else
  soil_albedo=c_albsoil;
#endif
  
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

  albedo = albstot + fbare * (sfr * c_albsnow + (1 - sfr) * soil_albedo);
  return albedo;
} /* end of albedo_stand() */

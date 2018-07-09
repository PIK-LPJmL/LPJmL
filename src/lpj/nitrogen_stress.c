/**************************************************************************************/
/**                                                                                \n**/
/**              n  i  t  r  o  g  e  n  _  s  t  r  e  s  s  .  c                 \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://gitlab.pik-potsdam.de/lpjml/lpjml                             \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real nitrogen_stress(Pft *pft,         /**< PFT */
                     Real temp,        /**< temperature (deg C) */
                     Real daylength,   /**< daylength (h) */
                     int npft,         /**< number of natural PFTs */
                     int nbiomass,     /**< number of biomass types */
                     int ncft          /**< number of crop PFTs */
                    )                  /** \return total N demand (gN/m2) */
{
  Real nplant_demand,ndemand_leaf;
  Real ndemand_leaf_opt;
  nplant_demand=0;
  if(pft->bm_inc.carbon>0)
  {
    nplant_demand=ndemand(pft,&ndemand_leaf,pft->vmax,daylength,temp,npft,nbiomass,ncft)*(1+pft->par->knstore);
    ndemand_leaf_opt=ndemand_leaf;
    /* calculation of limitation in ndemad_leaf is missing */
    if(nplant_demand>pft->bm_inc.nitrogen || pft->bm_inc.nitrogen<2)
      pft->stand->cell->output.mn_uptake+=nuptake(pft,&nplant_demand,&ndemand_leaf,npft,nbiomass,ncft)*pft->stand->frac;
    else if(pft->stand->type->landusetype!=AGRICULTURE)
      pft->vscal+=1;
    pft->nleaf=max(pft->nleaf,ndemand_leaf);
    if(ndemand_leaf_opt>ndemand_leaf)
    {
      pft->vmax=vmaxlimit(pft,daylength,temp);
#ifdef DEBUG_N
      printf("vmax_limit=%g  vscal=%g\n\n",pft->vmax,pft->vscal);
#endif
    }
  }
  else if(pft->stand->type->landusetype!=AGRICULTURE)
     pft->vscal+=1;
  return nplant_demand;
} /* of 'nitrogen_stress' */

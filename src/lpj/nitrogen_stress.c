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
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Real nitrogen_stress(Pft *pft,             /**< PFT */
                     Real temp,            /**< temperature (deg C) */
                     Real daylength,       /**< daylength (h) */
                     Real aet_layer[LASTLAYER], /**< [inout] layer-specific transpiration (mm/day) */
                     Real npp,
                     int npft,             /**< number of natural PFTs */
                     int ncft,             /**< number of crop PFTs */
                     const Config *config  /**< LPJmL configuration  */
                    )                      /** \return total N demand (gN/m2) */
{
  Real nplant_demand,ndemand_leaf;
  Real ndemand_leaf_opt;
  Real nup;
#ifdef DEBUG_N
  Real nplant_demand_opt;
  nplant_demand_opt=0;
#endif
  nplant_demand=0;
  nup=0;
  if(pft->bm_inc.carbon>0 || ((pft->stand->type->landusetype==GRASSLAND || (pft->stand->type->landusetype==OTHERS && !config->others_to_crop) || pft->stand->type->landusetype==BIOMASS_GRASS|| pft->stand->type->landusetype==SETASIDE_RF || pft->stand->type->landusetype==SETASIDE_IR) && pft->bm_inc.carbon>=0))
  {
    nplant_demand=ndemand(pft,&ndemand_leaf,pft->vmax,daylength,temp)*(1+pft->par->knstore);
    ndemand_leaf_opt=ndemand_leaf;
#ifdef DEBUG_N
    nplant_demand_opt=nplant_demand;
#endif
    /* calculation of limitation in ndemad_leaf is missing */
    if(nplant_demand>pft->bm_inc.nitrogen || pft->bm_inc.nitrogen<2)  //nuptake happens always if nitrogen bm_inc< 2
    {
      if(pft->par->nfixing && config->npp_controlled_bnf)
        pft->npp_bnf=npp-pft->npp_nrecovery;
      nup=nuptake(pft,&nplant_demand,&ndemand_leaf,npft,ncft,config);
    }
    else if(pft->stand->type->landusetype!=AGRICULTURE  && (pft->stand->type->landusetype!=OTHERS || !config->others_to_crop))
      pft->vscal+=1;
    getoutput(&pft->stand->cell->output,NUPTAKE,config)+=nup*pft->stand->frac;
    if(isagriculture(pft->stand->type->landusetype))
      getoutput(&pft->stand->cell->output,NUPTAKE_AGR,config)+=nup*pft->stand->frac;
    pft->nleaf=max(0,ndemand_leaf);
    if(ndemand_leaf_opt>ndemand_leaf)
    {
      pft->vmax=vmaxlimit(pft,daylength,temp);
#ifdef DEBUG_N
      printf("PFT: %s vmax_limit=%g  vscal=%g ndemand_leaf_opt:%g nleaf:%g nplant_demand_opt:%g nplant_demand:%g\n\n",pft->par->name,pft->vmax,pft->vscal,ndemand_leaf_opt,ndemand_leaf,nplant_demand_opt,nplant_demand);
#endif
    }
  }
  else if(pft->stand->type->landusetype!=AGRICULTURE  && (pft->stand->type->landusetype!=OTHERS || !config->others_to_crop))
    pft->vscal+=1;
  return nplant_demand;
} /* of 'nitrogen_stress' */

/**************************************************************************************/
/**                                                                                \n**/
/**                    n  p  p  _  c  r  o  p  .  c                                \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function calculates daily net primary productivity of crops                \n**/
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

/*
- called in npp_crop()
- calculation of the vegetation growth:
  -> calculation of carbon biomass (total plant growth)
  -> calculation of carbon mass root (root growth)
  -> calculation of carbon mass leaf (leaf growth)
  -> calculation of carbon mass so (storage organs growth)
  -> calculation of carbon mass pool (additional pool stem + reserve)
*/

Real npp_crop(Pft *pft,           /**< [inout] PFT variables */
              Real gtemp_air,     /**< [in] value of air temperature response function */
              Real gtemp_soil,    /**< [in] value of soil temperature response function */
              Real assim,         /**< [in] assimilation (gC/m2/day) */
              Bool *negbm,        /**< [out] on return: biomass is negative */
              Real wdf,           /**< [in] water deficit fraction */
              const Config *config/**< [in] LPJmL configuration */
             )                    /** \return net primary productivity (gC/m2/day) */
{
  Pftcrop *crop;
  const Pftcroppar *par;
  Real npp;
  Real rosoresp,presp,gresp;
  Cropratio nc_ratio;
  crop=pft->data;
  par=pft->par->data;
  if(!config->crop_resp_fix && crop->ind.root.carbon>epsilon)
    nc_ratio.root=crop->ind.root.nitrogen/crop->ind.root.carbon;
  else
    nc_ratio.root=par->nc_ratio.root;
  if(!config->crop_resp_fix && crop->ind.so.carbon>epsilon)
    nc_ratio.so=crop->ind.so.nitrogen/crop->ind.so.carbon;
  else
    nc_ratio.so=par->nc_ratio.so;
  if(!config->crop_resp_fix && crop->ind.pool.carbon>epsilon)
    nc_ratio.pool=crop->ind.pool.nitrogen/crop->ind.pool.carbon;
  else
    nc_ratio.pool=par->nc_ratio.pool;

  rosoresp=crop->ind.root.carbon*pft->par->respcoeff*param.k*nc_ratio.root*gtemp_soil
           +crop->ind.so.carbon*pft->par->respcoeff*param.k*nc_ratio.so*gtemp_air;
  presp=crop->ind.pool.carbon*pft->par->respcoeff*param.k*nc_ratio.pool*gtemp_air;
  /* pools can't be negative any more as LAI growth and SO allocation is limited by NPP now */
  gresp=(assim-rosoresp-presp)*param.r_growth;
  if(gresp<0.0)
    gresp=0.0;
  npp=assim-rosoresp-presp-gresp;
  if((pft->bm_inc.carbon+npp <=0.0001) ||
      (crop->lai-crop->lai_nppdeficit<=0 && !crop->senescence))
  {
    *negbm=TRUE;
    crop->ind.pool.carbon+=npp;
    pft->bm_inc.carbon+=npp;
  }
  else
    allocation_daily_crop(pft,npp,wdf,config);
  return npp;
} /* of 'npp_crop' */

/*
- called in update_daily():
  -> cell->output.mnpp[month]+=npp(pft,pft->phen,gtemp_air,gtemp_soil,(gpp-rd),
                    &negbm);
- this function returns the computed daily npp
  -> calculation of roresp (root respiration)
  -> calculation of soresp (storage organ respiration)
  -> calculation of presp (pool (stem + reserve) respiration)
  -> calculation of gresp (growth respiration)
  -> calculation of daily npp (net primary production)

- calls the function allocation_daily_crop() if npp>0 und carbon mass pool+npp>0
  -> allocation_daily_crop() needs the determined npp of npp_crop()
- sets negbm to TRUE if the biomass gets negative
  -> cropstand will be deleted in update_daily()
*/

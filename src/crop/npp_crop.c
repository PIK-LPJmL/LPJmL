/**************************************************************************************/
/**                                                                                \n**/
/**                    n  p  p  _  c  r  o  p  .  c                                \n**/
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
#include "crop.h"
#include "agriculture.h"

/*
- called in npp_crop()
- calculation of the vegetation growth:
  -> calculation of carbon biomass (total plant growth) 
  -> calculation of carbon mass root (root growth)
  -> calculation of carbon mass leaf (leaf growth)
  -> calculation of carbon mass so (storage organs growth)
  -> calculation of carbon mass pool (additional pool stem + reserve)
*/

Real npp_crop(Pft *pft, /**< PFT variables */
              Real gtemp_air, /**< value of air temperature response function */
              Real gtemp_soil, /**< value of soil temperature response function */
              Real assim,  /**< assimilation (gC/m2) */
              Bool *negbm, /**< on return: biomass is negative */
              Real wdf,     /**< water deficit fraction */
              Daily_outputs *output  /**< daily output structure */
             ) /** \return net primary productivity (gC/m2) */
{
  Pftcrop *crop;
  const Pftcroppar *par;
  Real npp;
  Real rosoresp,presp,gresp;
  Irrigation *data;
  data=pft->stand->data;
  crop=pft->data;
  par=pft->par->data;

  rosoresp=crop->ind.root*par->cn_ratio.root*gtemp_soil
           +crop->ind.so*par->cn_ratio.so*gtemp_air;
  presp=crop->ind.pool*par->cn_ratio.pool*gtemp_air;
  /* pools can't be negative any more as LAI growth and SO allocation is limited by NPP now */
  gresp=(assim-rosoresp-presp)*param.r_growth;
  if(gresp<0.0)
    gresp=0.0;
  npp=assim-rosoresp-presp-gresp;
  if((pft->bm_inc+npp <=0.0001) ||
      (crop->lai-crop->lai_nppdeficit<=0 && !crop->senescence))
  {
    *negbm=TRUE;
    crop->ind.pool+=npp;
    pft->bm_inc+=npp; 
  } 
  else 
    allocation_daily_crop(pft,npp,wdf,output);
  if(output!=NULL && output->cft==pft->par->id &&
     output->irrigation==data->irrigation)
  {
    output->rroot=crop->ind.root*par->cn_ratio.root*gtemp_soil;
    output->rso=crop->ind.so*par->cn_ratio.so*gtemp_air;
    output->rpool=presp;
    output->gresp=gresp;
    output->npp=npp;
    output->cleaf=crop->ind.leaf;
    output->croot=crop->ind.root;
    output->cso=crop->ind.so;
    output->cpool=crop->ind.pool;
    output->wdf=wdf;
    output->wscal=pft->wscal;
  }
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

/**************************************************************************************/
/**                                                                                \n**/
/**              i  n  i  t  s  o  i  l  .  c                                      \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function initializes soil variables                                        \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define checkptr(ptr) if(ptr==NULL) { printallocerr(#ptr); return TRUE;}

Bool initsoil(Soil *soil,             /**< Pointer to soil data */
              const Soilpar *soilpar, /**< soil parameter array */
              int ntotpft             /**< number of PFT including crops*/
             )                        /** \return TRUE on error */
{
  int l,p;
  soil->par=soilpar;
  forrootsoillayer(l)
  {
    soil->cpool[l].fast=soil->cpool[l].slow=soil->k_mean[l].fast=soil->k_mean[l].slow=0.0;
    soil->c_shift_fast[l]=newvec(Real,ntotpft);
    checkptr(soil->c_shift_fast[l]);
    soil->c_shift_slow[l]=newvec(Real,ntotpft);
    checkptr(soil->c_shift_slow[l]);
  }
  soil->YEDOMA=soil->alag=soil->amp=soil->meanw1=soil->decomp_litter_mean=0.0;
  soil->snowpack=0.0;
#ifdef MICRO_HEATING
  soil->litter.decomC=0.0;
#endif
  soil->w_evap=0.0;
  soil->count=0;
  for (l=0;l<NSOILLAYER;l++)
  {
    soil->w[l]=0.0;
    soil->w_fw[l]=0.0;
    soil->ice_fw[l]=0.0;
    soil->ice_depth[l]=0;
    soil->freeze_depth[l]=0;
    soil->ice_pwp[l]=0;
    soil->state[l]=NOSTATE;
    soil->perc_energy[l]=0;
#ifdef MICRO_HEATING
    soil->micro_heating[l]=0;
    soil->decomC[l]=0;
#endif
  }
  for (p=0;p<ntotpft;p++)
  {
    soil->c_shift_fast[0][p]=1;
    soil->c_shift_slow[0][p]=1;
  }
  for (l=1;l<LASTLAYER;l++)
    for (p=0;p<ntotpft;p++)
    {
      soil->c_shift_fast[l][p]=0;
      soil->c_shift_slow[l][p]=0;
    }
  soil->maxthaw_depth=2;
  soil->mean_maxthaw=layerbound[BOTTOMLAYER];
  for(l=0;l<NSOILLAYER+1;++l)
    soil->temp[l]=0.0;
  for (l=0;l<=NFUELCLASS;l++)
    soil->litter.avg_fbd[l]=0.0;
  soil->snowheight=soil->snowfraction=soil->rw_buffer=0;
  return FALSE;
} /* of 'initsoil' */

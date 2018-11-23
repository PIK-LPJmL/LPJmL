/**************************************************************************************/
/**                                                                                \n**/
/**              f  r  e  a  d  s  o  i  l  .  c                                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads soil data from file                                         \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool freadsoil(FILE *file,             /**< pointer to binary file */
               Soil *soil,             /**< Pointer to soil data */
               const Soilpar *soilpar, /**< soil parameter array */
               const Pftpar pftpar[],  /**< PFT parameter array */
               int ntotpft,            /**< total number of PFTs */
               Bool swap   /**< Byte order has to be changed (TRUE/FALSE) */
              )            /** \return TRUE on error */
{
  int l;
  soil->par=soilpar;
  forrootsoillayer(l)
  {
    freadreal((Real *)&soil->cpool[l],sizeof(Pool)/sizeof(Real),swap,file);
    soil->c_shift_fast[l]=newvec(Real,ntotpft);
    if(soil->c_shift_fast[l]==NULL)
    {
      printallocerr("c_shift_fast");
      return TRUE;
    }
    soil->c_shift_slow[l]=newvec(Real,ntotpft);
    if(soil->c_shift_slow[l]==NULL)
    {
      printallocerr("c_shift_slow");
      return TRUE;
    }
    freadreal(soil->c_shift_fast[l],ntotpft,swap,file);
    freadreal(soil->c_shift_slow[l],ntotpft,swap,file);
  }
  if(freadlitter(file,&soil->litter,pftpar,ntotpft,swap))
    return TRUE;
  freadreal(soil->w,NSOILLAYER,swap,file);
  freadreal1(&soil->w_evap,swap,file);
  freadreal(soil->w_fw,NSOILLAYER,swap,file);
  freadreal1(&soil->snowpack,swap,file);
  freadreal(soil->temp,NSOILLAYER+1,swap,file);
  freadreal(soil->ice_depth,NSOILLAYER,swap,file);
  freadreal(soil->ice_fw,NSOILLAYER,swap,file);
  freadreal(soil->freeze_depth,NSOILLAYER,swap,file);
  freadreal(soil->ice_pwp,NSOILLAYER,swap,file);
  freadreal(soil->perc_energy,NSOILLAYER,swap,file);
  freadshort(soil->state,NSOILLAYER,swap,file);
  freadreal1(&soil->mean_maxthaw,swap,file);
  freadreal1(&soil->alag,swap,file);
  freadreal1(&soil->amp,swap,file);
  freadreal1(&soil->rw_buffer,swap,file);
#ifdef MICRO_HEATING
  foreachsoillayer(l) soil->decomC[l]=soil->micro_heating[l]=0;
#endif
  freadreal((Real *)soil->k_mean,LASTLAYER*sizeof(Pool)/sizeof(Real),swap,file);
  freadreal1(&soil->decomp_litter_mean,swap,file);
  freadint1(&soil->count,swap,file);
#ifdef MICRO_HEATING
  soil->litter.decomC=0;
#endif
  soil->YEDOMA=0;
  soil->snowheight=soil->snowfraction=0;
  return (freadreal1(&soil->meanw1,swap,file)!=1);
} /* of 'freadsoil' */

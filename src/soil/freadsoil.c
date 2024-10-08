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
    freadreal((Real *)&soil->pool[l],sizeof(Pool)/sizeof(Real),swap,file);
    soil->c_shift[l]=newvec(Poolpar,ntotpft);
    if(soil->c_shift[l]==NULL)
    {
      printallocerr("c_shift");
      return TRUE;
    }
    freadreal((Real *)soil->c_shift[l],ntotpft*sizeof(Poolpar)/sizeof(Real),swap,file);
  }
  if(freadlitter(file,&soil->litter,pftpar,ntotpft,swap))
  {
    fprintf(stderr,"ERROR254: Cannot read litter data.\n");
    return TRUE;
  }
  freadreal(soil->NO3,LASTLAYER,swap,file);
  freadreal(soil->NH4,LASTLAYER,swap,file);
  freadreal(soil->wsat, NSOILLAYER, swap, file);
  freadreal(soil->wpwp, NSOILLAYER, swap, file);
  freadreal(soil->wfc, NSOILLAYER, swap, file);
  freadreal(soil->whc, NSOILLAYER, swap, file);
  freadreal(soil->whcs, NSOILLAYER, swap, file);
  freadreal(soil->wpwps, NSOILLAYER, swap, file);
  freadreal(soil->wsats, NSOILLAYER, swap, file);
  freadreal(soil->beta_soil,NSOILLAYER, swap,file);
  freadreal(soil->bulkdens, NSOILLAYER, swap, file);
  freadreal(soil->k_dry, NSOILLAYER, swap, file);
  freadreal(soil->Ks, NSOILLAYER, swap, file);
  freadreal(soil->df_tillage, NTILLLAYER, swap, file);
  freadreal(soil->w,NSOILLAYER,swap,file);
  freadreal1(&soil->w_evap,swap,file);
  freadreal(soil->w_fw,NSOILLAYER,swap,file);
  freadreal1(&soil->snowpack,swap,file);
  freadreal1(&soil->snowheight,swap,file);
  freadreal1(&soil->snowfraction,swap,file);
  freadreal(soil->temp,NSOILLAYER+1,swap,file);
  freadreal(soil->enth,NHEATGRIDP,swap,file);
  freadreal(soil->wi_abs_enth_adj,NSOILLAYER,swap,file);
  freadreal(soil->sol_abs_enth_adj,NSOILLAYER,swap,file);
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
  freadreal((Real *)soil->k_mean,LASTLAYER*sizeof(Poolpar)/sizeof(Real),swap,file);
  freadreal((Real *)soil->decay_rate,LASTLAYER*sizeof(Poolpar)/sizeof(Real),swap,file);
  freadreal((Real *)&soil->decomp_litter_mean,sizeof(Stocks)/sizeof(Real),swap,file);
  soil->decomp_litter_pft=newvec(Stocks,ntotpft);
  if(soil->decomp_litter_pft==NULL)
  {
    printallocerr("decomp_litter_pft");
    return TRUE;
  }
  freadreal((Real *)soil->decomp_litter_pft,ntotpft*sizeof(Stocks)/sizeof(Real),swap,file);
  freadint1(&soil->count,swap,file);
#ifdef MICRO_HEATING
  soil->litter.decomC=0;
#endif
  soil->YEDOMA=0;
  return (freadreal1(&soil->meanw1,swap,file)!=1);
} /* of 'freadsoil' */

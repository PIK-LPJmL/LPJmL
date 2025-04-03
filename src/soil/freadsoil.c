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

#define readrealarray2(file,name,var,size,swap) if(readrealarray(file,name,var,size,swap)) return TRUE
#define readreal2(file,name,var,swap) if(readreal(file,name,var,swap)) return TRUE
#define readint2(file,name,var,swap) if(readint(file,name,var,swap)) return TRUE

static Bool freadpool(FILE *file,const char *name,Pool *pool,Bool swap)
{
  if(readstruct(file,name,swap))
    return TRUE;
  if(readstocks(file,"slow",&pool->slow,swap))
    return TRUE;
  if(readstocks(file,"fast",&pool->fast,swap))
    return TRUE;
  return readendstruct(file);
} /* of 'freadpool' */

Bool freadpoolpararray(FILE *file,const char *name,Poolpar *pool,int size,Bool swap)
{
  int i,n;
  if(readarray(file,name,&n,swap))
    return TRUE;
  if(n!=size)
  {
    fprintf(stderr,"ERROR227: Size of %s=%d is not %d.\n",
            name,n,size);
    return TRUE;
  }
  for(i=0;i<size;i++)
  {
    if(readstruct(file,NULL,swap))
      return TRUE;
    if(readreal(file,"slow",&pool[i].slow,swap))
      return TRUE;
    if(readreal(file,"fast",&pool[i].fast,swap))
      return TRUE;
    if(readendstruct(file))
      return TRUE;
  }
  return readendarray(file);
} /* of 'freadpoolopararray' */

Bool freadsoil(FILE *file,             /**< pointer to restart file */
               const char *name,       /**< name of object */
               Soil *soil,             /**< Pointer to soil data */
               const Soilpar *soilpar, /**< soil parameter array */
               const Pftpar pftpar[],  /**< PFT parameter array */
               int ntotpft,            /**< total number of PFTs */
               Bool swap   /**< Byte order has to be changed (TRUE/FALSE) */
              )            /** \return TRUE on error */
{
  int l,size;
  soil->par=soilpar;
  if(readstruct(file,name,swap))
    return TRUE;
  if(readarray(file,"pool",&size,swap))
    return TRUE;
  if(size!=LASTLAYER)
  {
    fprintf(stderr,"ERROR227: Size of pool=%d is not %d.\n",
            size,LASTLAYER);
    return TRUE;
  }
  forrootsoillayer(l)
  {
    if(freadpool(file,NULL,soil->pool+l,swap))
      return TRUE;
  }
  if(readendarray(file))
    return TRUE;
  if(readarray(file,"c_shift",&size,swap))
    return TRUE;
  if(size!=LASTLAYER)
  {
    fprintf(stderr,"ERROR227: Size of c_shift=%d is not %d.\n",
            size,LASTLAYER);
    return TRUE;
  }
  forrootsoillayer(l)
  {
    soil->c_shift[l]=newvec(Poolpar,ntotpft);
    if(soil->c_shift[l]==NULL)
    {
      printallocerr("c_shift");
      return TRUE;
    }
    if(freadpoolpararray(file,NULL,soil->c_shift[l],ntotpft,swap))
      return TRUE;
  }
  if(readendarray(file))
    return TRUE;
  if(freadlitter(file,"litter",&soil->litter,pftpar,ntotpft,swap))
    return TRUE;
  readrealarray2(file,"NO3",soil->NO3,LASTLAYER,swap);
  readrealarray2(file,"NH4",soil->NH4,LASTLAYER,swap);
  readrealarray2(file,"wsat",soil->wsat, NSOILLAYER,swap);
  readrealarray2(file,"wpwp",soil->wpwp, NSOILLAYER,swap);
  readrealarray2(file,"wfc",soil->wfc, NSOILLAYER,swap);
  readrealarray2(file,"whc",soil->whc, NSOILLAYER,swap);
  readrealarray2(file,"whcs",soil->whcs, NSOILLAYER,swap);
  readrealarray2(file,"wpwwps",soil->wpwps, NSOILLAYER,swap);
  readrealarray2(file,"wsats",soil->wsats, NSOILLAYER,swap);
  readrealarray2(file,"beta_soil",soil->beta_soil, NSOILLAYER,swap);
  readrealarray2(file,"bulkdens",soil->bulkdens, NSOILLAYER,swap);
  readrealarray2(file,"k_dry",soil->k_dry, NSOILLAYER,swap);
  readrealarray2(file,"Ks",soil->Ks, NSOILLAYER,swap);
  readrealarray2(file,"df_tillage",soil->df_tillage, NTILLLAYER,swap);
  readrealarray2(file,"w",soil->w,NSOILLAYER,swap);
  readreal2(file,"w_evap",&soil->w_evap,swap);
  readrealarray2(file,"w_fw",soil->w_fw,NSOILLAYER,swap);
  readreal2(file,"snowpack",&soil->snowpack,swap);
  readreal2(file,"snowheight",&soil->snowheight,swap);
  readreal2(file,"snowfraction",&soil->snowfraction,swap);
  readrealarray2(file,"temp",soil->temp,NSOILLAYER+1,swap);
  readrealarray2(file,"enth",soil->enth,NHEATGRIDP,swap);
  readrealarray2(file,"wi_abs_enth_adj",soil->wi_abs_enth_adj,NSOILLAYER,swap);
  readrealarray2(file,"soil_abs_enth_adj",soil->sol_abs_enth_adj,NSOILLAYER,swap);
  readrealarray2(file,"ice_depth",soil->ice_depth,NSOILLAYER,swap);
  readrealarray2(file,"ice_fw",soil->ice_fw,NSOILLAYER,swap);
  readrealarray2(file,"freeze_depth",soil->freeze_depth,NSOILLAYER,swap);
  readrealarray2(file,"ice_pwp",soil->ice_pwp,NSOILLAYER,swap);
  readrealarray2(file,"perc_energy",soil->perc_energy,NSOILLAYER,swap);
  if(readshortarray(file,"state",soil->state,NSOILLAYER,swap))
    return TRUE;
  readreal2(file,"mean_maxthaw",&soil->mean_maxthaw,swap);
  readreal2(file,"alag",&soil->alag,swap);
  readreal2(file,"amp",&soil->amp,swap);
  readreal2(file,"rw_buffer",&soil->rw_buffer,swap);
  if(freadpoolpararray(file,"k_mean",soil->k_mean,LASTLAYER,swap))
    return TRUE;
  if(freadpoolpararray(file,"decay_rate",soil->decay_rate,LASTLAYER,swap))
    return TRUE;
  if(readstocks(file,"decomp_litter_mean",&soil->decomp_litter_mean,swap))
    return TRUE;
  soil->decomp_litter_pft=newvec(Stocks,ntotpft);
  if(soil->decomp_litter_pft==NULL)
  {
    printallocerr("decomp_litter_pft");
    return TRUE;
  }
  if(readstocksarray(file,"decomp_litter_pft",soil->decomp_litter_pft,ntotpft,swap))
    return TRUE;
  readint2(file,"count",&soil->count,swap);
  readreal2(file,"meanw1",&soil->meanw1,swap);
#ifdef MICRO_HEATING
  soil->litter.decomC=0;
#endif
  soil->YEDOMA=0;
  return readendstruct(file);
} /* of 'freadsoil' */

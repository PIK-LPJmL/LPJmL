/**************************************************************************************/
/**                                                                                \n**/
/**              f  r  e  a  d  s  o  i  l  .  c                                   \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function reads soil data from restart file                                 \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

#define readrealarray(file,name,var,size) if(bstruct_readrealarray(file,name,var,size)) return TRUE
#define readreal(file,name,var) if(bstruct_readreal(file,name,var)) return TRUE
#define readint(file,name,var) if(bstruct_readint(file,name,var)) return TRUE

Bool freadpoolpararray(Bstruct file,const char *name,Poolpar *pool,int size)
{
  int i,n;
  if(bstruct_readarray(file,name,&n))
    return TRUE;
  if(n!=size)
  {
    fprintf(stderr,"ERROR227: Size of %s=%d is not %d.\n",
            name,n,size);
    return TRUE;
  }
  for(i=0;i<size;i++)
  {
    if(bstruct_readstruct(file,NULL))
      return TRUE;
    readreal(file,"slow",&pool[i].slow);
    readreal(file,"fast",&pool[i].fast);
    if(bstruct_readendstruct(file,name))
      return TRUE;
  }
  return bstruct_readendarray(file,name);
} /* of 'freadpoolpararray' */

Bool freadsoil(Bstruct file,           /**< pointer to restart file */
               const char *name,       /**< name of object */
               Soil *soil,             /**< Pointer to soil data */
               const Soilpar *soilpar, /**< soil parameter array */
               const Pftpar pftpar[],  /**< PFT parameter array */
               int ntotpft             /**< total number of PFTs */
              )            /** \return TRUE on error */
{
  int l,size;
  soil->par=soilpar;
  if(bstruct_readstruct(file,name))
    return TRUE;
  if(bstruct_readarray(file,"pool",&size))
    return TRUE;
  if(size!=LASTLAYER)
  {
    fprintf(stderr,"ERROR227: Size of pool=%d is not %d.\n",
            size,LASTLAYER);
    return TRUE;
  }
  forrootsoillayer(l)
  {
    if(freadpool(file,NULL,soil->pool+l))
    {
      fprintf(stderr,"ERROR227: Cannot read pool for layer %d.\n",l);
      return TRUE;
    }
  }
  if(bstruct_readendarray(file,"pool"))
    return TRUE;
  if(bstruct_readarray(file,"c_shift",&size))
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
    if(freadpoolpararray(file,NULL,soil->c_shift[l],ntotpft))
      return TRUE;
  }
  if(bstruct_readendarray(file,"c_shift"))
    return TRUE;
  if(freadlitter(file,"litter",&soil->litter,pftpar,ntotpft))
    return TRUE;
  readrealarray(file,"NO3",soil->NO3,LASTLAYER);
  readrealarray(file,"NH4",soil->NH4,LASTLAYER);
  readrealarray(file,"wsat",soil->wsat, NSOILLAYER);
  readrealarray(file,"wpwp",soil->wpwp, NSOILLAYER);
  readrealarray(file,"wfc",soil->wfc, NSOILLAYER);
  readrealarray(file,"whc",soil->whc, NSOILLAYER);
  readrealarray(file,"whcs",soil->whcs, NSOILLAYER);
  readrealarray(file,"wpwwps",soil->wpwps, NSOILLAYER);
  readrealarray(file,"wsats",soil->wsats, NSOILLAYER);
  readrealarray(file,"beta_soil",soil->beta_soil, NSOILLAYER);
  readrealarray(file,"bulkdens",soil->bulkdens, NSOILLAYER);
  readrealarray(file,"k_dry",soil->k_dry, NSOILLAYER);
  readrealarray(file,"Ks",soil->Ks, NSOILLAYER);
  readrealarray(file,"df_tillage",soil->df_tillage, NTILLLAYER);
  readrealarray(file,"w",soil->w,NSOILLAYER);
  readreal(file,"w_evap",&soil->w_evap);
  readrealarray(file,"w_fw",soil->w_fw,NSOILLAYER);
  readreal(file,"snowpack",&soil->snowpack);
  readreal(file,"snowheight",&soil->snowheight);
  readreal(file,"snowfraction",&soil->snowfraction);
  readrealarray(file,"temp",soil->temp,NSOILLAYER+1);
  readrealarray(file,"enth",soil->enth,NHEATGRIDP);
  readrealarray(file,"wi_abs_enth_adj",soil->wi_abs_enth_adj,NSOILLAYER);
  readrealarray(file,"soil_abs_enth_adj",soil->sol_abs_enth_adj,NSOILLAYER);
  readrealarray(file,"ice_depth",soil->ice_depth,NSOILLAYER);
  readrealarray(file,"ice_fw",soil->ice_fw,NSOILLAYER);
  readrealarray(file,"freeze_depth",soil->freeze_depth,NSOILLAYER);
  readrealarray(file,"ice_pwp",soil->ice_pwp,NSOILLAYER);
  readrealarray(file,"perc_energy",soil->perc_energy,NSOILLAYER);
  if(bstruct_readshortarray(file,"state",soil->state,NSOILLAYER))
    return TRUE;
  readreal(file,"mean_maxthaw",&soil->mean_maxthaw);
  readreal(file,"alag",&soil->alag);
  readreal(file,"amp",&soil->amp);
  readreal(file,"rw_buffer",&soil->rw_buffer);
  if(freadpoolpararray(file,"k_mean",soil->k_mean,LASTLAYER))
    return TRUE;
  if(freadpoolpararray(file,"decay_rate",soil->decay_rate,LASTLAYER))
    return TRUE;
  if(freadstocks(file,"decomp_litter_mean",&soil->decomp_litter_mean))
    return TRUE;
  soil->decomp_litter_pft=newvec(Stocks,ntotpft);
  if(soil->decomp_litter_pft==NULL)
  {
    printallocerr("decomp_litter_pft");
    return TRUE;
  }
  if(freadstocksarray(file,"decomp_litter_pft",soil->decomp_litter_pft,ntotpft))
    return TRUE;
  readint(file,"count",&soil->count);
  readreal(file,"meanw1",&soil->meanw1);
#ifdef MICRO_HEATING
  soil->litter.decomC=0;
#endif
  soil->YEDOMA=0;
  return bstruct_readendstruct(file,name);
} /* of 'freadsoil' */

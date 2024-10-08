/**************************************************************************************/
/**                                                                                \n**/
/**               f  w  r  i  t  e  s  o  i  l  .  c                               \n**/
/**                                                                                \n**/
/**     C implementation of LPJmL                                                  \n**/
/**                                                                                \n**/
/**     Function write soil variables into binary file                             \n**/
/**                                                                                \n**/
/** (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file    \n**/
/** authors, and contributors see AUTHORS file                                     \n**/
/** This file is part of LPJmL and licensed under GNU AGPL Version 3               \n**/
/** or later. See LICENSE file or go to http://www.gnu.org/licenses/               \n**/
/** Contact: https://github.com/PIK-LPJmL/LPJmL                                    \n**/
/**                                                                                \n**/
/**************************************************************************************/

#include "lpj.h"

Bool fwritesoil(FILE *file, /**< pointer to binary file */
                const Soil *soil, /* soil data to be written */
                int ntotpft /**< total number of PFTs */
               )            /** \return TRUE on error */
{
  int l;
  forrootsoillayer(l)
  {
    fwrite1(&soil->pool[l],sizeof(Pool),file);
    fwriten(soil->c_shift[l],sizeof(Poolpar),ntotpft,file);
  }
  if(fwritelitter(file,&soil->litter))
    return TRUE;
  fwriten(soil->NO3,sizeof(Real),LASTLAYER,file);
  fwriten(soil->NH4,sizeof(Real),LASTLAYER,file);
  fwriten(soil->wsat, sizeof(Real), NSOILLAYER, file);
  fwriten(soil->wpwp, sizeof(Real), NSOILLAYER, file);
  fwriten(soil->wfc, sizeof(Real), NSOILLAYER, file);
  fwriten(soil->whc, sizeof(Real), NSOILLAYER, file);
  fwriten(soil->whcs, sizeof(Real), NSOILLAYER, file);
  fwriten(soil->wpwps, sizeof(Real), NSOILLAYER, file);
  fwriten(soil->wsats, sizeof(Real), NSOILLAYER, file);
  fwriten(soil->beta_soil, sizeof(Real), NSOILLAYER, file);
  fwriten(soil->bulkdens, sizeof(Real), NSOILLAYER, file);
  fwriten(soil->k_dry, sizeof(Real), NSOILLAYER, file);
  fwriten(soil->Ks, sizeof(Real), NSOILLAYER, file);
  fwriten(soil->df_tillage, sizeof(Real), NTILLLAYER, file);
  fwriten(soil->w,sizeof(Real),NSOILLAYER,file);
  fwrite1(&soil->w_evap,sizeof(Real),file);
  fwriten(soil->w_fw,sizeof(Real),NSOILLAYER,file);
  fwrite1(&soil->snowpack,sizeof(Real),file);
  fwrite1(&soil->snowheight,sizeof(Real),file);
  fwrite1(&soil->snowfraction,sizeof(Real),file);
  fwriten(soil->temp,sizeof(Real),NSOILLAYER+1,file);
  fwriten(soil->enth,sizeof(Real),NHEATGRIDP,file);
  fwriten(soil->wi_abs_enth_adj,sizeof(Real),NSOILLAYER,file);
  fwriten(soil->sol_abs_enth_adj,sizeof(Real),NSOILLAYER,file);
  fwriten(soil->ice_depth,sizeof(Real),NSOILLAYER,file);
  fwriten(soil->ice_fw,sizeof(Real),NSOILLAYER,file);
  fwriten(soil->freeze_depth,sizeof(Real),NSOILLAYER,file);
  fwriten(soil->ice_pwp,sizeof(Real),NSOILLAYER,file);
  fwriten(soil->perc_energy,sizeof(Real),NSOILLAYER,file);
  fwriten(soil->state,sizeof(short),NSOILLAYER,file);
  fwrite1(&soil->mean_maxthaw,sizeof(Real),file);
  fwrite1(&soil->alag,sizeof(Real),file);
  fwrite1(&soil->amp,sizeof(Real),file);
  fwrite1(&soil->rw_buffer,sizeof(Real),file);
  fwrite(soil->k_mean,sizeof(Poolpar),LASTLAYER,file);
  fwrite(soil->decay_rate,sizeof(Poolpar),LASTLAYER,file);
  fwrite1(&soil->decomp_litter_mean,sizeof(Stocks),file);
  fwrite(soil->decomp_litter_pft,sizeof(Stocks),ntotpft,file);
  fwrite1(&soil->count,sizeof(int),file);
  fwrite1(&soil->meanw1,sizeof(Real),file);
  return FALSE;
} /* of 'fwritesoil' */
